#include "main.h"

extern list_t *ready_queue;

// Starts up the I/O thread
int io_thread_init(pthread_t io_thread){    
    if ( 0 < pthread_create( &io_thread, NULL, io_thread_run, NULL ) ){
        fprintf(stderr,"Error %d: %s\n", errno, strerror(errno));
        return 1;
    }
    // // Detaches the thread, frees up the thread allocation automatically
    // pthread_detach(io_thread);
    return 0;
}

void * io_thread_run(void *data){
    return NULL;
}