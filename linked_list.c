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
lnode_t * create_node(int priority, int bursts_count, int *burst_times, int pid, struct timeval arrival_time){
    lnode_t *new_node = ( lnode_t * )malloc( sizeof( lnode_t ) );
    // Malloc failure
    if( new_node == NULL ){
        fprintf( stderr,"Error: no more space\n" );
        exit( 1 );
    }
    // Give node the data
    new_node->priority = priority;
    new_node->bursts_count = bursts_count;
    new_node->burst_indicator = 0;
    new_node->pid = pid;
    new_node->burst_times = burst_times;
    new_node->time_waited = 0.00;
    new_node->arrival_time = arrival_time;
    new_node->wait_began = arrival_time;
    new_node->next = NULL;
    new_node->previous = NULL;
    return new_node;
}

// Frees the allocation for a single node
void * free_node(lnode_t *node){
    free( node->burst_times );
    free( node ) ;
    return NULL;
}

// Free the allocation for the whole list
void free_list(list_t *list){
    list->tail->next=NULL; //Disconnect tail from head
    for ( lnode_t *ptr = list->head; ptr->next != NULL; ){
        lnode_t *nextPtr = ptr->next;
        free_node(ptr);
        ptr = nextPtr;
    }
    free(list);
}

// Removes a specific node from the list and returns it
lnode_t * remove_node(list_t *list, lnode_t *node){
    // If the removed node was the head
    if ( node == list->head ){
        return list_pop(list);
    }
    // If there are no nodes remaining in the list, the head and tail become NULL
    if ( list->head == list->tail ){
        list->head = NULL;
        list->tail = NULL;
    }
    node->previous->next = node->next;
    node->next->previous = node->previous;
    // If the removed node was the tail
    if ( node == list->tail ){
        list->tail = node->previous;
    }
    node->previous = NULL;
    node->next = NULL;
    return node;
}

// Remove node from head of queue and return it
lnode_t * list_pop(list_t *list){
    // Get head of list
    lnode_t *popped_head = list->head;
    // If there are no nodes remaining in the list, the head and tail become NULL
    if ( list->head == list->tail ){
        list->head = NULL;
        list->tail = NULL;
    }
    // Else, if there is one node remaining in the list, the head and tail become the same
    else if ( list->head->next == list->tail ){
        list->head = list->tail;
        list->head->next = list->tail;
        list->tail->next = list->tail;
        list->head->previous = list->tail;
        list->tail->previous = list->tail;
    }
    // Else, there are at least two nodes in the list remaining
    else {
        // The node after the head becomes the new head
        list->head = popped_head->next;
        // Circular list, so head's previous points to the tail
        list->head->previous = list->tail;
        // The tail points to the new head
        list->tail->next = list->head;
    }
    popped_head->previous = NULL;
    popped_head->next = NULL;
    return popped_head;
}

// Pushes node to the head of the list. Only use this if you know
// for sure that the node needs to be the HEAD, NOT the TAIL
lnode_t * list_push_head(list_t *list, lnode_t *node){
    lnode_t *old_head = list->head;
    list->head = node;
    list->tail->next = node;
    list->head->previous = list->tail;
    list->head->next = old_head;
    old_head->previous = list->head;
    return list->head;
}

// Adds a single node to the linked list by providing node information as parameters
list_t * list_add(list_t *list, int priority, int bursts_count, int *burst_times, int pid, struct timeval arrival_time){
    // Allocate space for and fill in the data for a node
    lnode_t *new_node = create_node( priority, bursts_count, burst_times, pid, arrival_time);
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

// Adds a single node to the linked list. There is no allocation, the node's
// pointers are changed.
list_t * list_insert(list_t *list, lnode_t *node){
    // If the list is empty, make the node loop back to itself
    // The single node is both the head and tail
    if ( list->head == NULL && list->tail == NULL ){
        node->next = node;
        node->previous = node;
        list->head = node;
        list->tail = node;
    }
    // Else, add node to tail of list
    else{ 
        lnode_t *tail = list->tail;
        tail->next = node;
        node->previous = tail;
        node->next = list->head;
        list->tail = node;
    }
    return list;
}

// Prints out contents in a linked list
int list_print(list_t *list){
    printf( "Printing out list contents:\n" );
    if ( list->head == NULL && list->tail == NULL){
        printf( "Empty list!\n" );
        return 1;
    }
    // Iterate through list nodes
    for ( lnode_t *ptr = list->head; ptr->next != list->head; ptr = ptr->next ){
        if ( ptr == NULL )  return 0;
        // Print out priority and bursts remaining
        printf( "Pid: %d, Priority: %d, Burst count: %d, Burst indicator: %d, Bursts: ", ptr->pid, ptr->priority, ptr->bursts_count, ptr->burst_indicator );
        // Don't segfault if there are no burst times for some reason
        if ( ptr->burst_times != NULL ){
            // Iterate through and print out burst times for the process
            for ( int i = 0; i < ptr->bursts_count; i++ ){
                printf( "%d ", ptr->burst_times[i] );
            }
        }
        printf( "\n" );
    }
    return 0;
}


// for ( lnode_t *ptr; ptr->next != list->head; ptr = ptr->next );
