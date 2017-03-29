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
#include "devices/input.h"

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
static int current_fd_num = 1;


void syscall_init (void);
void handle_halt (void);
void handle_exit (int status);
pid_t handle_exec (const char *cmd_line);
int handle_wait (pid_t pid);
bool handle_create (const char *file_name, unsigned initial_size);
bool handle_remove (const char *file_name);
int handle_open (const char *file_name);
int handle_filesize (int fd);
int handle_read (int fd_num, void *buffer, unsigned size);
int handle_write (int fd_num, const void *buffer, unsigned size);
void handle_seek (int fd_num, unsigned position);
unsigned handle_tell (int fd_num);
void handle_close (int fd_num);

struct file_descriptor * get_opened_file(int fd_num);

#endif /* userprog/syscall.h */
