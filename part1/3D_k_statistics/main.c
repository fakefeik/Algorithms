#include <stdio.h>
#include <stdlib.h>

typedef int T;

typedef struct
{
	T *data;
	size_t size;
	size_t capacity;
} Heap;

int _getLeftChildIndex(int nodeIndex) 
{
	return 2 * nodeIndex + 1;
}

int _getRightChildIndex(int nodeIndex) 
{
	return 2 * nodeIndex + 2;
}

int _getParentIndex(int nodeIndex) 
{
	return (nodeIndex - 1) / 2;
}

void _siftDown(Heap *heap, int nodeIndex) 
{
	int leftChildIndex, rightChildIndex, minIndex;
	T tmp;
	leftChildIndex = _getLeftChildIndex(nodeIndex);
	rightChildIndex = _getRightChildIndex(nodeIndex);
	if (rightChildIndex >= heap->size) 
	{
		if (leftChildIndex >= heap->size)
			return;
		else
			minIndex = leftChildIndex;
	}
	else 
	{
		if (heap->data[leftChildIndex] <= heap->data[rightChildIndex])
			minIndex = leftChildIndex;
		else
			minIndex = rightChildIndex;
	}
	if (heap->data[nodeIndex] > heap->data[minIndex]) 
	{
		tmp = heap->data[minIndex];
		heap->data[minIndex] = heap->data[nodeIndex];
		heap->data[nodeIndex] = tmp;
		_siftDown(heap, minIndex);
	}
}

Heap *Heap_create(int capacity)
{
	Heap *heap = (Heap *)malloc(sizeof(Heap));
	heap->data = (T *)malloc(sizeof(T) * capacity);
	heap->capacity = capacity;
	heap->size = 0;
	return heap;
}

void Heap_resize(Heap *heap, int capacity)
{
	heap->data = realloc(heap->data, sizeof(T) * capacity);
	heap->capacity = capacity;
	heap->size = 0;
}

void Heap_dealloc(Heap *heap)
{
	free(heap->data);
	free(heap);
}

void Heap_add(Heap *heap, T element)
{
	if (heap->size == heap->capacity) 
	{
		return;
	}
	else
	{
		heap->data[heap->size++] = element;
		int parentIndex = _getParentIndex(heap->size - 1);
		int nodeIndex = heap->size - 1;
		T temp;
		while (heap->data[parentIndex] > heap->data[nodeIndex]) 
		{
			temp = heap->data[parentIndex];
			heap->data[parentIndex] = heap->data[nodeIndex];
			heap->data[nodeIndex] = temp;
			nodeIndex = parentIndex;
			parentIndex = _getParentIndex(nodeIndex);
		}
	}
}

int Heap_isEmpty(Heap *heap)
{
	return (heap->size == 0);
}

T Heap_deleteMax(Heap *heap) 
{
	if (Heap_isEmpty(heap))
		return -1;

	T max = heap->data[0];
	heap->data[0] = heap->data[heap->size - 1];
	heap->size--;
	if (heap->size > 0) 
		_siftDown(heap, 0);
	return max;
}

int main(int argc, char *argv[])
{
	int n, m, k;
	scanf("%d %d %d", &n, &m, &k);
	int *arr = (int *)malloc(sizeof(int) * n);
	char *commands = (char *)malloc(sizeof(char) * m);
	for (int i = 0; i < n; i++)
		scanf("%d", &arr[i]);
	for (int i = 0; i < m; i++)
		scanf(" %c", &commands[i]);
	
	int leftBorder = 0;
	int rightBorder = 0;
	Heap *heap = Heap_create(0);
	for (int i = 0; i < m; i++)
	{
		switch (commands[i])
		{
		case 'L':
			leftBorder++;
			break;
		case 'R':
			rightBorder++;
			break;
		default:
			break;
		}
		
		if (rightBorder - leftBorder + 1 < k)
		{
			printf("%d\n", -1);
			continue;
		}

		Heap_resize(heap, rightBorder - leftBorder + 1);
		for (int j = leftBorder; j <= rightBorder; j++)
			Heap_add(heap, arr[j]);
		for (int j = 0; j < k - 1; j++)
			Heap_deleteMax(heap);
		printf("%d\n", Heap_deleteMax(heap));
	}
	Heap_dealloc(heap);
}