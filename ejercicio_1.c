
#include <stdio.h>
#include <pthread.h>

#define BUFFER_SIZE 256

void *leerTeclado(void *args){
    char *buffer = (char*) args;
    printf("Escribe algo: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    
    pthread_exit(0);
}

void *escribirArchivo(void *args){

    FILE *archivo;
    char *buffer = (char*) args;
    printf("Escribiendo en el archivo...\n");
    archivo = fopen("archivo.txt", "w");
    fprintf(archivo, "%s", buffer);
    fclose(archivo);
    
    printf("Escrito correctamente.\n");
    
    pthread_exit(0);
}

void *leeArchivo(void *args){

    char *buffer = (char*) args;
    
    FILE *archivo = fopen("archivo.txt", "r");

    fread(buffer, sizeof(char), BUFFER_SIZE, archivo);
    fclose(archivo);

    printf("El archivo dice: %s", buffer);

    pthread_exit(0);
}
    

int main(){

    pthread_t t1,t2,t3;
    char buffer[BUFFER_SIZE];
    

    pthread_create(&t1, NULL, leerTeclado, buffer);
    pthread_join(t1,NULL);


    pthread_create(&t2, NULL, escribirArchivo, (void*) buffer);
    pthread_join(t2,NULL);

    pthread_create(&t3, NULL, leeArchivo, buffer);
    pthread_join(t3,NULL);




}