/*
 * Copyright 2018 Oticon A/S
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include "bs_tracing.h"
#include "bs_types.h"
#include "bs_oswrap.h"
#include "bs_string.h"

#if (defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE>=200809))
/**
 * Set the handler for the signals listed in <signals[<n>]>,
 * Normally <signals> will be set to (int[]){SIGTERM, SIGINT}
 *
 * In some cases (the program is stuck) we may never actually exit.
 * Therefore we set SA_RESETHAND: This way, the 2nd time the signal is received
 * the default handler would be called to terminate the program no matter what.
 *
 * Note that SA_RESETHAND requires either _POSIX_C_SOURCE>=200809 or
 * _XOPEN_SOURCE>=500
 */
void bs_set_sig_term_handler(void (*f)(int), int signals[], int n) {
  struct sigaction act;

  act.sa_handler = f;
  sigemptyset(&act.sa_mask);

  act.sa_flags = SA_RESETHAND;

  for (int i = 0 ; i < n ; i++) {
    sigaction(signals[i], &act, NULL);
  }
}
#else
void bs_set_sig_term_handler(void (*f)(int), int signals[], int n) {
  bs_trace_error_line("Compile with -D_POSIX_C_SOURCE>=200809\n");
}
#endif

#if defined(__linux)
/**
 * Return a process start time (the time the process started after system boot)
 * in whatever format it is stored in /proc/<pid>/stat field 22
 * See man proc(5) for more info
 *
 * Note that the format of /proc/<pid>/stat is not part of any standard or
 * necessarily an stable API. But it has been stable in Linux.
 * In case you are having trouble with this code, you may want to set
 * DONT_READ_PROC_STAT
 */
uint64_t bs_get_process_start_time(long int pid) {
#if DONT_READ_PROC_STAT
  return 0;
#else
  char filename[50];
  FILE *fptr;
  sprintf(filename, "/proc/%li/stat",pid);
  fptr = bs_fopen(filename,"r");

  int c = fgetc(fptr);
  while(c != ')' && c!= EOF) { //If the executable would have a ")" in its name we'd have a problem.. but nevermind that odd case..
    c = fgetc(fptr);
  }
  int count = 1;
  while (count!=21) {
    c = fgetc(fptr);
    if (c==' ')
      count++;
    if (c == EOF)
      return 0;
  }
  long long unsigned int start_time;
  fscanf(fptr,"%llu", &start_time);
  fclose(fptr);
  return start_time;
#endif
}
#endif

/**
 * Wrapper to OS malloc which exits if it cannot allocate mem
 */
void* bs_malloc(size_t size) {
  void* pointer;
  pointer = malloc(size);
  if (pointer == NULL) {
    bs_trace_error_line("Can't allocate memory\n");
  }
  return pointer;
}

/**
 * Wrapper to OS aligned alloc which exits if it cannot allocate mem
 */
void* bs_aligned_alloc(size_t alignment, size_t size) {
  void* memptr;
  if (posix_memalign(&memptr, alignment, size) != 0) {
    bs_trace_error_line("Can't allocate aligned memory\n");
  }
  return memptr;
}

/**
 * Wrapper to OS calloc which exits if it cannot allocate mem
 */
void* bs_calloc(size_t nmemb, size_t size) {
  void* pointer;
  pointer = calloc(nmemb, size);
  if (pointer == NULL) {
    bs_trace_error_line("Can't allocate memory\n");
  }
  return pointer;
}

/**
 * Wrapper to OS realloc which exits if it cannot allocate mem
 */
void* bs_realloc(void *ptr, size_t size) {
  ptr = realloc(ptr, size);
  if (ptr == NULL) {
    bs_trace_error_line("Can't re-allocate memory\n");
  }
  return ptr;
}

/**
 * Move file pointer to the next line
 */
void bs_skipline(FILE *file) {
  char c;
  while (((c=getc(file)) != EOF ) && c!='\n') { }
}

/**
 * like fgets() but doesnt copy the end of line character to the buffer
 */
void bs_readline(char *s, int size, FILE *stream){
  char c;
  uint i = 0;
  while ( ( i < size - 1 ) && ( (c=getc(stream)) != EOF ) && c!='\n'){
    s[i++] =c;
  }
  s[i] = 0;
}

#define NO_FOLDER_WARNING "Can't create or access folder %s\n"
#define NO_FILE_WARNING "Cannot open file %s in mode %s\n"

/*
 * Create a folder if it doesn't exist
 * Accounting for the fact that some other program may be racing to
 * create it at the same time
 *
 * Note: can only create one directory level at a time
 */
int bs_createfolder(const char* folderpath) {
  if (access(folderpath, F_OK) == 0) {
    return 0;
  }
  if (errno != ENOENT) {
    bs_trace_warning_line(NO_FOLDER_WARNING, folderpath);
    return 1;
  }
  if ((mkdir(folderpath ,S_IRWXG | S_IRWXU) != 0) && (access(folderpath, F_OK) != 0)) {
    bs_trace_warning_line(NO_FOLDER_WARNING, folderpath);
    return 1;
  }

  return 0;
}

/**
 * Wrapper to OS fopen which exits if it cannot open the file
 */
FILE* bs_fopen(const char *file_path, const char *open_type) {
  FILE* file_pointer;
  file_pointer = fopen(file_path, open_type);
  if (file_pointer == NULL) {
    bs_trace_error_line(NO_FILE_WARNING, file_path, open_type);
  }
  return file_pointer;
}
