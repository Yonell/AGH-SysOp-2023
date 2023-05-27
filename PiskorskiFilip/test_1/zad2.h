#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>

void sig_pass() {
    printf("PASS: signal mask\n");
}

void check_mask()
{
    raise(SIGUSR2);

    struct sigaction sa;
    sa.sa_flags = 0;
    // sa.sa_mask = 0;
    sa.sa_handler = sig_pass;
    sigaction(SIGUSR1, &sa, NULL);

    raise(SIGUSR1);
}