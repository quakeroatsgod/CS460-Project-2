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
        if( !io_locked ){
            if(IO_DEBUG) printf("io wants io lock\n");
            success = pthread_mutex_trylock(&io_mutex); //Try to lock io queue
            if(success == 0) io_locked = 1;
        }
        if( io_locked ){
            if(IO_DEBUG) printf("io has io lock\n");
            lnode_t *node = io_queue->head; //Get IO Node
            pthread_mutex_unlock(&io_mutex); //Unlock mutex
            if(IO_DEBUG) printf("io released io lock\n");
            io_locked = 0;
            if(node != NULL){
                remove_node(io_queue,node); //Pop Node from front of IO queue
                usleep(1000 * node->burst_times[node->burst_indicator]); //Sleep for indicated burst time
                while(!ready_locked){
                    if(IO_DEBUG) printf("io wants ready lock\n");
                    success = pthread_mutex_trylock(&ready_mutex); //Try to lock ready queue
                    if(success == 0) ready_locked = 1;
                }
                if(IO_DEBUG) printf("io has ready lock\n");
                list_insert(ready_queue,node);
                ready_locked = 0;
                pthread_mutex_unlock(&ready_mutex);
                if(IO_DEBUG) printf("io released ready lock\n");
            }
        }
    }
    if(IO_DEBUG)   printf("io thred done\n");
    return NULL;
}