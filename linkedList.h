

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


requestNode* getRequestNode(unsigned char hash[32], uint64_t start, uint64_t end, uint8_t priority, int clientfd) {

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
	DoubleLinkedList* dll = (DoubleLinkedList*)malloc(sizeof(DoubleLinkedList));
	if (dll == NULL)
	{
		// Failed to create memory 
		return NULL;
	}
	// Set head , tail and list size
	dll->head = NULL;
	dll->tail = NULL;
	dll->listSize = 0;

	return dll;
}


// Function: add new requestNode to end of linked list
void insert(DoubleLinkedList* dll, unsigned char hash[32], uint64_t start, uint64_t end, uint8_t priority, int clientfd) {

	dll->listSize++;

	// Create a new node
	requestNode* newNode = getRequestNode(hash, start, end, priority, clientfd);

	if ((dll->head == NULL))
	{
		// Add first node
		dll->head = newNode;
		dll->tail = newNode;
		return;
	}
	// Add node at last position
	dll->tail->next = newNode;
	newNode->prev = dll->tail;
	dll->tail = newNode;

}



// Delete node value by priority value
void deleteNode(DoubleLinkedList* dll, int p) {

	requestNode* temp = NULL;
	if ((dll->head == NULL)) {
		// When linked list is empty
		printf("Empty linked list");
	}

	// node to be deleted is head
	if ((dll->head->priority == p)) {

		// When remove head
		temp = dll->head;
		dll->head = dll->head->next;

		if ((dll->head != NULL)) {
			dll->head->prev = NULL;
		}
		else {
			// When linked list empty after delete 
			dll->tail = NULL;
		}
		free(temp);
		return;
	}

	// node to be deleted is tail
	else if ((dll->tail->priority == p)) {

		// When remove last node
		temp = dll->tail;
		dll->tail = dll->tail->prev;

		if ((dll->tail != NULL)) {
			dll->tail->next = NULL;
		}
		else {
			// Remove all nodes
			dll->head = NULL;
		}
		free(temp);
		return;
	}


	// node to be deleted is neither head or tail in list
	else {

		temp = dll->head;

		while (temp != NULL && temp->priority != p) {
			temp = temp->next;
		}
		if ((temp == NULL)) {

			// Node key not exist
			printf("Deleted node are not found");
			return;
		}
		else {

			// Separating deleted node And combine next and previous node
			temp->prev->next = temp->next;
			if ((temp->next != NULL)) {
				// When deleted intermediate nodes
				temp->next->prev = temp->prev;
			}

			free(temp);
			return;
		}
	}
}




// Function: extract request with highest priority and remove it from linked list
requestNode extractMax(DoubleLinkedList* dll) {


	uint8_t max = dll->head->priority;
	(dll->listSize)--;

	// Get first node of linked list
	requestNode* temp = dll->head;

	requestNode maxPriorityRequest = *temp;


	// iterate linked list 
	while (temp != NULL) {
		if (temp->priority > max) {
			max = temp->priority;
			maxPriorityRequest = *temp;
		}
		// Visit to next node
		temp = temp->next;
	}


	deleteNode(dll, max);


	return maxPriorityRequest;
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
