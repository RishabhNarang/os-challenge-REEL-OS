
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <malloc.h>


// A node of the double linked list
typedef struct requestNode {

	unsigned char hash[32];
	uint64_t start, end;
	uint8_t priority;
	int clientfd;
	struct requestNode* next, *prev;

} requestNode;


requestNode* getRequestNode(unsigned char hash[32], uint64_t start, uint64_t end, uint8_t priority , int clientfd) {

	// Create dynamic memory of LinkNode
	struct requestNode* newReqNode = (requestNode*)malloc(sizeof(requestNode));
	if (newReqNode == NULL) {
		// Failed to create memory 
		return NULL;
	}
	newReqNode->hash[32] = hash[32];
	newReqNode->start = start;
	newReqNode->end = end;
	newReqNode->priority = priority;
	newReqNode->clientfd = clientfd;

	newReqNode->next = NULL;
	newReqNode->prev = NULL;
	return newReqNode;
}


typedef struct DoubleLinkedList {

	// Define useful field of DoublyLinkedList
	struct requestNode* head;
	struct requestNode* tail;
	int listSize;

}DoubleLinkedList;


DoubleLinkedList* getDoubleLinkedList() {

	// Create dynamic memory of DoublyLinkedList
	DoubleLinkedList* linkedList = (DoubleLinkedList*)malloc(sizeof(DoubleLinkedList));
	if (linkedList == NULL) {
		// Failed to create memory 
		return NULL;
	}
	// Set head, tail and list size
	linkedList->head = NULL;
	linkedList->tail = NULL;
	linkedList->listSize = 1;
	return linkedList;
}


// Function: add new requestNode to end of linked list
void insert(DoubleLinkedList* linkedList, unsigned char hash[32], uint64_t start, uint64_t end, uint8_t priority, int clientfd) {

	// Create a new request node
	requestNode* rn = getRequestNode(hash, start, end, priority, clientfd);
	if ((linkedList->head == NULL)) {
		// Add first node
		linkedList->head = rn;
		linkedList->tail = rn;
		return;
	}

	// Add node to end of linked list
	linkedList->tail->next = rn;
	rn->prev = linkedList->tail;
	linkedList->tail = rn;
	(linkedList->listSize)++;
}


// Function: extract request with highest priority and remove it from linked list
requestNode extractMax(DoubleLinkedList* linkedList) {

	uint8_t max = linkedList->head->priority;
	requestNode* nextNode, * prevNode;
	nextNode = NULL;
	prevNode = NULL;
	(linkedList->listSize)--;

	// Get first node of linked list
	requestNode* temp = linkedList->head;

	requestNode maxPriorityRequest = *temp;

	// iterate linked list 
	while (temp != NULL) {
		if (temp->priority > max) {
			max = temp->priority;
			maxPriorityRequest = *temp;
			prevNode = temp->prev;
			nextNode = temp->next;

		}
		// Go to next request node
		temp = temp->next;
	}
	prevNode->next = nextNode;
	nextNode->prev = prevNode;

	return maxPriorityRequest;
}


//Print linked list
void printLinkedList(struct requestNode* nodePtr) {

	printf("Doubly Linked List\n");
	while (nodePtr != NULL) {
		printf("%d", nodePtr->priority);
		nodePtr = nodePtr->next;
		if (nodePtr != NULL)
			printf("-><-");
	}
} 
