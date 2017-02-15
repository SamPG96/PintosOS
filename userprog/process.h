#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"

void retrieve_file_name(const char *file_name, char *program_name);
int handle_cmd_args(const char *args, void **esp);
tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */

#define MAX_PROGRAM_NAME_SIZE 16
