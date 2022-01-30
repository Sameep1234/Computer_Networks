/*
    Name - Sameep Vani
    Roll No. - AU1940049
    Question -
    Write a program that when executed creates two child processes using fork() system call where one child process
    prints date and the other displays the contents of the folder that contains the program. Further, all three
    processes print their PID and identify themselves as parent, child 1 and child 2 prior to printing anything else.
*/

/* Require header files */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(void)
{
    /* x = Variable for process id that fork() returns */
    int x;

    /* Process id of parent */
    printf("Process Id of Parent is: %d\n", getpid());
    x = fork();

    /* Error */
    if (x == -1)
    {
        perror("Fork Creation Failed");
        exit(EXIT_FAILURE);
    }
    /* First Child Process */
    else if (x == 0)
    {
        /* Child Process 1's ID and printing current date. */
        printf("Child 1 has a process id of %d and parent id is %d\n", getpid(), getppid());
        time_t curtime;
        time(&curtime);
        printf("Current Date is: %s\n", ctime(&curtime));
    }
    else if (x > 0)
    {
        /* Inside parent and perform new fork() to create second child. */
        x = fork();
        /* Error while creating the second file. */
        if (x == -1)
        {
            perror("Child 2 creation failed");
            exit(EXIT_FAILURE);
        }
        /* Second Child Process */
        else if (x == 0)
        {
            /* Print second process's id and use system call to print files in the current folder. */
            printf("Child 2 has a process id of %d and parent id is %d\n", getpid(), getppid());
            system("ls -A");
        }
    }
}