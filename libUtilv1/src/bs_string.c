/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "bs_string.h"
#include "bs_tracing.h"

/**
 * Return how many chars (digits) a number will occupy as a string
 */
int bs_number_strlen(long long int a) {
  int c = 0;
  if (a < 0) {
    c++;
    a=-a;
  }
  do {
    c++;
    a/=10;
  } while (a != 0);
  return c;
}

/**
 * Convert a bs_time_t into a string.
 * The format will always be: hh:mm:ss.ssssss\0
 *  hour will not wrap at 24 hours, but it will be truncated to 2 digits (so 161hours = 61)
 *
 * Note: the caller has to allocate the destination buffer == 16 chars
 */
char * bs_time_to_str(char* dest, bs_time_t time) {
  if ( time != TIME_NEVER ){
    uint hour;
    uint minute;
    uint second;
    uint us;

    hour   = ( time/3600/1000000 ) % 100;
    minute = ( time/60/1000000 ) % 60;
    second = ( time/1000000 ) % 60;
    us     = time % 1000000;

    sprintf(dest,"%02u:%02u:%02u.%06u",hour, minute, second,us);
  } else {
    sprintf(dest," NEVER/UNKNOWN ");
  }
  return dest;
}

/**
 * Function to dump a set of bytes as a Hexadecimal string
 * The caller allocates <buffer> with at least nbytes*3 + 1 bytes
 */
void bs_hex_dump(char* buffer, const uint8_t *bytes, size_t nbytes) {
  size_t ni,no = 0;
  buffer[0] = 0; //in case nbytes is 0, we terminate the string
  if ( nbytes >= 1 ) {
    for ( ni = 0; ni < nbytes -1; ni++){
      sprintf(&buffer[no], "%02X ", bytes[ni]);
      no +=3;
    }
    sprintf(&buffer[no], "%02X", bytes[nbytes-1]);
  }
}

/**
 * Convert a single hexadecimal (ASCII) char to an integer
 */
static inline uint8_t valuefromhexchar(char a) {
  if ( ( a >= '0' ) && ( a <= '9') ) {
    return a - '0';
  } else if ( ( a >= 'A' ) && ( a <= 'F' ) ) {
    return a - 'A' + 0xA;
  } else if ( ( a >= 'a' ) && ( a <= 'f' ) ) {
    return a - 'a' + 0xA;
  } else {
    bs_trace_error_line("Character '%c' is not valid hexadecimal\n",a);
    return -1;
  }
}

/**
 * Read back into buffer a HexDump of another buffer
 * (maximum size bytes)
 */
void bs_read_hex_dump(char *s,uint8_t *buffer, uint size) {
  uint ni = 0;
  uint no = 0;
  while ( s[ni] != 0 && no < size ){
    if (s[ni] != 0 && s[ni+1]!= 0){
      buffer[no] = valuefromhexchar(s[ni])*16;
      buffer[no] += valuefromhexchar(s[ni+1]);
      ni+=2;
    } else {
      break;
    }
    if ( s[ni]!= 0 )
      ni++;
    no++;
  }
}
