//queue (header)
//Authro: Robert Bergeron
//ID: rbergero
//Main and only file for Lab4. Largely original, though some was taken from the C background info
//posed on Piazza, and switch statement syntax from StackExchange, but its basically the same as
//Java anyways

#ifndef _QUEUE_H_INCLUDE_
#define _QUEUE_H_INCLUDE_

#include <stdio.h>
#include <stdlib.h>

typedef struct RefQueue* Queue;

int enqueue(int toQueue, struct Queue* current);

int dequeue(struct Queue* head);

void printQueue(struct Queue* current, FILE out);

#endif

