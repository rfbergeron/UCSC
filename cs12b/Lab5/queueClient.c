//queueClient
//Authro: Robert Bergeron
//ID: rbergero
//Main and only file for Lab4. Largely original, though some was taken from the C background info
//posed on Piazza, and switch statement syntax from StackExchange, but its basically the same as
//Java anyways

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int main(int argc, char* argv[]) {
	struct Queue* queue = malloc(sizeof(Queue));
	char *currentInput = malloc(sizeof(char));
	int lineValid = 0;
	FILE* in;
	FILE* out;
	
	if(argc < 3) {
		printf("Usage: %s input-file output-file\n", argv[0]);
		return EXIT_FAILURE;
	}

	in = fopen(argv[1], "r");

	if(in == NULL){
		printf("Unable to read from file %s\n", argv[1]);
		exit(EXIT_FAILURE);
 	}

	out = fopen(argv[2], "w");

	if(out == NULL){
		printf("Unable to write to file %s\n", argv[2]);
		exit(EXIT_FAILURE);
	}

	while(fscanf(in, "%c", currentInput ) != EOF){ 
		int toAdd = 0;
		int enqueueLine = 1;
		switch(*currentInput) {
			case 'E':
				fprintf(out, "Enqueue ");
				enqueueLine = 0;
				break;
			case 'D':
				int dequeued = dequeue(queue);
				if(dequeued >= 0) {
					fprintf(out, "Dequeue %d", &dequeued);
				}
				else {
					fprintf(out, "Queue is empty\nDequeue fails");
				}
				break;
			case 'P':
				fprintf(out, "Queue is ");
				printQueue(queue, out);
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				toAdd *= 10;
				toAdd += *currentInput - '0';
				break;
			case '\n':
				if(enqueueLine == 0) {
					enqueue(toAdd, queue);
					toAdd = 0;
					enqueueLine = 1;
				}
				fprintf("%c", currentInput);
				break;
		}
 	}
	
	fclose(in);
	fclose(out);
	return EXIT_SUCCESS;
} 
