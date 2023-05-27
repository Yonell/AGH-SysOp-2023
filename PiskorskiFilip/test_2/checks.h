#include <stdio.h>
#include <pthread.h>

int processed_value_count = 0;
int processed_value_sum = 0;

void process_value(int value)
{
    processed_value_sum += value;
    processed_value_count += 1;
}

void check_results()
{
    int expected = (20 / 2) * (0 + 19);

    if (processed_value_sum == expected)
    {
        printf("PASS: value processing\n");
    }
    else
    {
        printf("FAIL: value processing\n");
    }
}

void check_wait()
{
    if (processed_value_count == 20)
    {
        printf("PASS: thread awaiting\n");
    }
    else
    {
        printf("FAIL: thread awaiting\n");
    }
}

pthread_mutex_t *mutex1 = NULL;

void check_mutex_producer(pthread_mutex_t *mutex)
{
    if (pthread_mutex_lock(mutex) == 0)
    {
        if (mutex1 == NULL)
        {
            mutex1 = mutex;
            printf("PASS: mutex producer\n");
        }
        else
        {
            if (mutex1 == mutex)
            {
                printf("PASS: mutex producer\n");
            }
            else
            {
                printf("FAIL: mutex producer\n");
            }
        }
        pthread_mutex_unlock(mutex);
    }
    else
    {
        printf("FAIL: mutex producer\n");
    }
}

void check_mutex_consumer(pthread_mutex_t *mutex)
{
    if (pthread_mutex_lock(mutex) == 0)
    {
        if (mutex1 == NULL)
        {
            mutex1 = mutex;
            printf("PASS: mutex consumer\n");
        }
        else
        {
            if (mutex1 == mutex)
            {
                printf("PASS: mutex consumer\n");
            }
            else
            {
                printf("FAIL: mutex consumer\n");
            }
        }
        pthread_mutex_unlock(mutex);
    }
    else
    {
        printf("FAIL: mutex consumer\n");
    }
}

pthread_cond_t *cond1 = NULL;

void check_cond_producer(pthread_cond_t *cond)
{
    if (pthread_cond_broadcast(cond) == 0)
    {
        pthread_mutex_lock(mutex1);

        if (cond1 == NULL)
        {
            cond1 = cond;
            printf("PASS: cond producer\n");
        }
        else
        {
            if (cond1 != cond)
            {
                printf("PASS: cond producer\n");
            }
            else
            {
                printf("FAIL: cond producer\n");
            }
        }

        pthread_mutex_unlock(mutex1);
    }
    else
    {
        printf("FAIL: cond producer\n");
    }
}

void check_cond_consumer(pthread_cond_t *cond)
{
    if (pthread_cond_broadcast(cond) == 0)
    {
        pthread_mutex_lock(mutex1);

        if (cond1 == NULL)
        {
            cond1 = cond;
            printf("PASS: cond consumer\n");
        }
        else
        {
            if (cond1 != cond)
            {
                printf("PASS: cond consumer\n");
            }
            else
            {
                printf("FAIL: cond consumer\n");
            }
        }

        pthread_mutex_unlock(mutex1);
    }
    else
    {
        printf("FAIL: cond consumer\n");
    }
}