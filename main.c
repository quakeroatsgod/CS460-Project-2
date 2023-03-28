#include "main.h"
list_t *ready_queue;
list_t *io_queue;
pthread_mutex_t ready_mutex;
pthread_mutex_t io_mutex;
int input_finished, cpu_finished, jobs_completed, total_jobs, alg_type, quantum_time;
int total_wait_time, total_turnaround_time;
// ./exec -alg [FCFS|SJF|PR|RR] [-quantum [integer(ms)]] -input [filename]
int main(int argc, char **argv){
    
    // Variables 
    int arg_current_counter = 1;
    FILE *fp = NULL;
    char *filename = NULL;
    pthread_t input_thread = 0, io_thread = 0, cpu_thread = 0;
    // struct timeval start, end;
    clock_t start = 0, end = 0;
    float throughput = 0.0;
    input_finished = 0, cpu_finished = 0, jobs_completed = 0, total_jobs = 1, alg_type = 0, quantum_time = 0;

    //No argument inputs
    if ( argc < 2 ) {
        printf( "Error, usage: ./exec -alg [FCFS|SJF|PR|RR] [-quantum [integer(ms)]] -input [filename]\n");
        return 1;
    }
    // Algorithm option
    if ( strcmp(argv[1],"-alg") == 0 ) {
        if ( strcmp(argv[2],"FCFS") == 0 )     alg_type = FCFS_ALG;
        else if ( strcmp(argv[2],"SJF") == 0 )     alg_type = SJF_ALG;
        else if ( strcmp(argv[2],"PR") == 0 )     alg_type = PR_ALG;
        else if ( strcmp(argv[2],"RR") == 0 )     alg_type = RR_ALG;
        arg_current_counter = 3;
    }
    // Quantum option and time
    if ( strcmp( argv[arg_current_counter ++],"-quantum" ) == 0 ){
        quantum_time = atoi( argv[arg_current_counter ] );
        arg_current_counter ++;
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
    // Init lists, mutexes, and threads
    ready_queue = list_init();
    io_queue = list_init();

    pthread_mutex_init( &ready_mutex,NULL );
    pthread_mutex_init( &io_mutex,NULL );

    input_thread_init( &input_thread, fp );
    io_thread_init( &io_thread );
    cpu_thread_init( &cpu_thread );

    // gettimeofday(&start,NULL); //Start clock as threads start
    start = clock();
    input_thread_join( input_thread );
    io_thread_join( io_thread );
    cpu_thread_join( cpu_thread );
    // gettimeofday(&end,NULL); //End clock once threads terminate
    // throughput = ((end.tv_usec-start.tv_usec)/1000)/input_finished; //Throughput = runtime/# of jobs
    end = clock();
    throughput = ( ( float ) ( end - start ) / CLOCKS_PER_SEC ) * 1000.0;
    print_output(filename, throughput);
    free( filename );
    fclose( fp );
    return 0;
}

void print_output(char *filename, float throughput){
    char algo[5]="";
    switch ( alg_type ){ //Assign algo based on alg_type
        case FCFS_ALG:
            strcpy(algo,"FCFS");
            break;
        case SJF_ALG:
            strcpy(algo,"SJF");
            break;
        case PR_ALG:
            strcpy(algo,"PR");
            break;
        case RR_ALG:
            strcpy(algo,"RR");
            break;
    }
    printf("%-32s: %s\n","Input File Name",filename); //Print File Name
    if(strcmp(algo,"RR")==0){ //Print Round Robin with Quantum Time
        printf("%-32s: %s %d\n","CPU Scheduling Alg",algo,quantum_time);
    }
    else{ //Print Algorithm Used
        printf("%-32s: %s\n","CPU Scheduling Alg",algo);
    }
    printf("%-32s: %0.3f\n","Throughput",( float ) total_jobs / throughput); //Print Throughput
    printf("%-32s: %0.3f\n","Avg. Turnaround Time", ( float ) total_turnaround_time / total_jobs );
    printf("%-32s: %0.3f\n","Avg. Waiting Time in Ready Queue", ( float ) total_wait_time / total_jobs );
}