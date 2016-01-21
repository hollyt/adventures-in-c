#ifndef JOB_CONTROL_H
#define JOB_CONTROL_H

#include <termios.h>
#include <unistd.h>

/*single process*/
typedef struct process
{
  struct process *next;
  char **argv;                /* for exec */
  pid_t pid;
  char completed;
  char stopped;
  int status;
} process;

/*pipeline of processes*/
typedef struct job
{
  struct job *next;
  char *command;              /* command line, used for messages */
  process *first_process;     /* list of processes in this job */
  pid_t pgid;
  char notified;              /* true if user told about stopped job */
  struct termios tmodes;      /* saved terminal modes */
  int stdin, stdout, stderr;
} job;

/*active jobs*/
extern job *first_job;
/*for process control*/
struct termios shell_tmodes;

#endif
