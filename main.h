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

// Linked list node
typedef struct _lnode_t    {
    int priority;
    int bursts_remaining;
    struct _lnode_t *previous;
    struct _lnode_t *next;
}lnode_t;

typedef struct _list_t {
    lnode_t *head;
    lnode_t *tail;
}list_t;

// Doubly-linked list
list_t * init();
lnode_t * create_node(int priority, int bursts_remaining);
int free_node(lnode_t *node);
list_t * add(list_t *list, int priority, int bursts_remaining);

// Input thread
int input_thread_init(pthread_t input_thread, FILE *fp);
void * input_thread_run(void *data);
