#include "main.h"
// ./exec -alg [FCFS|SJF|PR|RR] [-quantum [integer(ms)]] -input [filename]
int main(int argc, char **argv){
    // Variables 
    int alg_type = 0;
    int arg_current_counter = 1;
    int quantum_time = 0;
    FILE *fp = NULL;
    char *filename = NULL;
    pthread_t input_thread = 0, io_thread = 0, cpu_thread = 0;

    // TODO Handle no argument inputs
    // Algorithm option
    if ( strcmp(argv[1],"-alg") == 0 ) {
        if(strcmp(argv[2],"FCFS") == 0 )     alg_type = FCFS_ALG;
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
    if ( strcmp(argv[arg_current_counter++],"-input" ) == 0 ){
        fp = fopen(argv[arg_current_counter], "r" );
        filename = ( char * )malloc(sizeof( char ) * strlen( argv[arg_current_counter] ) );
        strcpy(filename,argv[arg_current_counter] );
        if ( fp == NULL ) {
            fprintf( stderr,"Error, cannot open: %s\n",filename );
            return 1;
        }
    }

    input_thread_init(input_thread, fp);
    usleep(400000);
    // printf("brufh\n");
    // I have no idea why, but joining the thread caused a segfault. 
    // Detached thread did not

    // if ( 0 < pthread_join( input_thread, NULL ) ) {
    //     fprintf(stderr,"Error %d: %s\n", errno, strerror(errno));
    //     return 1;
    // }
    // printf("bruh\n");
    alg_type+=1;
    quantum_time+=1;
    io_thread+=1;
    cpu_thread+=1;
    free( filename );
    fclose( fp );
    return 0;
}