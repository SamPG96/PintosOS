#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>

typedef int pid_t;

#define ARG_CODE 0
#define ARG_1 4
#define ARG_2 8
#define ARG_3 12


void syscall_init (void);
void handle_halt (void);
void handle_exit (int status);
pid_t handle_exec (const char *cmd_line);
int handle_wait (pid_t pid);
bool handle_create (const char *file, unsigned initial_size);
bool handle_remove (const char *file);
int handle_open (const char *file);
int handle_filesize (int fd);
int handle_read (int fd, void *buffer, unsigned size);
int handle_write (int fd, const void *buffer, unsigned size);
void handle_seek (int fd, unsigned position);
unsigned handle_tell (int fd);
void handle_close (int fd);

#endif /* userprog/syscall.h */
