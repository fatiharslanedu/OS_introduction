#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdint.h>

extern int end;
extern int etext;
int globVar;   /* should be an BSS Segment (uninitialized data) */ /* Declare, define available. */
int data = 10; /* initialize with 10, should be data locations.*/ /* Declare, define, initialize available.*/
extern void showStackGrowth(void);

int main(int argc, char **argv)
{
    char *s;
    char c;
    char *b, *nb;
    char *q;
    char cmd[100];
    /*Print the page size with linux command on console.*/
    sprintf(cmd, "echo -n  pageSize = ; echo -n ' ' ; getconf PAGESIZE"); 
    system(cmd);

    int localVar;
    char *p = (char *)malloc(sizeof(char)); /* memory allocating in heap segment */
    printf("&globVar\t = 0x%lx\n", &globVar);
    printf("&localVar\t = 0x%lx\n", &localVar);
    printf("&main()\t\t = 0x%lx\n", main); /* main is text locations */
    // showStackGrowth(); /* function address is in text location. But function is in stack location!*/
    // q = (char *)alloca(32);
    printf("text-segment end = 0x%lx\n", sbrk(0)); /*Shrink*/
    printf("data-seg end\t = 0x%lx\n", &data);
    printf("heap-seg end\t = 0x%lx\n", b);
    printf("\n\n");
    printf("Enter memory location in hex (start with 0x): ");
    fflush(stdout);
    scanf("0x%x", &s);
    printf("Reading 0x%x: ", s);
    fflush(stdout);
    c = *s;
    printf("%d\n", c);
    printf("Writing %d back to 0x%x: ", c, s);
    fflush(stdout);
    *s = c;
    printf("ok\n");
    return 0;
}

void showStackGrowth(void)
{   /*Stack level and stack var increace*/
    static int lvl = 0; 
    auto int sVar; /*Stack variable*/
    if (++lvl == 3)
    {
        return;
    }
    showStackGrowth(); /* Recursive Call */
}