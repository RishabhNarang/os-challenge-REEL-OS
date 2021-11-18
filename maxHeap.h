// Max-Heap data structure in C

#include <stdio.h>
typedef struct requestNode {
        unsigned char hash[32];
        uint64_t start;
        uint64_t end;
        uint8_t priority;
        int clientfd;
} requestNode;


typedef struct maxHeap {
        int curSize;
	int maxSize;
        requestNode* heap;
} maxHeap;


// Function: initialize new max heap with size = 0
maxHeap * initMaxHeap(int maxSize) {
        maxHeap *hp = (maxHeap *)malloc(sizeof(maxHeap));
	hp->maxSize = maxSize;
        hp->curSize = 0;
	hp->heap = (requestNode *)malloc(sizeof(requestNode) * maxSize);
        return hp;
}
void swap(requestNode *a, requestNode *b)
{
  requestNode temp = *b;
  *b = *a;
  *a = temp;
}
void heapify(maxHeap * hp, int size, int i)
{
  if (size == 1)
  {
    printf("Single element in the heap");
  }
  else
  {
    int largest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < size && hp->heap[l].priority > hp->heap[largest].priority)
      largest = l;
    if (r < size && hp->heap[r].priority > hp->heap[largest].priority)
      largest = r;
    if (largest != i)
    {
      swap(&(hp->heap[i]), &(hp->heap[largest]));
      heapify(hp, size, largest);
    }
  }
}
void insert(maxHeap * hp, requestNode rq)
{
  if (hp->curSize == 0)
  {
    hp->heap[0] = rq;
    hp->curSize += 1;
  }
  else
  {
    hp->heap[hp->curSize] = rq;
    hp->curSize += 1;
    for (int i = (hp->curSize) / 2 - 1; i >= 0; i--)
    {
      heapify(hp, hp->curSize, i);
    }
  }
}
requestNode *extractMax(maxHeap *hp)
{
  requestNode * max = (requestNode *)malloc(sizeof(requestNode));
  *max = hp->heap[0];
  swap(&(hp->heap[0]), &(hp->heap[hp->curSize - 1]));
  hp->curSize -= 1;
  for (int i = (hp->curSize) / 2 - 1; i >= 0; i--)
  {
    heapify(hp, hp->curSize, i);
  }
  return max;
}
void printPriorities(maxHeap * hp)
{
  for (int i = 0; i < hp->curSize; i++)
    printf("%d ", hp->heap[i].priority);
  printf("\n");
}
