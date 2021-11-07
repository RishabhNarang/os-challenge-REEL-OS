#ifndef QUEUE_H
#define QUEUE_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct node{
    struct node* next;
    struct node* prev;
    int64_t* value;
} node;

typedef struct Queue{
    size_t size;
    struct node* head;
    struct node* tail;
} Queue;


struct node* node_new(int64_t* val, struct node* previous, struct node* nextNode){
    struct node* newNode = malloc(sizeof(struct node));
    newNode->value = val;
    newNode->prev = previous;
    newNode->next = nextNode;
    
    return newNode;
};

void enqueue(struct Queue* queue, int64_t* val){
    // printf("Adding to queue %ld \n", *val);

    if(queue->size == 0){
        struct node* tmp = node_new(val, NULL, NULL);

        queue->head = tmp;
        queue->tail = tmp;
        queue->size++;

        char n = tmp->next == NULL ? 'N' : *tmp->next->value;
        char e = tmp->prev == NULL ? 'N' : *tmp->prev->value;
        printf("%ld <- %ld -> %ld \n", n, *val, e);

        return;
    }   

    struct node* tmp = node_new(val, NULL, queue->tail);
    queue->tail->prev = tmp;
    queue->tail = tmp;
    queue->size++;


    // Output to check if added with the correct references
    // char n = tmp->next == NULL ? 'N' : *tmp->next->value;
    // char e = tmp->prev == NULL ? 'N' : *tmp->prev->value;
    // printf("%ld <- %ld -> %ld \n", n, *val, e);

};

int64_t* dequeue(struct Queue* queue){
    if(queue->size <= 0){return NULL;}

    // Update 
    int64_t* tmp = queue->head->value;
    struct node* prevNode = queue->head->prev;

    // printf("Removing from queue %ld \n", *tmp);

    queue->head->prev = NULL;
    free(queue->head);

    if(queue->size == 1){
        queue->size--;
        queue->head = NULL;
        queue->tail = NULL;
        return tmp;
    }

    prevNode->next = NULL;
    queue->head = prevNode;
    queue->size--;

    return tmp;

};

void print_queue(struct Queue* queue){
    if(queue->size <= 0){
        printf("Empty queue \n");
        return;
    }
        
    struct node* currentNode = queue->head;
    
    if(queue->head->next == NULL)
        printf("NULL ");

    for(size_t i = 0; i < queue->size; i++){

        printf("%ld ", *currentNode->value);
        currentNode = currentNode->prev;
    }

    if(currentNode == NULL)
        printf("NULL");

    printf("\n");
}

struct Queue* queue_new(){
    struct Queue* q = malloc(sizeof(struct Queue));
    q->size = 0;
    q->head = NULL;
    return q;
}

struct Queue* queue_new2(int64_t* val){
    struct Queue* q = malloc(sizeof(struct Queue));
    struct node* n = node_new(val, NULL, NULL);
    q->size = 1;
    q->head = n;
    return q;
}


void DeleteQueue(struct Queue* queue){
    size_t originalSize = queue->size;
    

    for(size_t i = 0; i < originalSize; i++){
        dequeue(queue);
    }

    free(queue);
}


#endif