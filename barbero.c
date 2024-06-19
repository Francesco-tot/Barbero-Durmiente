#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define NUM_CHAIRS 5 
#define NUM_CUSTOMERS 10  


sem_t customers;
sem_t barber;  
sem_t accessSeats; 
int freeSeats = NUM_CHAIRS;  

void* barber_function(void* arg) {
    while (1) {
        sem_wait(&customers); //Barbero en estado dormilon zzz... (si es cero entonces no hay clientes)
        sem_wait(&accessSeats);  // Regula que solo se pueda modificar el recurso compartido freeSeats un hilo a la vez
        freeSeats++;  // El barbero atendera a un cliente (se aumenta dado que una silla ha sido liberada porque un cliente esta 
        // siendo atendido)
        sem_post(&barber); //  El barbero se encuentra listo para atender (se aumenta dado esta en estado ocupado)
        sem_post(&accessSeats);  // Permite que se pueda ahora modificar el recurso compartido de freeSeats
        printf("El barbero esta cortando el pelo\n"); // Se menciona que el barbero corta el cabello
        sleep(3);  // ha tomado un cierto tiempo el barbero en cortar el cabello
    }
}

void* customer_function(void* arg) {
    sem_wait(&accessSeats); // el recurso compartido no puede ser modificado hasta que el hilo lo deje
    if (freeSeats > 0) { // Entra un cliente (Hay sillas libres para sentarme y esperar al barbero ?)
        // si la respuesta es que hay sillas libres, entonces ingresa aca
        printf("Cliente entro a esperar\n");
        freeSeats--; // se decrementa dado que entro un cliente y se sento entonces ya hay una silla libre menos
        sem_post(&customers);  // El barbero ve que un nuevo cliente a entrado al negocio (si suma uno al contador de los clientes)
        sem_post(&accessSeats);  // se permite ahora modificar el recurso compartido
        sem_wait(&barber); // El barbero esta ocupado cortando cabello, espera tu turno (dado que atiende ya a un cliente decrementa su contador)
        printf("Cliente esta recibiendo un corte de pelo\n"); // mensaje de comprobacion
    } else {
        sem_post(&accessSeats); // en caso de que no hubo sillas libres entonces se permite acceder al recurso compartido
        printf("Cliente se va porque no hay sillas libres\n"); // mensaje de comprobacion
    }
}

int main() {
    pthread_t barberThread; 
    pthread_t customerThreads[NUM_CUSTOMERS]; 

    sem_init(&customers, 0, 0);
    sem_init(&barber, 0, 0);
    sem_init(&accessSeats, 0, 1);

    pthread_create(&barberThread, NULL, barber_function, NULL); 

    for (int i = 0; i < NUM_CUSTOMERS; i++) { 
        pthread_create(&customerThreads[i], NULL, customer_function, NULL);
        sleep(rand() % 3); 
    }

    for (int i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customerThreads[i], NULL);
    }
    pthread_cancel(barberThread);
    pthread_join(barberThread, NULL); 

    sem_destroy(&customers);
    sem_destroy(&barber);
    sem_destroy(&accessSeats);

    return 0;
}

