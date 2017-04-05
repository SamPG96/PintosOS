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
#include "devices/input.h"

typedef int pid_t;

#define ARG_CODE 0
#define ARG_1 4
#define ARG_2 8
#define ARG_3 12

// stores the relevant attributes for an open file
struct file_descriptor{
  struct list_elem elem; // for use in a linked list
  int fd_num;
  struct file *f;
};

// stores all files that have been opened
struct list all_open_files;

/*
  Function: syscall_init
  Description: Sets interrupt register on.
  Params: none (void)
  Returns: none (void)
*/
void syscall_init (void);

/*
  Function: load_stack
  Description: Gets the required argument for a process from the stack.
  Params:
    f: The frame to which we can read the stack.
    offset: The point of the stack we want to read.
  Returns: The argument required from the stack
*/
uint32_t load_stack(struct intr_frame *f, int offset);

/*
  Function:handle_halt
  Description: Shuts down Pintos.
  Params:none (void)
  Returns:none (void)
*/
void handle_halt (void);

/*
  Function:handle_exit
  Description: Exits current thread and pinds it's status.
  Params:
    status: Used to set current threads status.
  Returns:none (void)
*/
void handle_exit (int status);

/*
  Function:handle_exec
  Description: Runs the executable given in the comand line
  Params:
    cmd_line: The string of chars from the comand line
  Returns: process id of executing thread.
*/
pid_t handle_exec (const char *cmd_line);

/*
  Function:handle_wait
  Description: Waits for a child and retrieves the child's exit status.
  Params:
    pid: Process id of child to be waited on
  Returns: Childs exit status
*/
int handle_wait (pid_t pid);

/*
  Function: handle_create
  Description: Creates a new file of a specified size in bytes.
  Params:
    file_name: The name of the file to create.
    initial_size: The initial size of the new file.
  Returns: True if the file was created successful, False otherwise.
*/
bool handle_create (const char *file_name, unsigned initial_size);

/*
  Function: handle_remove
  Description: Delete a file from the filesystem. A file can be removed whether
               or not the file is open.
  Params:
    file_name: The name of the file to remove.
  Returns: True if removal was successful or False otherwise.
*/
bool handle_remove (const char *file_name);

/*
  Function: handle_open
  Description: Open specified file.
  Params:
    file_name: The name of the file to open.
  Returns: File descriptor number or -1 if the file could not be opened.
*/
int handle_open (const char *file_name);

/*
  Function: handle_filesize
  Description: Determine the size of a file in bytes.
  Params:
    fd_num: The file descriptor number of the open file.
  Returns: The size of the file in bytes.
*/
int handle_filesize (int fd_num);

/*
  Function: handle_read
  Description: Read 'size' amount of bytes from the open file and store it
               inside the buffer. When fd_num is 0, the keyboard is read.
  Params:
    fd_num: The file descriptor number of the open file.
    buffer: A pointer to where the read bytes should be stored to.
    size: The amount of bytes to read.
  Returns: The number of bytes actually read (0 at end of the file) or -1 if the
           file cant be read (because of a condition other than end of file).
*/
int handle_read (int fd_num, void *buffer, unsigned size);

/*
  Function: handle_write
  Description: Write 'size' amount of bytes from the buffer to the open file.
               When fd_num is 1, then the bytes are written to the console.
  Params:
    fd_num: The file descriptor number of the open file.
    buffer: A pointer to where the bytes to be written are stored.
    size: The amount of bytes to write.
  Returns: The number of bytes actually written (0 if no bytes could be written).
*/
int handle_write (int fd_num, const void *buffer, unsigned size);

/*
  Function: handle_seek
  Description: Changes the next byte to be read or written in an open file.
  Params:
    fd_num: The file descriptor number of the open file.
    position: The byte to move to (position 0 is the start of the file).
  Returns: void
*/
void handle_seek (int fd_num, unsigned position);

/*
  Function: handle_tell
  Description: Determine the position of the next byte to be read or written
               from the begining of an open file. Expressed in bytes.
  Params:
    fd_num: The file descriptor number of the open file.
  Returns: The byte position.
*/
unsigned handle_tell (int fd_num);

/*
  Function: handle_close
  Description: Closes a open file
  Params:
    fd_num: The file descriptor number of the open file.
  Returns: void
*/
void handle_close (int fd_num);

/*
  Function: get_opened_file
  Description: Retrieves the file_descriptor struct for an open file by using a
               file descriptor number. This struct can be used to access the
               data inside a open file.
  Params:
    fd_num: The file descriptor number of the open file.
  Returns: The file_descriptor struct of the open file.
*/
struct file_descriptor * get_opened_file(int fd_num);

#endif /* userprog/syscall.h */
