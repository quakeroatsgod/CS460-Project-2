#include "main.h"
list_t *ready_queue;
list_t *io_queue;
pthread_mutex_t ready_mutex;
pthread_mutex_t io_mutex;
pthread_mutex_t in_fin_mutex, tot_job_mutex, job_complete_mutex;
int input_finished, jobs_completed, total_jobs, alg_type, quantum_time;
double total_wait_time, total_turnaround_time;

// ./exec -alg [FCFS|SJF|PR|RR] [-quantum [integer(ms)]] -input [filename]
int main(int argc, char **argv){
    
    // Variables 
    int arg_current_counter = 1;
    // First element is the algorithm type, second is the quantum time
    int *alg_and_quantum = ( int * ) malloc( sizeof( int ) * 2 ); 
    alg_and_quantum[0] =  0, alg_and_quantum[1] = 0;
    FILE *fp = NULL;
    char *filename = NULL;
    pthread_t input_thread = 0, io_thread = 0, cpu_thread = 0;
    struct timeval start, end;
    float throughput = 0.0;
    input_finished = 0, jobs_completed = 0, total_jobs = 1, alg_type = 0, quantum_time = 0;

    //No argument inputs
    if ( argc < 2 ) {
        printf( "Error, usage: ./exec -alg [FCFS|SJF|PR|RR] [-quantum [integer(ms)]] -input [filename]\n");
        return 1;
    }
    // Algorithm option
    if ( strcmp(argv[1],"-alg") == 0 ) {
        if ( strcmp(argv[2],"FCFS") == 0 )     alg_and_quantum[0] = FCFS_ALG;
        else if ( strcmp(argv[2],"SJF") == 0 )     alg_and_quantum[0] = SJF_ALG;
        else if ( strcmp(argv[2],"PR") == 0 )     alg_and_quantum[0] = PR_ALG;
        else if ( strcmp(argv[2],"RR") == 0 )     alg_and_quantum[0] = RR_ALG;
        arg_current_counter = 3;
    }
    // Quantum option and time
    if ( strcmp( argv[arg_current_counter],"-quantum" ) == 0 ){
        alg_and_quantum[1] = atoi( argv[arg_current_counter+1] );
        arg_current_counter += 2;
    }
    // Input file option
    if ( strcmp( argv[arg_current_counter++],"-input" ) == 0 ){
        fp = fopen( argv[arg_current_counter], "r" );
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
    cpu_thread_init( &cpu_thread, alg_and_quantum );

    //Run threads and time runtime
    gettimeofday(&start, NULL); //Start clock as threads start
    input_thread_join( input_thread );
    io_thread_join( io_thread );
    cpu_thread_join( cpu_thread );
    gettimeofday(&end, NULL); //End clock once threads terminate

    //Calcuate throughput and print data
    throughput = time_in_ms(start,end);
    print_output(argv[arg_current_counter], throughput, alg_and_quantum[0], alg_and_quantum[1]);
    
    //Free data
    free ( io_queue );
    free ( ready_queue );
    fclose( fp );
    free( alg_and_quantum );
    return 0;
}

//Print the output data in the format specified in the design document
void print_output(char *filename, float throughput, int alg_type, int quantum_time){
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
    printf("%-32s: %0.1f\n","Avg. Turnaround Time", ( float ) total_turnaround_time / total_jobs );
    printf("%-32s: %0.1f\n","Avg. Waiting Time in Ready Queue", ( float ) total_wait_time / total_jobs );
}

//Utility Function that returns the time in milliseconds within a period
double time_in_ms(struct timeval start, struct timeval end){
    return ( ( double ) ( end.tv_sec - start.tv_sec )*1e3 + 1e-3*(end.tv_usec - start.tv_usec));
}

//Utility function that returns a struct timeval with the current time
struct timeval get_time(){
    struct timeval time;
    gettimeofday(&time,NULL);
    return time;
}

//Mutex wrapper function to access global variables between threads
int get_global(int caseVal){
    int waiting = 1, success = 0, result = 0;
    switch (caseVal)
    {
    case INPUT_FINISHED:
        while(waiting){
            success = pthread_mutex_trylock(&in_fin_mutex);
            if(success==0) waiting = 0;
        }
        result = input_finished;
        pthread_mutex_unlock(&in_fin_mutex);
        break;
    case TOTAL_JOBS:
        while(waiting){
            success = pthread_mutex_trylock(&tot_job_mutex);
            if(success==0) waiting = 0;
        }
        result = total_jobs;
        pthread_mutex_unlock(&tot_job_mutex);
        break;
    case JOBS_COMPLETE:
        while(waiting){
            success = pthread_mutex_trylock(&job_complete_mutex);
            if(success==0) waiting = 0;
        }
        result = jobs_completed;
        pthread_mutex_unlock(&job_complete_mutex);
        break;
    }
    return result;
}