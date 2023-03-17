#include "main.h"

extern list_t *ready_queue;

// Starts up the CPU thread
int cpu_thread_init(pthread_t *cpu_thread){    
    if ( 0 < pthread_create( cpu_thread, NULL, cpu_thread_run, NULL ) ){
        fprintf(stderr,"Error %d: %s\n", errno, strerror(errno));
        return 1;
    }
    // // Detaches the thread, frees up the thread allocation automatically
    // pthread_detach(cpu_thread);
    return 0;
}

void * cpu_thread_run(void *data){
    return NULL;
}