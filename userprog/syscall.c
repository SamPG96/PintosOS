#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

void syscall_handler (struct intr_frame *f);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static uint32_t load_stack(struct intr_frame *f, int offset)
{
  //TODO: check for valid address

  return *((uint32_t*)(f->esp + offset));
}

void
syscall_handler (struct intr_frame *f)
{
  int sys_call_code;

  sys_call_code = (int)load_stack(f, ARG_CODE);

  switch(sys_call_code){
    // case(SYS_HALT): /* Halt the operating system. */
    // case(SYS_EXIT): /* Terminate this process. */
    // case(SYS_EXEC): /* Start another process. */
    // case(SYS_WAIT): /* Wait for a child process to die. */
    // case(SYS_CREATE): /* Create a file. */
    // case(SYS_REMOVE): /* Delete a file. */
    // case(SYS_OPEN): /* Open a file. */
    // case(SYS_FILESIZE): /* Obtain a file's size. */
    // case(SYS_READ): /* Read from a file. */
    case SYS_WRITE:
    { /* Write to a file. */
      int result = handle_write(
         (int)load_stack(f,ARG_1),
         (void *)load_stack(f, ARG_2),
         (unsigned int)load_stack(f, ARG_3));
      // set return value
      f->eax = result;
      break;
    }
    // case(SYS_SEEK): /* Change position in a file. */
    // case(SYS_TELL): /* Report current position in a file. */
    // case(SYS_CLOSE): /* Close a file. */
    default:
    {
      printf("SYS_CALL (%d) is not implemented\n", sys_call_code);
      thread_exit ();
    }
  }
  thread_exit ();
}

// void handle_halt (void){}
//
// void handle_exit (int status){}
//
// pid_t handle_exec (const char *cmd_line){}
//
// int handle_wait (pid_t pid){}
//
// bool handle_create (const char *file, unsigned initial_size){}
//
// bool handle_remove (const char *file){}
//
// int handle_open (const char *file){}
//
// int handle_filesize (int fd){}
//
// int handle_read (int fd, void *buffer, unsigned size){}
//
int handle_write (int fd, const void *buffer, unsigned int length){
  printf("BUFFER: %s, LENGTH: %u\n", buffer, length);
  if (fd == STDOUT_FILENO) {
    putbuf((const char *)buffer, (size_t)length);
  }
  else {
    printf("handle_write does not support fd output\n");
  }
  return length;
}
//
// void handle_seek (int fd, unsigned position){}
//
// unsigned handle_tell (int fd){}
//
// void handle_close (int fd);{}
