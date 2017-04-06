#include "userprog/syscall.h"

void syscall_handler (struct intr_frame *f);

// stores the last fd number allocated
static int current_fd_num = 1;

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  list_init(&all_open_files);
}

/* Gets the required argument for a process from the stack*/
uint32_t
load_stack(struct intr_frame *f, int offset)
{
  return *((uint32_t*)(f->esp + offset));
}

/* Handle the system call code */
void
syscall_handler (struct intr_frame *f)
{
  int sys_call_code;

  // determine the system call code
  sys_call_code = (int)load_stack(f, ARG_CODE);

  switch(sys_call_code){

    case SYS_HALT: /* Halt the operating system. */
    {
      handle_halt();
      return;
    }

    case SYS_EXIT: /* Terminate this process. */
    {
      handle_exit((int)load_stack(f, ARG_1));
      return;
    }

    case SYS_EXEC: /* Start another process. */
    {
      pid_t pid;

      pid = handle_exec((char*)load_stack(f, ARG_1));

      // set return value
      f->eax = pid;
       return;
     }

     // FIXME: for wait syscall
     // case SYS_WAIT: /* Wait for a child process to die. */
     // {
     //   int ret;
     //
     // 	ret = handle_wait((pid_t)load_stack(f, ARG_1));
     //
     // 	f->eax = ret;
     // 	return;
     // }

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

    case SYS_SEEK: /* Change position in a file. */
    {
      handle_seek(
        (int)load_stack(f,ARG_1),
        (unsigned)load_stack(f, ARG_2));

      return;
    }

    case SYS_TELL: /* Report current position in a file. */
    {
      unsigned next_byte;

      next_byte = handle_tell(
         (int)load_stack(f,ARG_1));

      // set return value
      f->eax = next_byte;
      return;
    }

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
}

void handle_halt (void){
  shutdown_power_off();
}

void handle_exit (int status){
  struct thread* cur;
  cur = thread_current();

  // remove thread from list
  struct list_elem element;
  element = cur->elem;
  list_remove(&element);
  cur->status = status;

  thread_exit();
}

pid_t handle_exec (const char *cmd_line){
  return process_execute(cmd_line);
}

// FIXME: for wait syscall
// int handle_wait (pid_t pid){
//   return process_wait((tid_t) pid);
// }

bool handle_create (const char *file_name, unsigned initial_size){
  return filesys_create(file_name, initial_size);
}

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

  // generate a unique fd number by incrementing the last fd number allocated
  fd->fd_num = ++current_fd_num;

  // add the file descriptor struct to the linked list of open files
  list_push_back(&all_open_files, &fd->elem);

  return fd->fd_num;
}

int handle_filesize (int fd_num){
  struct file_descriptor *fd;

  fd = calloc(1, sizeof *fd);
  fd = get_opened_file(fd_num);

  // return error code if there was a problem opening the file
  if (fd == NULL){
    return -1;
  }

  return file_length(fd->f);
}

int handle_read (int fd_num, void *buffer, unsigned size){
  struct file_descriptor *fd;
  uint8_t key_input;
  unsigned byte_pos;
  uint8_t* b = (uint8_t*)buffer;

  fd = calloc(1, sizeof *fd);

  if (fd_num == STDOUT_FILENO){
    return -1;
  }
  else if(fd_num == STDIN_FILENO){
    // get keyboard input
    for (byte_pos = 0; byte_pos < size; byte_pos++){
      key_input = input_getc();
      // add input character to the buffer
      *b = key_input;
      b++;
    }

    return byte_pos;
  }
  else{
    fd = get_opened_file(fd_num);
    // return error code if there was a problem opening the file
    if (fd == NULL){
      return -1;
    }
    return file_read(fd->f , buffer, size);
  }
}

int handle_write (int fd_num, const void *buffer, unsigned int length){
  struct file_descriptor *fd;
  int bw; // bytes written

  fd = calloc(1, sizeof *fd);

  if (fd_num == STDOUT_FILENO) {
    // write to console
    putbuf((const char *)buffer, (size_t)length);
    bw = length;
  }
  else if (fd_num == STDIN_FILENO){
    return -1;
  }
  else { // write to a file
    fd = get_opened_file(fd_num);

    // return error code if there was a problem opening the file
    if (fd == NULL){
      return - 1;
    }

    bw = file_write(fd->f, buffer, length);
  }

  return bw;
}

void handle_seek (int fd_num, unsigned position){
  struct file_descriptor *fd;

  fd = calloc(1, sizeof *fd);
  fd = get_opened_file(fd_num);

  // return if there was a problem opening the file
  if (fd == NULL){
    return;
  }

  file_seek (fd->f, position);
}

unsigned handle_tell (int fd_num){
  struct file_descriptor *fd;

  fd = calloc(1, sizeof *fd);
  fd = get_opened_file(fd_num);

  if (fd == NULL){
    return 0;
  }

  return file_tell (fd->f);
}

void handle_close (int fd_num){
  struct list_elem *element;
  struct file_descriptor *fd;

  fd = calloc(1, sizeof *fd);

  // find the open file in the list to remove and close it
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

  fd = calloc(1, sizeof *fd);

  // locate file in the list and return its file_descriptor struct
  for (element = list_begin(&all_open_files);
       element != list_end(&all_open_files);
       element = list_next(element))
       {
         fd = list_entry(element, struct file_descriptor, elem);
         if (fd->fd_num == fd_num){
           return fd;
         }
       }
  // unable to find the file
  return NULL;
}
