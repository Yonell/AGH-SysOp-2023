/**
 * W tym programie mamy dwa wątki: producenta i konsumenta. Wątek producenta produkuje 
 * wartości i przechowuje je w współdzielonym buforze, podczas gdy wątek konsumenta 
 * pobiera wartości z bufora.
 * 
 * Używamy muteksu i dwóch zmiennych warunkowych do synchronizacji dostępu do bufora. 
 * Muteks zapewnia, że tylko jeden wątek może jednocześnie uzyskać dostęp do bufora, 
 * podczas gdy zmienne warunkowe pozwalają wątkom sygnalizować sobie nawzajem, gdy bufor 
 * jest pełny lub pusty. 
 * 
 * Wątek producenta zajmuje muteks, sprawdza za pomocą zmiennej count, czy bufor jest 
 * pełny, i jeśli tak, oczekuje na zmiennej warunkowej producer_cond. Gdy w buforze jest 
 * miejsce, produkuje wartość, przechowuje ją w buforze i sygnalizuje zmiennej warunkowej, 
 * aby powiadomić wątek konsumenta. Na koniec zwalnia muteks.
 * 
 * Wątek konsumenta zajmuje muteks, sprawdza za pomocą zmiennej count, czy bufor jest 
 * pusty, i jeśli tak, oczekuje na zmiennej warunkowej consumer_cond. Gdy wartość jest 
 * dostępna w buforze, pobiera ją, przetwarza wartość i sygnalizuje zmiennej warunkowej 
 * producer_cond, aby powiadomić wątek producenta. Na koniec zwalnia muteks.
 * 
 * Funkcja main tworzy wątki producenta i konsumenta, a następnie oczekuje na ich 
 * zakończenie.
*/
#include "checks.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define BUFFER_SIZE 10
#define VALUE_COUNT 20

int buffer[BUFFER_SIZE];
int count = 0;
pthread_mutex_t mutex2;
pthread_cond_t buffer_not_full;
pthread_cond_t buffer_not_empty;


void *producer(void *arg)
{
    /* Podaj stworzony przez siebie, odpowiedni mutekx do funkcji check_mutex_producer */
    check_mutex_producer(&mutex2);
    /* Podaj stworzoną przez siebie, odpowiednią zmienną warunkową do funkcji check_cond_producer */
    check_cond_producer(&buffer_not_full);

    int i;
    for (i = 0; i < VALUE_COUNT; ++i)
    {
        /* Zajmij mutex */
        pthread_mutex_lock(&mutex2);

        /* Tu czekaj tak długo jak: count == BUFFER_SIZE */
        while(count == BUFFER_SIZE){
            pthread_cond_wait(&buffer_not_full, &mutex2);
        }

        buffer[count++] = i;

        /* Zasygnalizuj, że bufor jest pełny */
        pthread_cond_signal(&buffer_not_empty);

        /* Zwolnij mutex */
        pthread_mutex_unlock(&mutex2);
    }

    return NULL;
}

void *consumer(void *arg)
{
    /* Podaj stworzony przez siebie, odpowiedni mutekx do funkcji check_mutex_consumer */
    check_mutex_consumer(&mutex2);
    /* Podaj stworzoną przez siebie, odpowiednią zmienną warunkową do funkcji check_cond_consumer */
    check_cond_consumer(&buffer_not_empty);

    int i;
    for (i = 0; i < VALUE_COUNT; ++i)
    {
        /* Zajmij mutex */
        pthread_mutex_lock(&mutex2);

        /* Tu czekaj tak długo jak: count == 0 */
        while(count == 0){
            pthread_cond_wait(&buffer_not_empty, &mutex2);
        }

        int value = buffer[--count];
        process_value(value);

        /* Zasygnalizuj, że bufor jest pusty */
        pthread_cond_signal(&buffer_not_full);

        /* Zwolnij mutex */
        pthread_mutex_unlock(&mutex2);
    }

    return NULL;
}

int main()
{
    pthread_mutex_init(&mutex2, NULL);
    pthread_cond_init(&buffer_not_full, NULL);
    pthread_cond_init(&buffer_not_empty, NULL);
    /* Stwórz wątek producenta, podaj mu funkcję producer() do wykonywania */
    pthread_t producer_thr;
    pthread_create(&producer_thr, NULL, producer, NULL);

    /* Stwórz wątek konsumenta, podaj mu funkcję consumer() do wykonywania */
    pthread_t consumer_thr;
    pthread_create(&consumer_thr, NULL, consumer, NULL);


    /* Poczekaj na zakończenie działania wątków */
    pthread_join(producer_thr, NULL);
    pthread_join(consumer_thr, NULL);


    pthread_mutex_destroy(&mutex2);
    pthread_cond_destroy(&buffer_not_full);
    pthread_cond_destroy(&buffer_not_empty);


    check_wait();
    check_results();

    return 0;
}