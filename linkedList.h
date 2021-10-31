
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <malloc.h>


// A node of the double linked list
struct requestNode {
	unsigned char hash[32];
	uint64_t start, end;
	uint8_t priority;
	struct requestNode* next, *prev;
} *head;


void initialize() {
	head = NULL;
};


// Function: add new requestNode to front of linked list
void insert(unsigned char hash[32], uint64_t start, uint64_t end, uint8_t priority) {

	struct requestNode* newReqNode = (struct requestNode*)malloc(sizeof(struct requestNode));
	newReqNode->hash[32] = hash[32];
	newReqNode->start = start;
	newReqNode->end = end;
	newReqNode->priority = priority;

	newReqNode->next = head;
	newReqNode->prev = NULL;

	if (head != NULL) {
		head->prev = newReqNode;
	}

	// Set newNode as head of doubly linked list 
	head = newReqNode;
	printf("Inserted Priority : %d\n", newReqNode->priority);

} 


// Function: extraxt request with highest priority and remove it from linked list
int getMaxNode(struct requestNode* head) {

	uint8_t max = head->priority;
	struct requestNode* nextNode, * prevNode;
	nextNode = NULL;
	prevNode = NULL;

	while (head != NULL) {
		if (head->priority > max) {
			max = head->priority;
			prevNode = head->prev;
			nextNode = head->next;
		}
		head = head->next;	
	}

	prevNode->next = nextNode;
	nextNode->prev = prevNode;

	return max;
}


void printLinkedList(struct requestNode* nodePtr) {

	printf("Doubly Linked List\n");
	while (nodePtr != NULL) {
		printf("%d", nodePtr->priority);
		nodePtr = nodePtr->next;
		if (nodePtr != NULL)
			printf("-><-");
	}
}
