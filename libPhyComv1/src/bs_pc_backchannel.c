/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */
/**
 * Functions to setup and utilize "back channels" (for test purposes) in
 * between devices.
 *
 * Back channels are "cheat" communication channels, tests running in device
 * can use to exchange information.
 * They are fully reliable, and do not try to emulate any network or protocol.
 * This channels behave just like unix pipes.
 *
 * A device may open one or several channels to any other device
 * (Note that both devices need to open the channels or the other side will be
 * blocked)
 *
 * Each channel is bidirectional
 * Each channel is assigned a channel_id on creation, all subsequent operations
 * towards that channel will use that channel_id
 *
 * What is carried in the channel is fully up to the user. But note:
 *  * The channel is *non* *blocking*
 *  * Before any read you shall check if there is anything to be read (you will
 *    get an error if you try to read from an empty back channel).
 *  * Writes are also non blocking, if there is no space in the channel the
 *    write will fail:
 *    * You cannot have more pending data in any given channel than 64KB in
 *      Linux
 *    * To avoid problems you should not send messages bigger than PIPE_BUF
 *      ( 4K in Linux, to be POSIX portable 512B ) - 4 bytes
 */

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "bs_pc_base_fifo_user.h"
#include "bs_tracing.h"
#include "bs_oswrap.h"

static bool channel_ever_opened = false;

typedef enum {In=0, Out} direction_t;

typedef struct {
  char *ff_path[2];
  int ff[2];
  int pending_read_bytes; //-1 == channel is closed
  int dev_nbr;
  int channel_nbr;
} channels_status_t;

static channels_status_t *channels_status;
static int number_back_channels = -1;

static uint *channel_id_table = NULL;

/**
 * Close and cleanup the back channel communication
 */
void bs_clean_back_channels(){
  if ( channel_ever_opened ){
    if ( channels_status != NULL ) {
      for (int i = 0; i < number_back_channels ; i ++) {
        for (direction_t dir = In ; dir <= Out; dir++) {
          if ( channels_status[i].ff_path[dir] ) {
            close(channels_status[i].ff[dir]); //Close FIFO
            remove(channels_status[i].ff_path[dir]); //Attempt to delete FIFO
            free(channels_status[i].ff_path[dir]);
          }
        }
      }

      free(channels_status);
      channels_status = NULL;
    }
  }
  if ( pb_com_path != NULL ) {
    rmdir(pb_com_path);
  }
  if ( channel_id_table != NULL ) {
    free(channel_id_table);
    channel_id_table = NULL;
  }
  number_back_channels = 0;
}

/**
 * Open <nbr_of_channels> back channels to other devices,
 * where <global_dev_nbr> is this device global number.
 * <dev_nbrs> are the devices to which to open the channels
 * <channel_nbrs> are the channel numbers to each device (you can have several channels to each device)
 * e.g. to open 2 channels to device 1 and 1 channel to device 5 call like:
 *   device_nbrs[3] = {1,1,5};
 *   channel_numbers[3] = {0,1,0};
 *   number_of_channels = 3;
 *
 * Note that this function can only be called *once*
 *
 * This function is blocking until the other side devices open the corresponding back channels
 * This function returns NULL on failure or
 * an array of channel identifiers to be used in subsequent back channel operations
 * (DO NOT free that pointer)
 *
 */
uint *bs_open_back_channel(uint global_dev_nbr, uint* dev_nbrs, uint* channel_nbrs, uint nbr_of_channels){
  if ( channel_ever_opened )
    bs_trace_error_line("To prevent deadlocks you have to open all channels in one call to %s\n", __func__);

  extern bool is_base_com_initialized;
  if ( ! is_base_com_initialized ){
    bs_trace_error_line("You canNOT call %s before this device has connected to its phy(s)\n", __func__);
  }

  channels_status = bs_calloc(nbr_of_channels, sizeof(channels_status_t));
  channel_ever_opened = true;
  number_back_channels = nbr_of_channels;
  channel_id_table = bs_malloc(nbr_of_channels*sizeof(uint));

  for (direction_t dir = In ; dir <= Out; dir++){
    for (int i = 0 ; i < nbr_of_channels; i ++){

      channels_status[i].ff_path[dir] = (char*)bs_calloc( pb_com_path_length + 50 , sizeof(char));
      if ( dir == In ){
        sprintf(channels_status[i].ff_path[dir], "%s/Device%u_from%u_%u.bc",
                pb_com_path, global_dev_nbr, dev_nbrs[i], channel_nbrs[i]);
      } else {
        sprintf(channels_status[i].ff_path[dir], "%s/Device%u_from%u_%u.bc",
                pb_com_path, dev_nbrs[i], global_dev_nbr, channel_nbrs[i]);
      }

      if ( pb_create_fifo_if_not_there(channels_status[i].ff_path[dir]) != 0 ){
        bs_clean_back_channels();
        free(channel_id_table);
        return NULL;
      }

      if ( dir == In ){
        //Open FIFO not locking for In side
        if ( ( channels_status[i].ff[dir] = open(channels_status[i].ff_path[dir],O_RDONLY | O_NONBLOCK) ) == -1 ) {
          bs_clean_back_channels();
          free(channel_id_table);
          return NULL;
        }
        channels_status[i].pending_read_bytes = 0;
        channel_id_table[i] = i;
        channels_status[i].dev_nbr = dev_nbrs[i];
        channels_status[i].channel_nbr = channel_nbrs[i];
      } else {
        //Open FIFO locking for out side (this will block until the other device opens for reading)
        if ( ( channels_status[i].ff[dir] = open(channels_status[i].ff_path[dir],O_WRONLY ) ) == -1 ) {
          bs_clean_back_channels();
          free(channel_id_table);
          return NULL;
        }
        //Change write side permissions to non locking
        int flags = fcntl(channels_status[i].ff[dir], F_GETFL);
        flags |= O_NONBLOCK;
        fcntl(channels_status[i].ff[dir], F_SETFL, flags);
      }

    } //for i
  } //for dir

  return channel_id_table;
}

