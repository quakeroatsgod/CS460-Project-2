#include "main.h"

extern list_t *ready_queue;

int input_thread_init(pthread_t input_thread, FILE *fp){    
    if ( 0 < pthread_create( &input_thread, NULL, input_thread_run, ( void * )fp ) ){
        fprintf(stderr,"Error %d: %s\n", errno, strerror(errno));
        return 1;
    }
    pthread_detach(input_thread);
    return 0;
}

void * input_thread_run(void *data){
    printf("thred rnning\n");
    FILE *fp = ( FILE * )data;
    size_t buffer_length = 0;
    char *line_buffer = NULL, *save_ptr = NULL, *token_next = NULL;
    int sleep_duration = 0, proc_priority = 0, proc_count = 0;
    proc_count+=1;
    proc_priority+=1;
    // Iterate through each line in the input file
    while ( getline( &line_buffer,&buffer_length,fp ) != -1 ){
        char *token_first = strtok_r (line_buffer," ", &save_ptr );
        // Add process from input line to ready queue
        if ( strcmp( token_first, "proc") == 0 ) {
            // Get process count value after the "proc" token
            // token_next = strtok_r( NULL," ", &save_ptr );
            proc_priority = atoi( strtok_r( NULL," ", &save_ptr ) );
            proc_count = atoi( strtok_r( NULL," ", &save_ptr ) );
            int *burst_times = ( int * )malloc( sizeof( int ) * proc_count );
            for ( int i = 0; i < proc_count; i++ ){
                burst_times[i] = atoi( strtok_r( NULL," ", &save_ptr ) );
            }
            list_add( ready_queue, proc_priority, proc_count, burst_times );
        }
        // Wait for sleep duration until next process "arrives"
        else if ( strcmp( token_first, "sleep") == 0 ) {
            // Get sleep duration value after the "sleep" token
            token_next = strtok_r( NULL," ", &save_ptr );
            sleep_duration = atoi( token_next );
            usleep( 1000 * sleep_duration );
        }
        // End of input
        else if ( strcmp( token_first, "stop") == 0 )   break;
    }

    list_print( ready_queue );

    free( line_buffer );
    printf( "thred done\n" );
    return NULL;
}


