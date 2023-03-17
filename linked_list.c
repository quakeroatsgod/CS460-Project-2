#include "main.h"

extern list_t *ready_queue;

// Initializes a linked list wrapper with NO nodes inside it
list_t * list_init(){
    list_t *list = ( list_t * )malloc( sizeof( list_t ) );
    list->head = NULL;
    list->tail = NULL;
    return list;
}

// Creates a new linked list node by allocating space and giving it data 
lnode_t * create_node(int priority, int bursts_remaining, int *burst_times){
    lnode_t *new_node = ( lnode_t * )malloc( sizeof( lnode_t ) );
    // Malloc failure
    if( new_node == NULL ){
        fprintf( stderr,"Error: no more space\n" );
        exit( 1 );
    }
    // Give node the data
    new_node->priority = priority;
    new_node->bursts_remaining = bursts_remaining;
    new_node->burst_times = burst_times;
    return new_node;
}

// Frees the allocation for a single node
int free_node(lnode_t *node){
    // free( node.priority );
    // free( node.bursts_remaining );
    free( node->burst_times );
    free( node ) ;
    return 0;
}

// Adds a single node to the linked list
list_t * list_add(list_t *list, int priority, int bursts_remaining, int *burst_times){
    // Allocate space for and fill in the data for a node
    lnode_t *new_node = create_node( priority, bursts_remaining, burst_times);
    // If the list is empty, make the node loop back to itself
    // The single node is both the head and tail
    if ( list->head == NULL && list->tail == NULL ){
        new_node->next = new_node;
        new_node->previous = new_node;
        list->head = new_node;
        list->tail = new_node;
    }
    // Else, add node to tail of list
    else{ 
        lnode_t *tail = list->tail;
        tail->next = new_node;
        new_node->previous = tail;
        new_node->next = list->head;
        list->tail = new_node;
    }
    return list;
}

// Prints out contents in a linked list
int list_print(list_t *list){
    printf( "Printing out list contents:\n" );
    // Iterate through list nodes
    for ( lnode_t *ptr = list->head; ptr->next != list->head; ptr = ptr->next ){
        // Print out priority and bursts remaining
        printf( "Priority: %d, Bursts Remaining: %d, Bursts: ", ptr->priority, ptr->bursts_remaining );
        // Don't segfault if there are no burst times for some reason
        if ( ptr->burst_times != NULL ){
            // Iterate through and print out burst times for the process
            for ( int i = 0; i < ptr->bursts_remaining; i++ ){
                printf( "%d ", ptr->burst_times[i] );
            }
        }
        printf( "\n" );
    }
    return 0;
}

// for ( lnode_t *ptr; ptr->next != list->head; ptr = ptr->next );
