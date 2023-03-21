#include "main.h"

extern list_t *ready_queue;
extern list_t *io_queue;
extern pthread_mutex_t ready_mutex;
extern pthread_mutex_t io_mutex;
extern int input_finished;

// Starts up the I/O thread
int io_thread_init(pthread_t *io_thread){    
    if ( 0 < pthread_create( io_thread, NULL, io_thread_run, NULL ) ){
        fprintf(stderr,"Error %d: %s\n", errno, strerror(errno));
        return 1;
    }
    return 0;
}

int io_thread_join(pthread_t io_thread){
    if ( 0 < pthread_join( io_thread, NULL ) ) {
        fprintf( stderr,"Error %d: %s\n", errno, strerror( errno ) );
        return 1;
    }
    return 0;
}

void * io_thread_run(void *data){
    if(IO_DEBUG)   printf("io thred rnnin\n");
    int ready_locked = 0, io_locked = 0, success = 0;
    while( !input_finished || ready_queue->head != NULL ){ //Repeat until there are no more jobs
        // TODO lock io queue with mutex, then do IO stuff. Then unlock IO mutex,
        // TODO then lock ready queue and push to ready queue
        while ( !io_locked ) {
            lnode_t *io_node = io_queue->head;
            if( io_node != NULL ){
                usleep(1000 * io_node->burst_times[io_node->burst_indicator]); //Sleep for indicated burst time
                //TO DO: Remove node from io_queue and add it to ready queue, updating io_queue

            }
        }
    }
    if(IO_DEBUG)   printf("io thred done\n");
    return NULL;
}