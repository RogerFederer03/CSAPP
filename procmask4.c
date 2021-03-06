/* Using sigprocmask to synchronize processes. In this example, the parent
   ensures that addjob executes before the corresponding deletejob. */

#include "csapp.h"

pid_t rFork(void);

void initjobs()
{
    printf("initjobs\n");
}

void addjob(int pid)
{
    printf("addjob\n");
}

void deletejob(int pid)
{
    printf("deletejob\n");
}

/* $begin procmask2 */
void handler(int sig)
{
    pid_t pid;
    while ((pid = waitpid(-1, NULL, 0)) > 0) /* Reap a zombie child */
    deletejob(pid); /* Delete the child from the job list */
    if (errno != ECHILD)
    unix_error("waitpid error");
}
    
int main(int argc, char **argv)
{
    int pid;
    sigset_t mask;

    Signal(SIGCHLD, handler);
    initjobs(); /* Initialize the job list */

    while (1) {
        Sigemptyset(&mask);
        Sigaddset(&mask, SIGCHLD); 
        Sigprocmask(SIG_BLOCK, &mask, NULL); /* Block SIGCHLD */
        
        /* Child process */
        if ((pid = rFork()) == 0) {
            Sigprocmask(SIG_UNBLOCK, &mask, NULL); /* Unblock SIGCHLD */
            Execve("/bin/date", argv, NULL);
        }
        /* Parent process */
        addjob(pid);  /* Add the child to the job list */
        Sigprocmask(SIG_UNBLOCK, &mask, NULL);  /* Unblock SIGCHLD */
        sleep (1);
    }
    exit(0);
}
/* $end procmask2 */
