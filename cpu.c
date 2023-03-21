#include "main.h"

extern list_t *ready_queue;
extern list_t *io_queue;
extern pthread_mutex_t ready_mutex;
extern pthread_mutex_t io_mutex;
extern int input_finished;
extern int alg_type;
extern int quantum_time;
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
    if(C_DEBUG)   printf("cpu thred rnnin\n");
    int ready_locked = 0, io_locked = 0, success = 0;
    lnode_t *node = NULL;
    // Loop over and over of checking the ready queue and seeing if there are processes to run
    while ( !input_finished || ready_queue->head != NULL ){
        if ( !ready_locked ) {
            if(C_DEBUG)   printf("cpu wants ready lock to get job\n");
            // Get the ready queue mutex if this thread did not lock it
            success = pthread_mutex_trylock( &ready_mutex );
            // If successful lock, set the flag to say that this thread has 
            // access to the ready queue
            if ( success == 0 )  ready_locked = 1;
        }
        // If the ready queue is locked by this thread, then get next process to CPU burst
        if ( ready_locked ){
            if(C_DEBUG)   printf("cpu has ready queue lock\n");
            // If there is at least one node in the ready queue
            if ( ready_queue->head != NULL ){
                switch ( alg_type ){
                    case FCFS_ALG:
                        list_print(ready_queue);
                        node = cpu_select_FCFS();
                        break;
                    case SJF_ALG:
                        node = cpu_select_SJF();
                        break;
                    case PR_ALG:
                        node = cpu_select_PR();
                        break;
                    case RR_ALG:
                        node = cpu_select_RR( quantum_time );
                        break;
                }
                if ( node != NULL ){
                    if(C_DEBUG)   printf("cpu got pid %d from ready queue\n", node->pid);
                    // After getting a node using some algorithm, remove it from the
                    // ready queue to move it to the I/O queue
                    remove_node(ready_queue, node);
                }
            }
            // Remove ready queue mutex lock after (maybe) getting a node 
            pthread_mutex_unlock( &ready_mutex );
            ready_locked = 0;
            if(C_DEBUG)   printf("cpu released ready queue lock \n");
            // Sleep for length of quantum or CPU burst time
            if ( node != NULL ){
                if(C_DEBUG)   printf("cpu bursting for %d ms on pid %d\n", node->burst_times[node->burst_indicator], node->pid);
                if ( alg_type != RR_ALG ){
                    node = cpu_burst_normal( node );
                }
                else
                    node = cpu_burst_RR( node, quantum_time );
            }
            // If node was deleted from bursting, continue checking ready queue,
            // and don't do any of the stuff after this if statement
            if ( node == NULL ){
                continue;
            }
            if(C_DEBUG)   printf("cpu pid %d has %d bursts left\n", node->pid, node->bursts_count - node->burst_indicator);
            // Else, add process to I/O queue after finishing CPU burst
            if ( node->burst_indicator % 2 == 1 ){
                // Maybe wait to get io mutex
                while ( !io_locked ){
                    if(C_DEBUG)   printf("cpu wants io lock\n");
                    // Try to get the io queue mutex and then perform the list insert
                    success = pthread_mutex_trylock( &io_mutex );
                    if ( success == 0 )  io_locked = 1;
                }
                list_insert( io_queue, node );
                // Release io mutex and continue onto next process
                io_locked = 0;
                pthread_mutex_unlock( &io_mutex );
                if(C_DEBUG)   printf("cpu released io lock\n");
                continue;
            }
            // Add process back onto ready queue if using RR and the quantum time is up,
            // but NOT the CPU burst.
            if ( node->burst_indicator % 2 == 0 && alg_type == RR_ALG ){
                // Maybe wait to get io mutex
                while ( !ready_locked ){
                    if(C_DEBUG)   printf("cpu wants ready lock to add process\n");
                    // Try to get the io queue mutex and then perform the list insert
                    success = pthread_mutex_trylock( &ready_mutex );
                    if ( success == 0 )  ready_locked = 1;
                }
                list_insert( ready_queue, node );
                // Release ready mutex and continue onto next process
                ready_locked = 0;
                pthread_mutex_unlock( &ready_mutex );
                if(C_DEBUG)   printf("cpu released ready lock to add process\n");
                continue;
            }
        }
    }
    if(C_DEBUG)   printf("cpu thred done\n");
    return NULL;
}

// Get a node from the ready queue based on FCFS (First Come First Serve)
lnode_t * cpu_select_FCFS(){
    lnode_t *first_come_node = ready_queue->head;
    // Iterate through ready queue and find process with lowest PID
    for ( lnode_t *ptr = ready_queue->head; ptr->next != ready_queue->head; ptr = ptr->next ){
        // If the current node has a lower PID than the previous lowest,
        // change that node to have the highest priority
        if ( ptr->pid < first_come_node->pid ){
            first_come_node = ptr;
        }
    }
    return first_come_node;
}

// Get a node from the ready queue based on SJR (Shortest Job First)
lnode_t * cpu_select_SJF(){
    lnode_t *node = NULL;
    return node;
}

// Get a node from the ready queue based on PR (Priority)
lnode_t * cpu_select_PR(){
    lnode_t *highest_priority_node = ready_queue->head;
    // Iterate through ready queue and find process with highest priority
    for ( lnode_t *ptr = ready_queue->head; ptr->next != ready_queue->head; ptr = ptr->next ){
        // If the current node has a higher priority than the previous highest,
        // change that node to have the highest priority
        if ( ptr->priority > highest_priority_node->priority ){
            highest_priority_node = ptr;
        }
    }
    return highest_priority_node;
}

// Get a node from the ready queue based on RR (Round Robin)
lnode_t * cpu_select_RR(int quantum){
    lnode_t *node = NULL;
    return node;
}
// Perform CPU burst and update process node info
lnode_t * cpu_burst_normal(lnode_t *node){
    // Sleep for length of CPU burst
    usleep( 1000 * node->burst_times[node->burst_indicator ++]);
    node->burst_indicator ++;
    // If process performed its last CPU burst, delete the process node
    if ( node->burst_indicator == node->bursts_count ){
        if (C_DEBUG)    printf( "node %d is done bursting. Freeing it from memory\n", node->pid);
        return free_node( node );
    }
    // Return the node if there are more bursts to do
    return node;
}

// Perform CPU burst and update process node info. The CPU burst time is
// handled slightly differently for Round Robin (RR)
lnode_t * cpu_burst_RR(lnode_t *node, int quantum){
    int burst_time = quantum;
    // If the remaining burst time is less than the quantum, use that time for
    // the CPU burst time.
    if ( node->burst_times[node->burst_indicator] <= quantum )
        burst_time = node->burst_times[node->burst_indicator];
    // Sleep for length of CPU burst or quantum
    usleep( 1000 * burst_time );
    // Update burst indicator if the current CPU burst expired before the quantum
    node->burst_times[node->burst_indicator] -= quantum;
    if ( node->burst_times[node->burst_indicator] <= 0 )
        node->burst_indicator ++;
    // If process performed its last CPU burst, delete the process node
    if ( node->burst_indicator == node->bursts_count ){
        if (C_DEBUG)    printf( "node %d is done bursting. Freeing it from memory\n", node->pid);
        return free_node( node );
    }
    // Return the node if there are more bursts to do
    return node;
}