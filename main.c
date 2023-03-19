#include "main.h"
list_t *ready_queue;
list_t *io_queue;

// ./exec -alg [FCFS|SJF|PR|RR] [-quantum [integer(ms)]] -input [filename]
int main(int argc, char **argv){
    // Variables 
    int alg_type = 0;
    int arg_current_counter = 1;
    int quantum_time = 0;
    FILE *fp = NULL;
    char *filename = NULL;
    pthread_t input_thread = 0, io_thread = 0, cpu_thread = 0;

    // No argument inputs
    // if ( argc < 2 ) {
    //     printf( "Error, usage: ./exec -alg [FCFS|SJF|PR|RR] [-quantum [integer(ms)]] -input [filename]\n");
    //     return 1;
    // }
    // Algorithm option
    if ( strcmp(argv[1],"-alg") == 0 ) {
        if ( strcmp(argv[2],"FCFS") == 0 )     alg_type = FCFS_ALG;
        else if ( strcmp(argv[2],"SJF") == 0 )     alg_type = SJF_ALG;
        else if ( strcmp(argv[2],"PR") == 0 )     alg_type = PR_ALG;
        else if ( strcmp(argv[2],"RR") == 0 )     alg_type = RR_ALG;
        arg_current_counter = 3;
    }
    // Quantum option and time
    if ( strcmp( argv[arg_current_counter],"-quantum" ) == 0 ){
        quantum_time = atoi( argv[arg_current_counter] );
        arg_current_counter += 2;
    }
    // Input file option
    if ( strcmp( argv[arg_current_counter++],"-input" ) == 0 ){
        fp = fopen( argv[arg_current_counter], "r" );
        filename = ( char * )malloc(sizeof( char ) * strlen( argv[arg_current_counter] ) );
        strcpy( filename, argv[arg_current_counter] );
        if ( fp == NULL ) {
            fprintf( stderr,"Error, cannot open: %s\n",filename );
            return 1;
        }
    }
    ready_queue = list_init();
    io_queue = list_init();
    input_thread_init( &input_thread, fp );
    io_thread_init( &io_thread );
    cpu_thread_init( &cpu_thread );

    usleep( 400000 );

    if ( 0 < pthread_join( input_thread, NULL ) ) {
        fprintf(stderr,"Error %d: %s\n", errno, strerror(errno));
        return 1;
    }
    if ( 0 < pthread_join( io_thread, NULL ) ) {
        fprintf( stderr,"Error %d: %s\n", errno, strerror( errno ) );
        return 1;
    }
    if ( 0 < pthread_join( cpu_thread, NULL ) ) {
        fprintf( stderr,"Error %d: %s\n", errno, strerror( errno ) );
        return 1;
    }
    alg_type+=1;
    quantum_time+=1;
    free( filename );
    fclose( fp );
    ready_queue->head=NULL; //Test for io_queue
    //free_list(ready_queue); //Free the ready queue
    return 0;
}