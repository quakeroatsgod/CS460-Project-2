#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h> 
#include <sys/time.h>

#define FCFS_ALG 0
#define SJF_ALG 1
#define PR_ALG 2
#define RR_ALG 3

#define INPUT_FINISHED 0
#define TOTAL_JOBS 1
#define JOBS_COMPLETE 2

#define IN_DEBUG 0
#define IO_DEBUG 0
#define C_DEBUG 0

#pragma once 

// Linked list node to hold a process
typedef struct _lnode_t    {
    int priority;
    int bursts_count;
    int burst_indicator;
    int pid;
    int *burst_times;
    double time_waited;
    struct timeval arrival_time;
    struct timeval wait_began;
    struct _lnode_t *previous;
    struct _lnode_t *next;
}lnode_t;

typedef struct _list_t {
    lnode_t *head;
    lnode_t *tail;
}list_t;

// Doubly-linked list
list_t * list_init();
list_t * list_add(list_t *list, int priority, int bursts_count, int *burst_times, int pid, struct timeval arrival_time);
int list_print(list_t *list);
void free_list(list_t *list);
lnode_t * list_pop(list_t *list);
list_t * list_insert(list_t *list, lnode_t *node);

// Node specific functions
lnode_t * create_node(int priority, int bursts_count, int *burst_times, int pid, struct timeval arrival_time);
void * free_node(lnode_t *node);
lnode_t * remove_node(list_t *list, lnode_t *node);

// Input thread
int input_thread_init(pthread_t *input_thread, FILE *fp);
int input_thread_join(pthread_t input_thread);
void * input_thread_run(void *data);

// CPU thread
int cpu_thread_init(pthread_t *cpu_thread, int *alg_and_quantum);
int cpu_thread_join(pthread_t cpu_thread);
void * cpu_thread_run(void *data);
lnode_t * cpu_select_FCFS();
lnode_t * cpu_select_SJF();
lnode_t * cpu_select_PR();
lnode_t * cpu_select_RR(int quantum);
lnode_t * cpu_burst_normal(lnode_t *node);
lnode_t * cpu_burst_RR(lnode_t *node, int quantum_time );

// IO thread
int io_thread_init(pthread_t *io_thread);
int io_thread_join(pthread_t io_thread);
void * io_thread_run(void *data);

// Output
void print_output(char *filename, float throughput, int alg_type, int quantum_time);
double time_in_ms(struct timeval start, struct timeval end);
struct timeval get_time();

//Mutexes
int get_global(int caseVal);