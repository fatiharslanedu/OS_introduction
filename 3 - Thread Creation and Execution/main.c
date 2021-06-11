#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <errno.h>
#include <sys/wait.h>
#define BUFFERSIZE 2500000
#define True 1

char *pathServer;
struct stat attribute;

void *funcprintTimeAndChanges(char *pathServer)
{
    char pathLFile[650];
    sprintf(pathLFile, "Version %s.0\n", pathServer);
    printf("------------------------------------------------\n");
    /* Print to time to txt file. */
    printf("Last modified time: %s", ctime(&attribute.st_mtime));
    printf("\r\033[1A%c", system("date"));
    fflush(stdout); /* Clear the output buffer */
    usleep((int)(100000));

    printf("----------------------END----------------------\n");
}

void *funcChecker(void *pathClient)
{
    stat(pathClient, &attribute);
}
int copy_file(const char *fPath, const char *tPath)
{
    struct stat fInfo, tInfo;
    char list[BUFFERSIZE];
    ssize_t count;
    int source;
    int destination;
    int cause;

    if (!fPath || !*fPath ||
        !*tPath || !*tPath)
    {
        fprintf(stderr, "function empty or null...\n");
        return errno = EINVAL;
    }

    source = open(fPath, O_RDONLY | O_NOCTTY);
    if (source == -1)
    {
        cause = errno;
        return errno = cause;
    }
    if (fstat(source, &fInfo) == -1)
    {
        cause = errno;
        return errno = cause;
    }

    destination = open(tPath, O_WRONLY | O_CREAT | O_EXCL, fInfo.st_mode & 0777);
    if (destination == -1)
    {
        cause = errno;
        errno = cause;
    }

    while (True)
    {
        char *pointer1, *pointer2;

        count = read(source, list, sizeof list);
        if (count == 0)
        {
            /* End of input. */
            cause = 0;
            break;
        }
        else if (count < 0)
        {
            /* Interrupt if there any error. */
            if (count == -1)
                cause = errno;
            else
                cause = EIO;
            break;
        }

        pointer1 = list;
        pointer2 = (char *)malloc(count);
        cause = 0;
        while (pointer1 < pointer2)
        {
            count = write(destination, pointer1, (size_t)(pointer2 - pointer1));
            if (count > 0)
                pointer1 += count;
            else if (count == -1)
            {
                cause = errno;
                break;
            }
            else
            {
                /* Bug */
                cause = EIO;
                break;
            }
        }
        if (cause)
        {
            break;
        }
    }

    if (cause)
    {
        unlink(tPath);
        return errno = cause;
    }
    if (fstat(destination, &tInfo) == -1)
    {
        cause = errno;
        unlink(tPath);
        return errno = cause;
    }

    /* Size gonna be zero... */
    if (fInfo.st_size > 0 &&
        fInfo.st_size != tInfo.st_size)
    {
        cause = EIO;
        unlink(tPath);
        return errno = cause;
    }

    /* Closing... */
    if (close(source) == -1)
    {
        cause = errno;
        unlink(tPath);
        return errno = cause;
    }

    if (close(destination) == -1)
    {
        cause = errno;
        unlink(tPath);
        return errno = cause;
    }

    /* Return Success. */
    return errno = 0;
}
void *funcStarter(void *pathClient)
{
    printf("Here is WorkerThread!\n");
    printf("\r\033[1A%c", system("date"));
    /* If file is not blank copy */
    copy_file(pathServer, pathClient);
    funcprintTimeAndChanges(pathServer);
}

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Terminal parameters at least be 2...\nUsage: ./main [pathServer] [pathClient]\n");
        exit(-1);
    }
    pathServer = argv[1];       /* Assignment of first path from input file */
    char *pathClient = argv[2]; /* Assignment of second path from input file */

    pthread_t threadWorker_TimeAndChanges;
    pthread_t threadChecker;
    int result1, result2;

    /*
        Shared paramater is same between pathClient and pathServer.
    */
    result1 = pthread_create(&threadChecker, NULL, &funcChecker, argv[2]);
    if (result1 < 0)
    {
        printf("Thread_Checker Creation Failed\n");
        return 1;
    }

    result2 = pthread_create(&threadWorker_TimeAndChanges, NULL, &funcStarter, argv[2]);
    /* Control the state for threadWorker_TimeAndChanges */
    if (result2 < 0)
    {
        printf("Thread_Worker Creation Failed\n");
        return 1;
    }
    /* 
        Parent waits for Checker in first one,
        Parent waits for Worker in second one.
    */
    pthread_join(threadChecker, NULL);
    pthread_join(threadWorker_TimeAndChanges, NULL);
    /* Check the error for thread. */
    pthread_exit(0);
    pthread_exit(0);

    return 0;
}
