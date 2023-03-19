#include "main.h"

extern list_t *ready_queue;
extern list_t *io_queue;

// Starts up the I/O thread
int io_thread_init(pthread_t *io_thread){    
    if ( 0 < pthread_create( io_thread, NULL, io_thread_run, NULL ) ){
        fprintf(stderr,"Error %d: %s\n", errno, strerror(errno));
        return 1;
    }
    // // Detaches the thread, frees up the thread allocation automatically
    // pthread_detach(io_thread);
    return 0;
}

void * io_thread_run(void *data){
    while( ready_queue->head != NULL ){ //Repeat until there are no more jobs
        lnode_t *io_node = io_queue->head;
        if( io_node != NULL ){
            usleep(1000 * io_node->burst_times[io_node->burst_indicator]); //Sleep for indicated burst time
            //TO DO: Remove node from io_queue and add it to ready queue, updating io_queue
        }
    }
    return NULL;
}