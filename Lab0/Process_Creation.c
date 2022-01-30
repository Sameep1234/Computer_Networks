/*
    Name - Sameep Vani
    Roll No. - AU1940049
    Question -
    Write a program that when executed creates two child processes using fork() system call where one child process
    prints date and the other displays the contents of the folder that contains the program. Further, all three
    processes print their PID and identify themselves as parent, child 1 and child 2 prior to printing anything else.
*/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

int main(void)
{
    int x, numberOfChild = 1;
    printf("Process Id of Parent is: %d\n", getpid());
    x = fork();
    if (x == -1)
    {
        perror("Fork Creation Failed");
        exit(EXIT_FAILURE);
    }
    else if (x == 0)
    {
        printf("Child 1 has a process id of %d and parent id is %d\n", getpid(), getppid());
        time_t curtime;
        time(&curtime);
        printf("Current Date is: %s\n", ctime(&curtime));
    }
    else if (x > 0)
    {
        x = fork();
        if (x == -1)
        {
            perror("Child 2 creation failed");
            exit(EXIT_FAILURE);
        }
        else if (x == 0)
        {
            printf("Child 2 has a process id of %d and parent id is %d\n", getpid(), getppid());
            system("ls -A");
        }
    }
}