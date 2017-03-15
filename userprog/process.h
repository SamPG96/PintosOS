#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include <stdbool.h>

void retrieve_file_name(const char *file_name, char *program_name);
int handle_cmd_args(const char *args, void **esp);
tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);
char* push_args_to_stack(void **esp, char *arg, int arg_length);
void push_header_to_stack(void **esp, int argv, char *argp[]);


#endif /* userprog/process.h */

#define MAX_ARG_SIZE 16
