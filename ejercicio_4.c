// TODO: ejercicio Práctico 4

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t mutex;
sem_t colaA;
sem_t colaB;

int sentidoPuente = -1; // -1: vacio, 0: A, 1: B
int carrosEnPuente = 0;

int esperandoA = 0;
int esperandoB = 0;

typedef struct {
  int id;
  int sentido;
} DatosCarro;

int carrosCruzaron = 0;

void entrarPuente(DatosCarro *data) {
  printf("Vehiculo %d sentido %s entrando al puente\n", data->id, data->sentido == 0 ? "A" : "B");
}

void salirPuente(DatosCarro *data) {
  printf("Vehiculo %d sentido %s saliendo del puente\n", data->id, data->sentido == 0 ? "A" : "B");

  pthread_mutex_lock(&mutex);

  carrosCruzaron++;
  carrosEnPuente--;
  printf("Hay %d vehiculos en el puente\n", carrosEnPuente);

  if (carrosEnPuente == 0) {
    int sentidoPrevio = sentidoPuente;

    if (esperandoA > 0 && sentidoPrevio == 0) { // Si venian A y hay A
      int cantidad = esperandoA;
      sentidoPuente = 0;
      esperandoA = 0;
      for (int i = 0; i < cantidad; i++) {
        sem_post(&colaA);
      }
    } else if (esperandoB > 0 && sentidoPrevio == 1) { // Si venian B y hay B
      int cantidad = esperandoB;
      sentidoPuente = 1;
      esperandoB = 0;
      for (int i = 0; i < cantidad; i++) {
        sem_post(&colaB);
      }
    } else if (esperandoA >
               0) { // Si hay A pero venian B o el puente estaba vacio
      int cantidad = esperandoA;
      sentidoPuente = 0;
      esperandoA = 0;
      for (int i = 0; i < cantidad; i++) {
        sem_post(&colaA);
      }
    } else if (esperandoB >
               0) { // Si hay B pero venian A o el puente estaba vacio
      int cantidad = esperandoB;
      sentidoPuente = 1;
      esperandoB = 0;
      for (int i = 0; i < cantidad; i++) {
        sem_post(&colaB);
      }
    } else { // El puente esta vacio
      sentidoPuente = -1;
    }
  }

  printf("Han cruzado %d vehiculos\n", carrosCruzaron);

  pthread_mutex_unlock(&mutex);
}

void *vehiculo(void *arg) {
  DatosCarro *data = (DatosCarro *)arg;
  sleep(rand() % 20);

  int pudoEntrar = 0;
  int direccion = data->sentido;

  while (pudoEntrar == 0) {

    pthread_mutex_lock(&mutex);
    if (sentidoPuente == -1 || sentidoPuente == direccion) {

      sentidoPuente = direccion;
      carrosEnPuente++;


      entrarPuente(data);
      printf("Hay %d vehiculos en el puente\n", carrosEnPuente);
      pthread_mutex_unlock(&mutex);
      pudoEntrar = 1;

    } else {
      if (data->sentido == 0) {
        esperandoA++;
        pthread_mutex_unlock(&mutex);
        sem_wait(&colaA);

      } else {
        esperandoB++;
        pthread_mutex_unlock(&mutex);
        sem_wait(&colaB);
      }
    }
  }

  sleep(1);
  salirPuente(data);

  free(data);
  pthread_exit(NULL);
}

int main() {

  pthread_t tVehiculo[100];
  srand(time(NULL));

  pthread_mutex_init(&mutex, NULL);
  sem_init(&colaA, 0, 0);
  sem_init(&colaB, 0, 0);

  for (int i = 0; i < 100; i++) {
    DatosCarro *datos = (DatosCarro *)malloc(sizeof(DatosCarro));
    datos->id = i+1;

    if (i % 2 == 0) {
      datos->sentido = 0;
    } else {
      datos->sentido = 1;
    }
    pthread_create(&tVehiculo[i], NULL, vehiculo, datos);
  }

  for (int i = 0; i < 100; i++) {
    pthread_join(tVehiculo[i], NULL);
  }

  pthread_mutex_destroy(&mutex);
  sem_destroy(&colaA);
  sem_destroy(&colaB);

  return 0;
}