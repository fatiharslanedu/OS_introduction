/*
    2020 - 2021 Spring
    @author: Fatih Arslan
    @Lesson: Operating System Lab.
    @Lab Application 4
*/

// Define the necessary library.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <linux/limits.h>
#include <unistd.h>
/* 
    This function created for thread. 
    And return date format.
*/

void *phoneFunc(); 

int after3Digits(long int number)
{ /* This math operation give the after 3 digits. */
    while (number > 100000)
    {
        number /= 10;
    }
    return number;
}

int getFirst2Digits(long int number)
{ /* This math operation give the first 2 digits. */
    while (number > 100)
    {
        number /= 10;
    }
    return number;
}
int last7Digits(long int number)
{ /* This math operation give the last 7 digits. */
    while (number > 10000000)
    {
        number %= 10000000;
    }
    return number;
}

int reverse(int number)
{ /* This function get reverse of number. */
    int revNumber = 0, remainder;
    while (number != 0)
    {
        remainder = number % 10;
        revNumber = revNumber * 10 + remainder;
        number /= 10;
    }
    return revNumber;
}

int aFlag = 0; /* Define this flag 0, and use for alarmHandling. 0 mean False. */
void alarmHandling()
{ /*This function use signals and alarm function.
    Use for incorrect situtation. */
    printf("Alarm Handler-> Alarm clock signal has been received\n");
    aFlag = 1;
}

void sigHandler(int sig_num)
{  /*
    If the last 7 digits of number smaller than reverse of last 7 digits,
    and user press CTRL+Z, CTRL+C the program not be terminated. 
    That mean not gonna stop.
    */
    signal(SIGTSTP, sigHandler);
    signal(SIGINT, sigHandler);
    printf("Signal Handler -> Protection from Ctrl+C | Ctrl+Z.\n");
    fflush(stdout);
}

int main(int argc, char *argv[]) /*argc = argumant counter and argv = argument vector*/
{ /* argv[1] ==> input file(phone.txt) */

    if(argc == 2)
    {
    long int phoneNumber;
    pthread_t t1; /* Thread definition */
    signal(SIGALRM, alarmHandling); /* Signal definition take signal type and function. */
    int status = 0;
    int fd[2]; /* File descriptor use for pipe. fd[0] for read, fd[1] for write. */
    if (pipe(fd) == -1)
    { /*Fail process, -1 mean False*/
        printf("pipe was failed\n");
        exit(2);
    }
    printf("Pipe was created successfully...\n");
    pid_t child_pid = fork();
    if (child_pid == -1)
    { /*Fail process, -1 mean False*/
        perror("fork failed");
        exit(3);
    }
    if (child_pid == 0)
    {
        // Child process
        close(fd[0]);
        printf("Child        -> My PID is %d, PPID is %d\n", getpid(), getppid());

        char mychar2[4096];
        realpath(argv[1], mychar2); /* Take the file path */
        printf("             -> %s\n", mychar2);
        FILE *rFile = fopen(argv[1], "r"); /*Only read the file. Take from argv[1]*/
        if (rFile == NULL)
        {   /*Fail process, NULL mean False*/
            printf("File was not openned.\n");
            exit(4);
        }

        while (fscanf(rFile, "%ld", &phoneNumber) == 1)
        {   /*Read the phone number with long int size. */
            //printf("Phone number: %ld\n", phoneNumber);
        }
        
        if (pthread_create(&t1, NULL, &phoneFunc, argv[1]) != 0)
        { /* Create a thread */ 
            printf("thread failed\n");
            exit(1);
        }
        if (pthread_join(t1, NULL) != 0)
        { /* Execute the thread */
            exit(5);
        }
        printf("Child        -> Parsing and printing Phone Number (Hardcodded, just for info)\n");
        /* Math operations */
        int first2d = getFirst2Digits(phoneNumber);
        int after3d = after3Digits(phoneNumber);
        int last7d = last7Digits(phoneNumber);
        int reverse7d = reverse(last7d);

        if (last7d < reverse7d || first2d != 90 || after3d != 90222)
        { /*Check the wrong situtation*/
            /*Write from the child process, fd[1] = write*/
            write(fd[1], &phoneNumber, sizeof(phoneNumber));
            printf("Child        -> The message is being send. Warning: Type Constraint!\n");
        }
        else
        { /*If everything is correct*/
                /*Write from the child process, fd[1] = write*/
            write(fd[1], &phoneNumber, sizeof(phoneNumber));
            printf("Child        -> The message is being send.\n");
        }
        printf("Child        -> Sleeping 4 secs\n");
        close(fd[1]); /*Close the write part of pipe.*/
    }
    else
    { // Parent process
        int returnStatus;

        close(fd[1]); /*Close the write part of pipe.*/
        printf("Parent       -> My PID is %d, PPID is %d\n", getpid(), getppid());
        long int getNumber;
        /*Read from the parent process*/
        read(fd[0], &getNumber, sizeof(getNumber));
        sleep(4);
        printf("Parent       -> Incomming message from child is: %ld\n", getNumber);
        printf("             -> Hint: Incoming message size is fixed 8 bytes for this application. Not 12 (digit count) or something other!\n");
        /*Math operations*/
        int first2d = getFirst2Digits(getNumber);
        int after3d = after3Digits(getNumber);
        int last7d = last7Digits(getNumber);
        int reverse7d = reverse(last7d);

        if (first2d == 90)
        {
            if (after3d == 90222)
            {
                wait(NULL);
            }
            else
            {
                printf("Parent       -> Alarm clock signal was scheduled in 4 seconds...\n");
                alarm(4);
                /*SIGTSTP mean CTRL+Z for break*/
                signal(SIGTSTP, sigHandler);
                /*SIGINT mean CTRL+C for break*/
                signal(SIGINT, sigHandler);
                while (!aFlag)
                {
                    pause();
                }
            }
        }
        else
        {
            printf("             -> Please enter the area code with 90. Check the file.\n");
            printf("Parent       -> Alarm clock signal was scheduled in 6 seconds...\n");
            alarm(6);
            signal(SIGTSTP, sigHandler);
            signal(SIGINT, sigHandler);
            while (!aFlag)
            {
                pause();
            }

            aFlag = 0;
        }
        if (last7d < reverse7d)
        {
            printf("Warning: Last 7 digits smaller than reverse.\n");
            printf("Parent        -> Alarm clock signal was scheduled in 6 seconds...\n");
            alarm(4);
            while (!aFlag)
            {
                pause();
            }
        }

        close(fd[0]);
        wait(NULL);

        printf("Parent       -> Child process is being waited.\n");
        printf("Child        -> Sleept 4 secs\n");
        sleep(4);
        aFlag = 0;
        printf("Terminated PID: %d, PPID: %d\n", child_pid, getppid());
        WTERMSIG(status);
        waitpid(child_pid, &returnStatus, 0); /*if we want to wait for.*/

        printf("Parent       -> Waiting is done.\n");
        printf("Terminated PID: %d, PPID: %d\n", getpid(), getppid());
    }
    int returnStatus;
    return 0;
    }
    else
    {
        printf("Enter only 1 argument like [phone.txt]\n");
    }
}

struct stat attr;
void *phoneFunc(void *arg)
{   
    /*stat is status of given file*/
    if(!stat(arg, &attr))
    {
        printf("Change your file name with phone.txt or you can edit on code.\n");
    } 
    printf("             -> Last modified time: %s", ctime(&attr.st_mtime));
    printf("             -> Last updated time %s", ctime(&attr.st_ctime));
}
