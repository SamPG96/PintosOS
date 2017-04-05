#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/malloc.h"
#include "devices/shutdown.h"
#include "process.h"

typedef int pid_t;

#define ARG_CODE 0
#define ARG_1 4
#define ARG_2 8
#define ARG_3 12

struct file_descriptor{
  struct list_elem elem;
  int fd_num;
  struct file *f;
};

struct list all_open_files;

/*
  Function: syscall_init
  Description:
  Params:none (void)
  Returns:none (void)
*/
void syscall_init (void);

/*
  Function:handle_halt
  Description:
  Params: none (void)
  Returns:none (void)
*/
void handle_halt (void);

/*
  Function:handle_exit
  Description:
  Params:none (void)
  Returns:none (void)
*/
void handle_exit (int status);

/*
  Function:handle_exec
  Description:
  Params:
  Returns:
*/
pid_t handle_exec (const char *cmd_line);

/*
  Function:handle_wait
  Description:
  Params:
  Returns:
*/
int handle_wait (pid_t pid);

/*
  Function:handle_create
  Description:
  Params:
  Returns:
*/
bool handle_create (const char *file_name, unsigned initial_size);

/*
  Function:handle_remove
  Description:
  Params:
  Returns:
*/
bool handle_remove (const char *file_name);

/*
  Function:handle_open
  Description:
  Params:
  Returns:
*/
int handle_open (const char *file_name);

/*
  Function:handle_filesize
  Description:
  Params:
  Returns:
*/
int handle_filesize (int fd);

/*
  Function:handle_read
  Description:
  Params:
  Returns:
*/
int handle_read (int fd_num, void *buffer, unsigned size);

/*
  Function:handle_write
  Description:
  Params:
  Returns:
*/
int handle_write (int fd, const void *buffer, unsigned size);

/*
  Function:handle_seek
  Description:
  Params:
  Returns:
*/
void handle_seek (int fd, unsigned position);

/*
  Function:handle_tell
  Description:
  Params:
  Returns:
*/
unsigned handle_tell (int fd);

/*
  Function:handle_close
  Description:
  Params:
  Returns:none (void)
*/
void handle_close (int fd_num);

/*
  Function:get_opened_file
  Description:
  Params:
  Returns:
*/
struct file_descriptor * get_opened_file(int fd_num);

#endif /* userprog/syscall.h */
