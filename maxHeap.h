
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <malloc.h>


#define LEFT(n) 2*n + 1
#define RIGHT(n) 2*n + 2
#define PARENT(n) (n-1)/2


typedef struct requestNode {
	unsigned char hash[32];
	uint64_t start;
	uint64_t end;
	uint8_t priority;
} requestNode;


typedef struct maxHeap {
	int heapSize;
	requestNode* elem;
} maxHeap;


// Function: initialize new max heap with size = 0
maxHeap initMaxHeap() {
	maxHeap hp;
	hp.heapSize = 0;
	return hp;
}


// Function: insert new requestnode into maxheap
void insert(maxHeap* mh, requestNode rn) {

	// allocating memory
	if (mh->heapSize) {
		mh->elem = (requestNode *)realloc(mh->elem, (mh->heapSize + 1) * sizeof(rn));
	}
	else {
		mh->elem = (requestNode *)malloc(sizeof(rn));
	}

	// Heapify to ensure max-heap property is satisfied
	int i = (mh->heapSize)++;
	while (i && rn.priority > mh->elem[PARENT(i)].priority) {
		mh->elem[i] = mh->elem[PARENT(i)];
		i = PARENT(i);
	}
	mh->elem[i] = rn;
}


// Function: swap data of two request nodes
void swap(requestNode* rn1, requestNode* rn2) {
	requestNode temp = *rn1;
	*rn1 = *rn2;
	*rn2 = temp;
}


// Function: heapify to ensure max-heap property is satisied
void heapify(maxHeap* mh, int i) {

	int largest = (LEFT(i) < mh->heapSize && mh->elem[LEFT(i)].priority > mh->elem[i].priority) ? LEFT(i) : i;
	if (RIGHT(i) < mh->heapSize && mh->elem[RIGHT(i)].priority > mh->elem[largest].priority) {
		largest = RIGHT(i);
	}
	if (largest != i) {
		swap(&(mh->elem[i]), &(mh->elem[largest]));
		heapify(mh, largest);
	}
}


// Function: extract requestNode with highest priority and remove it from heap
requestNode exctractMax(maxHeap* mh) {

	requestNode maxRN = mh->elem[0];

	mh->elem[0] = mh->elem[--(mh->heapSize)];
	mh->elem = (requestNode *)realloc(mh->elem, mh->heapSize * sizeof(requestNode));
	heapify(mh, 0);

	return maxRN;
}

