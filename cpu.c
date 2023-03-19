#include "main.h"

extern list_t *ready_queue;
extern list_t *io_queue;
extern pthread_mutex_t ready_mutex;
extern pthread_mutex_t io_mutex;

// Starts up the CPU thread
int cpu_thread_init(pthread_t *cpu_thread){    
    if ( 0 < pthread_create( cpu_thread, NULL, cpu_thread_run, NULL ) ){
        fprintf(stderr,"Error %d: %s\n", errno, strerror(errno));
        return 1;
    }
    return 0;
}

int cpu_thread_join(pthread_t cpu_thread){
    if ( 0 < pthread_join( cpu_thread, NULL ) ) {
        fprintf( stderr,"Error %d: %s\n", errno, strerror( errno ) );
        return 1;
    }
    return 0;
}

void * cpu_thread_run(void *data){
    return NULL;
}