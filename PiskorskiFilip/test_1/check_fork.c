#include <unistd.h>
#include <signal.h>
#include <stdio.h>

void sig_pass() {
    printf("PASS: fork exec\n");
}


int main()
{
    struct sigaction sa;
    sa.sa_flags = 0;
    // sa.sa_mask = 0;
    sa.sa_handler = sig_pass;
    sigaction(SIGUSR1, &sa, NULL);

    sigset_t mask;
    sigemptyset (&mask);
    // sigaddset (&mask, SIGUSR1);
    sigsuspend(&mask);

    return 0;
}