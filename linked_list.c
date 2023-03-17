#include "main.h"
// Init a linked list with a sentinel node that is the head and tail. 
list_t * init(){
    list_t *list = ( list_t * )malloc( sizeof( list_t ) );
    lnode_t *sentinel = ( lnode_t * )malloc( sizeof( lnode_t ) );
    sentinel->next = sentinel;
    sentinel->previous = sentinel;
    list->head = sentinel;
    list->tail = sentinel;
    return list;
}

lnode_t * create_node(int priority, int bursts_remaining){
    lnode_t *new_node = ( lnode_t * )malloc( sizeof( lnode_t ) );
    if( new_node == NULL ){
        fprintf( stderr,"Error: no more space\n" );
        exit( 1 );
    }
    new_node->priority = priority;
    new_node->bursts_remaining = bursts_remaining;

    return new_node;
}

int free_node(lnode_t *node){
    // free( node.priority );
    // free( node.bursts_remaining );
    free( node ) ;
    return 0;
}

list_t * add(list_t *list, int priority, int bursts_remaining){
    lnode_t *tail = list->tail;
    lnode_t *new_node = create_node( priority, bursts_remaining);
    tail->next = new_node;
    new_node->previous = tail;
    new_node->next = list->head;
    list->tail = new_node;
    return list;
}


// for ( lnode_t *ptr; ptr->next != list->head; ptr = ptr->next );
