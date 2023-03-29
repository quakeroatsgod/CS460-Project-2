#include "main.h"

extern list_t *ready_queue;
extern pthread_mutex_t ready_mutex;
extern pthread_mutex_t io_mutex;
extern int input_finished;
extern int jobs_completed;
extern int total_jobs;

// Starts up the input thread
int input_thread_init(pthread_t *input_thread, FILE *fp){    
    if ( 0 < pthread_create( input_thread, NULL, input_thread_run, ( void * )fp ) ){
        fprintf(stderr,"Error %d: %s\n", errno, strerror(errno));
        return 1;
    }
    return 0;
}

int input_thread_join(pthread_t input_thread){
    if ( 0 < pthread_join( input_thread, NULL ) ) {
        fprintf( stderr,"Error %d: %s\n", errno, strerror( errno ) );
        return 1;
    }
    return 0;
}

void * input_thread_run(void *data){
    if(IN_DEBUG)    printf("input thred rnning\n");
    FILE *fp = ( FILE * )data;
    size_t buffer_length = 0;
    clock_t arr_time=0;
    char *line_buffer = NULL, *save_ptr = NULL;
    int sleep_duration = 0, proc_priority = 0, proc_count = 0, 
    ready_locked = 0, success = 0, pid = 0, jobs = 0;
    // Iterate through each line in the input file
    while ( getline( &line_buffer,&buffer_length,fp ) != -1 ){
        char *token_first = strtok_r (line_buffer," ", &save_ptr );
        // Add process from input line to ready queue
        if ( strcmp( token_first, "proc") == 0 ) {
            // Enter a loop of trying to get access to the ready queue
            while ( 1 ){
                if( !ready_locked ){
                    if(IN_DEBUG)   printf("input wants ready lock\n");
                    // Get the ready queue mutex if this thread did not lock it
                    success = pthread_mutex_trylock( &ready_mutex );
                    // If successful lock, set the flag to say that this thread has 
                    // access to the ready queue
                    if ( success == 0 )  ready_locked = 1;
                }
                // If the ready queue is locked by this thread, then add to the 
                // ready queue.
                if ( ready_locked ){
                    if(IN_DEBUG)   printf("input has ready queue lock\n");
                    // Get process priority, count, and burst time values after the "proc" token
                    proc_priority = atoi( strtok_r( NULL," ", &save_ptr ) );
                    proc_count = atoi( strtok_r( NULL," ", &save_ptr ) );
                    // Allocate an array of integers of size equal to the proc count
                    int *burst_times = ( int * )malloc( sizeof( int ) * proc_count );
                    for ( int i = 0; i < proc_count; i++ ){
                        burst_times[i] = atoi( strtok_r( NULL," ", &save_ptr ) );
                    }
                    if(IN_DEBUG)   printf("input add node. priority %d and process count %d\n",proc_priority,proc_count);
                    // Add process to ready queue list
                    arr_time = clock();
                    list_add( ready_queue, proc_priority, proc_count, burst_times, pid++, arr_time);
                    jobs++;
                    if(IN_DEBUG)   printf("input added node. priority %d and process count %d\n",proc_priority,proc_count);
                    // Break to leave the loop of trying to add the process
                    break;
                }
            }
        }
        // Wait for sleep duration until next process "arrives"
        else if ( strcmp( token_first, "sleep") == 0 ) {
            // Release the ready queue mutex if this thread locked the queue
            if ( ready_locked ){
                if(IN_DEBUG)   printf("input released ready queue lock to sleep\n");
                pthread_mutex_unlock( &ready_mutex );
                ready_locked = 0;
            }
            // Get sleep duration value after the "sleep" token
            sleep_duration = atoi( strtok_r( NULL," ", &save_ptr ) );
            if(IN_DEBUG)   printf("input going to sleep for %d ms\n", sleep_duration);
            usleep( 1000 * sleep_duration );
        }
        // End of input
        else if ( strcmp( token_first, "stop") == 0 ){
            // Release the ready queue mutex if this thread locked the queue
            if ( ready_locked ){
                if(IN_DEBUG)   printf("input thread released ready queue lock to exit\n");
                pthread_mutex_unlock( &ready_mutex );
                ready_locked = 0;
            }
            total_jobs = jobs;
            break;
        }   
    }
    // Global flag to let other threads know that there is no more input
    input_finished = 1;
    if(IN_DEBUG)   list_print( ready_queue );

    free( line_buffer );
    if(IN_DEBUG)   printf( "input thred done\n" );
    return NULL;
}

// Get the next integer from string tokens
// Don't use this, for some reason it doesn't work lol
int get_next_int( char *save_ptr ){
    return atoi( strtok_r( NULL," ", &save_ptr ) );
}

