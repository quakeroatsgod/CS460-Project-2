#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h> 

#define FCFS_ALG 0
#define SJF_ALG 1
#define PR_ALG 2
#define RR_ALG 3

#define IN_DEBUG 0
#define IO_DEBUG 0
#define C_DEBUG 1

#pragma once 

// Linked list node to hold a process
typedef struct _lnode_t    {
    int priority;
    int bursts_count;
    int burst_indicator;
    int *burst_times;
    struct _lnode_t *previous;
    struct _lnode_t *next;
}lnode_t;

typedef struct _list_t {
    lnode_t *head;
    lnode_t *tail;
}list_t;

// Doubly-linked list
list_t * list_init();
list_t * list_add(list_t *list, int priority, int bursts_count, int *burst_times);
int list_print(list_t *list);
void free_list(list_t *list);
lnode_t * list_pop(list_t *list);

// Node specific functions
lnode_t * create_node(int priority, int bursts_count, int *burst_times);
void * free_node(lnode_t *node);
lnode_t * remove_node(list_t *list, lnode_t *node);

// Input thread
int input_thread_init(pthread_t *input_thread, FILE *fp);
int input_thread_join(pthread_t input_thread);
void * input_thread_run(void *data);
int get_next_int( char *save_ptr );

// CPU thread
int cpu_thread_init(pthread_t *cpu_thread);
int cpu_thread_join(pthread_t cpu_thread);
void * cpu_thread_run(void *data);
lnode_t * cpu_select_FCFS();
lnode_t * cpu_select_SJF();
lnode_t * cpu_select_PR();
lnode_t * cpu_select_RR(int quantum);
lnode_t * cpu_burst_normal(lnode_t *node);
lnode_t * cpu_burst_RR(lnode_t *node, int quantum);

// IO thread
int io_thread_init(pthread_t *io_thread);
int io_thread_join(pthread_t io_thread);
void * io_thread_run(void *data);