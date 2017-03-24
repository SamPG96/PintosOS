#include "userprog/syscall.h"

void syscall_handler (struct intr_frame *f);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  list_init(&all_open_files);
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

    // TODO: handle if already exists
    case SYS_CREATE: /* Create a file. */
    {
      bool success;

      success = handle_create(
        (char*)load_stack(f, ARG_1),
        load_stack(f, ARG_2));

      // set return value
      f->eax = success;
      return;
    }

    case SYS_REMOVE: /* Delete a file. */
    {
      bool success;

      success = handle_remove(
        (char*)load_stack(f, ARG_1));

      // set return value
      f->eax = success;
      return;
    }

    case SYS_OPEN: /* Open a file. */
    {
      int result;

      result = handle_open(
        (char*)load_stack(f, ARG_1));

      // set return value
      f->eax = result;
      return;
    }

    case SYS_FILESIZE: /* Obtain a file's size. */
    {
      int size;

      size = handle_filesize(
        (int)load_stack(f, ARG_1));

      f->eax = size;
      return;
    }

    // TODO: handle if file num == 1
    case SYS_READ: /* Read from a file. */
    {
      int bytes_read;

      bytes_read = handle_read(
        (int)load_stack(f,ARG_1),
        (void *)load_stack(f, ARG_2),
        (unsigned int)load_stack(f, ARG_3));

      // set return value
      f->eax = bytes_read;
      return;
    }

    case SYS_WRITE: /* Write to a file. */
    {
      int result;

      result = handle_write(
         (int)load_stack(f,ARG_1),
         (void *)load_stack(f, ARG_2),
         (unsigned int)load_stack(f, ARG_3));

      // set return value
      f->eax = result;
      return;
    }
    // case(SYS_SEEK): /* Change position in a file. */
    // case(SYS_TELL): /* Report current position in a file. */

    case SYS_CLOSE: /* Close a file. */
      handle_close(
        (int)load_stack(f,ARG_1));

      return;

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
bool handle_create (const char *file_name, unsigned initial_size){
  return filesys_create(file_name, initial_size);
}


/* Use filesys to delete the file */
bool handle_remove (const char *file_name){
  return filesys_remove(file_name);
}

int handle_open(const char *file_name){
  struct file_descriptor *fd;

  fd = calloc(1, sizeof *fd);
  fd->f = filesys_open(file_name);

  // return error code if there was a problem opening the file
  if (fd->f == NULL){
    return -1;
  }

  fd->fd_num = ++current_fd_num;

  list_push_back(&all_open_files, &fd->elem);

  return fd->fd_num;
}

int handle_filesize (int fd_num){
  struct file_descriptor *fd;

  fd = get_opened_file(fd_num);

  if (fd != NULL){
    return file_length(fd->f);
  }
  else{
    return -1;
  }
}

int handle_read (int fd_num, void *buffer, unsigned size){
  struct file_descriptor *fd;

  fd = get_opened_file(fd_num);
  if (fd == NULL){
    return -1;
  }

  return file_read(fd->f , buffer, size);
}


int handle_write (int fd, const void *buffer, unsigned int length){
  //printf("BUFFER: %s, LENGTH: %u\n", buffer, length);
  if (fd == STDOUT_FILENO) {
    putbuf((const char *)buffer, (size_t)length);
  }
  else {
    printf("handle_write does not support fd output\n");
  }
  return length;
}


// void handle_seek (int fd, unsigned position){}

// unsigned handle_tell (int fd){}

void handle_close (int fd_num){
  struct list_elem *element;
  struct file_descriptor *fd;

  for (element = list_begin(&all_open_files);
       element != list_end(&all_open_files);
       element = list_next(element))
       {
         fd = list_entry(element, struct file_descriptor, elem);
         if (fd->fd_num == fd_num){
           list_remove(element);
           file_close(fd->f);
           return;
         }
       }
   return;
}

struct file_descriptor * get_opened_file(int fd_num){
  struct list_elem *element;
  struct file_descriptor *fd;

  for (element = list_begin(&all_open_files);
       element != list_end(&all_open_files);
       element = list_next(element))
       {
         fd = list_entry(element, struct file_descriptor, elem);
         if (fd->fd_num == fd_num){
           return fd;
         }
       }
  return NULL;
}