/**
 * Send a message to the other device thru the channel
 * Note that if the other device has closed the channel (pipe) == disconnected
 * we will get a SIGPIPE here and will terminate abruptly
 */
void bs_bc_send_msg(uint channel_id, uint8_t *ptr, size_t size){
  if ( channel_id >= number_back_channels )
    bs_trace_error_line("you are trying to send a message thru a non existent back channel (%u)\n", channel_id);

  char message[size+4];
  *(uint32_t*)message = size;
  memcpy(&message[4], ptr, size);
  //To avoid problems we move all data in one write() call.
  //(otherwise the context maybe switched out between writes and the read may fail on the other side)

  int bytes_written = write(channels_status[channel_id].ff[Out], message, size+4);
  if ( bytes_written != size+4 ) {
    bs_trace_error_line("back channel %u filled up (%i != %zu+4, errno=%i)\n",
                        channel_id, bytes_written, size, errno);
  }
}

/**
 * check if there is any pending message in the queue
 * Returns -1 if the channel is closed
 * Returns 0 if nothing is available yet
 * the size of the next message if there is something
 */
int bs_bc_is_msg_received(uint channel_id){
  if ( channel_id >= number_back_channels )
    bs_trace_error_line("you are trying to check for a message in a non existent back channel (%u)\n", channel_id);

  while ( channels_status[channel_id].pending_read_bytes == 0 ){ //otherwise the user is calling this function twice (and we'd break the protocol)
    uint32_t size32;
    int read_size = read(channels_status[channel_id].ff[In],&size32,sizeof(uint32_t)); //non blocking read
    if ( read_size == sizeof(uint32_t) ) {
      channels_status[channel_id].pending_read_bytes = size32;
    } else if ( ( ( read_size == -1 ) && (errno == EAGAIN) ) || (read_size == 0) ) { //Nothing yet there
      break;
    } else if ( ( read_size == -1 ) && (errno == EINTR) ) {
      //A signal interrupted the read before anything was read => retry needed
      bs_trace_warning_line("Read to back channel %u interrupted by signal => Retrying\n",
                            channel_id);
    } else if ( read_size == EOF ) { //The FIFO was closed by the other side
      channels_status[channel_id].pending_read_bytes = -1;
      bs_trace_raw_time(3,"The back channel %u was closed by the other side\n",channel_id);
      break;
    } else {
      bs_trace_error_line("Unexpected error in channel %u (%i read, errno=%i: %s)\n",
                          channel_id, read_size, errno, strerror(errno));
    }
  }
  return channels_status[channel_id].pending_read_bytes;
}

/**
 * Receive a message into <ptr> of <size> bytes from the other side
 *
 * Always call bs_bc_is_msg_received() before to check that there is
 * indeed a message,
 * and always ask for a message of not more than the number of bytes
 * bs_bc_is_msg_received() returned
 */
void bs_bc_receive_msg(int channel_id , uint8_t *ptr, size_t size){
  if ( channel_id >= number_back_channels )
    bs_trace_error_line("You are trying to receive a message in a non existent back channel (%u)\n", channel_id);

  if ( size > channels_status[channel_id].pending_read_bytes )
    bs_trace_error_line("Last time you checked bs_bc_is_msg_received() told there was %u bytes in channel %u, but now you try to read %u??\n",
                         channels_status[channel_id].pending_read_bytes, channel_id, size);

  if ( size == 0 )
    return;

  int read_size = read(channels_status[channel_id].ff[In],ptr,size); //Non-blocking read
  if ( read_size != size ) {
    bs_trace_error_line("Back channel %u broken (%i != %z bytes, errno=%i, pending=%i) "
                        "(probably the other side crashed in the middle of a message == nasty)\n",
                        channel_id, read_size, size, errno, channels_status[channel_id].pending_read_bytes);
  }
  channels_status[channel_id].pending_read_bytes -= size;
}
