#include <stdio.h>
#include <pthread.h>

#define NUM_THREADS 5

typedef struct {
    int count;
    int waiting;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} Barrier;

void barrier_init(Barrier *barrier, int count) {
    // Atribui o número total de threads que devem atingir a barreira
    barrier->count = count;
    
    // Inicializa a variável de espera com zero
    barrier->waiting = 0;
    
    // Inicializa o mutex para garantir exclusão mútua
    pthread_mutex_init(&barrier->mutex, NULL);
    
    // Inicializa a variável de condição para sincronização
    pthread_cond_init(&barrier->cond, NULL);
}

void barrier_wait(Barrier *barrier) {
    pthread_mutex_lock(&barrier->mutex); // Bloqueia o mutex para garantir exclusão mútua

    barrier->waiting++; // Incrementa o contador de threads esperando

    // Se todas as threads atingiram a barreira
    if (barrier->waiting >= barrier->count) {
        barrier->waiting = 0; // Reinicia o contador de threads esperando
        pthread_cond_broadcast(&barrier->cond); // Libera todas as threads na barreira
    } else {
        // Aguarda até que todas as threads atinjam a barreira
        pthread_cond_wait(&barrier->cond, &barrier->mutex);
    }

    pthread_mutex_unlock(&barrier->mutex); // Libera o mutex
}

void barrier_destroy(Barrier *barrier) {
    pthread_mutex_destroy(&barrier->mutex); // Destroi o mutex
    pthread_cond_destroy(&barrier->cond); // Destroi a variável de condição
}


Barrier my_barrier;

void *worker(void *arg) {
    int thread_id = *(int *)arg;
    printf("Thread %d: Iniciando...\n", thread_id);
    
    // Simulando algum trabalho
    for (int i = 0; i < 3; ++i) {
        printf("Thread %d: Realizando trabalho iterativo %d...\n", thread_id, i);
    }

    // Barreira - todas as threads esperam até que todas tenham atingido este ponto
    barrier_wait(&my_barrier);
    
    printf("Thread %d: Continuando após a barreira...\n", thread_id);
    // Outras operações após a barreira

    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Inicializando a barreira com o número de threads
    barrier_init(&my_barrier, NUM_THREADS);

    // Criando threads
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, worker, &thread_ids[i]);
    }

    // Esperando que as threads terminem
    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    // Destruindo a barreira
    barrier_destroy(&my_barrier);

    return 0;
}
