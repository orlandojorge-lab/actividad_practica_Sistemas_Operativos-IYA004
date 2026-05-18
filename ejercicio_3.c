#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex;
sem_t empty;
sem_t full;

int in = 0;  // posicion en la que el productor escribe
int out = 0; // posicion en la que el consumidor lee

typedef struct {
  int *buffer;
  int id;
} DatosProductor;


void *productor(void *args) {

  DatosProductor *datos = (DatosProductor *)args;
  int *buffer = datos->buffer;
  int id = datos->id;

  for (int i = 0; i < 10; i++) {
    sem_wait(&empty);
    pthread_mutex_lock(&mutex); // Bloquea el recurso del buffer
    int numero = rand() % 100;
    buffer[in] = numero;
    printf("Numero producido por %d: %d\n", id, numero);
    in = (in + 1) % 5;
    pthread_mutex_unlock(&mutex); // Desbloquea el recurso del buffer
    sem_post(&full);
  }

  pthread_exit(0);
}

void *consumidor(void *args) {

  int *buffer = (int *)args;

  for (int i = 0; i < 30; i++) {
    sem_wait(&full);

    pthread_mutex_lock(&mutex);

    int numero = buffer[out];
    out = (out + 1) % 5;
    printf("Numero consumido: %d\n", numero);

    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
  }

  pthread_exit(0);
}


int main() {

  int buffer[5];
  int id1 = 1;
  int id2 = 2;
  int id3 = 3;

  DatosProductor datosProd1 = {buffer, id1};
  DatosProductor datosProd2 = {buffer, id2};
  DatosProductor datosProd3 = {buffer, id3};

  pthread_t tProd1, tProd2, tProd3;
  pthread_t tConsumidor;

  pthread_mutex_init(&mutex, NULL);
  sem_init(&empty, 0, 5);
  sem_init(&full, 0, 0);

  pthread_create(&tProd1, NULL, productor, (void *)&datosProd1);
  pthread_create(&tProd2, NULL, productor, (void *)&datosProd2);
  pthread_create(&tProd3, NULL, productor, (void *)&datosProd3);
  pthread_create(&tConsumidor, NULL, consumidor, (void *)buffer);

  pthread_join(tProd1, NULL);
  pthread_join(tProd2, NULL);
  pthread_join(tProd3, NULL);
  pthread_join(tConsumidor, NULL);

  pthread_mutex_destroy(&mutex);
  sem_destroy(&empty);
  sem_destroy(&full);

  return 0;
}