
#include <stdio.h>
#include <pthread.h>
#include <string.h>

pthread_mutex_t mutex;

#define BUFFER_SIZE 256


void *leerTeclado(void *args){
    char *buffer = (char*) args;
    printf("Escribe algo: ");

    pthread_mutex_lock(&mutex);//  Bloquea el recurso de la terminal
    fgets(buffer, BUFFER_SIZE,  stdin);
    pthread_mutex_unlock(&mutex);// Desbloquea el recurso de la terminal
    
    pthread_exit(0);
}

void *escribirArchivo(void *args){

    FILE *archivo;
    char *buffer = (char*) args;
    printf("Escribiendo en el archivo...\n");

    pthread_mutex_lock(&mutex);//  Bloquea el recurso del archivo
    archivo = fopen("archivo.txt", "w");
    fprintf(archivo, "%s", buffer);
    fclose(archivo);
    pthread_mutex_unlock(&mutex);// Desbloquea el recurso del archivo
    
    printf("Escrito correctamente.\n");
    
    pthread_exit(0);
}

void *leeArchivo(void *args){

    char *buffer = (char*) args;

    pthread_mutex_lock(&mutex);//  Bloquea el recurso del archivo
    FILE *archivo = fopen("archivo.txt", "r");

    fread(buffer, sizeof(char), BUFFER_SIZE, archivo);
    fclose(archivo);

    buffer[strlen(buffer)] = '\0';

    pthread_mutex_unlock(&mutex);// Desbloquea el recurso del archivo

    printf("El archivo dice: %s", buffer);

    pthread_exit(0);
}
    

int main(){
    
    pthread_t t1,t2,t3;
    char buffer[BUFFER_SIZE];

    if(pthread_mutex_init(&mutex, NULL) != 0){
        printf("Error al inicializar el mutex\n");
        return 1;
    }


    pthread_create(&t1, NULL, leerTeclado, buffer);
    pthread_join(t1,NULL);

    pthread_create(&t2, NULL, escribirArchivo, (void*) buffer);
    pthread_create(&t3, NULL, leeArchivo, buffer);

    pthread_join(t2,NULL);
    pthread_join(t3,NULL);

    pthread_mutex_destroy(&mutex);

    return 0;

}