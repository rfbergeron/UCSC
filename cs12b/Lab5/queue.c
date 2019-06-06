//balanced
//Authro: Robert Bergeron
//ID: rbergero
//Main and only file for Lab4. Largely original, though some was taken from the C background info
//posed on Piazza, and switch statement syntax from StackExchange, but its basically the same as
//Java anyways

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

struct RefQueue {
	int number;
	struct RefQueue *next;
}

int enqueue(int toQueue, struct RefQueue *current) {
	while(current->next != null) {
		*current = current->next;
	}
	
	current->next = malloc(sizeof (struct RefQueue));
	
	return 0;
}

int dequeue(struct RefQueue *head) {
	if(head == null) {
		return -1;
	}
	else {
		int toDequeue = head->number;
		struct RefQueue* temp = head;
		*head = head->next;
		free(temp);
	}
	
	return toDequeue;
}

void printQueue(struct RefQueue *current, FILE* out) {
	
	while(current != null) {
		int toPrint = current->number;
		out.printf("%d ", &toPrint);
		current = current->next;
	}
	
	//out.printf("\n");
}

int getSize(struct RefQueue *current) {
	int count = 0;
	if(*current != null) {
		count++;
		*current = current->next;
	}
	
	return count;
}
