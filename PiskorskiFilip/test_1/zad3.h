#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int read_pipe(int[2], size_t);

void check_pipe(int pd[2])
{
    write(pd[1], "a", 1);

    char a;
    if (read(pd[0], &a, 1) == 1)
    {
        printf("PASS: create pipe\n");
    }
    else
    {
        printf("FAIL: create pipe\n");
    }
}

void check_write(int pd[2], size_t block_size, void readwrite(int, size_t))
{
    fflush(stdout);
    int cpid = fork();
    if (cpid != 0)
    {
        close(pd[0]);
        readwrite(pd[1], block_size);
        close(pd[1]);

        int retval = 0;
        waitpid(cpid, &retval, 0);

        if (retval)
        {
            printf("PASS: writing to pipe\n");
        }
        else
        {
            printf("FAIL: writing to pipe\n");
        }
    }
    else
    {
        int ok = read_pipe(pd, block_size);
        exit(ok);
    }
}

int read_pipe(int pd[2], size_t block_size)
{
    close(pd[1]);

    char buffer[block_size];
    size_t bytes_total = 0;
    size_t bytes_read = 0;

    while ((bytes_read = read(pd[0], buffer, block_size)) > 0)
    {
        bytes_total += bytes_read;
    }

    FILE *fp = fopen("./unix.txt", "r");
    fseek(fp, 0L, SEEK_END);
    long sz = ftell(fp);
    fclose(fp);

    return (bytes_total == sz);
}