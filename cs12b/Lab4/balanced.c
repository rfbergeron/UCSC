//balanced
//Authro: Robert Bergeron
//ID: rbergero
//Main and only file for Lab4. Largely original, though some was taken from the C background info
//posed on Piazza, and switch statement syntax from StackExchange, but its basically the same as
//Java anyways

#include <stdio.h>
#include <stdlib.h>

struct Stack {
	int maxSize;
	int currSize;
	char s[80];
};

int isEmpty(struct Stack *ref) {
	char* bottom = ref->s;
	return (int) *bottom;
}

struct Stack * createStack(int size) {
	struct Stack *ref = malloc(sizeof (struct Stack));
	ref->maxSize = size;
	ref->currSize = 0;
	*(ref->s) = (char) 0;

	return ref;
}

int push(struct Stack *ref, char toAdd) {
	if(ref->currSize >= ref->maxSize) {
		return 1;
	}
	else {
		char *curr = ref->s + ref->currSize;
		*curr = toAdd;
		ref->currSize++;
		return 0;
	}
}

char peek(struct Stack *ref) {
	if(ref->currSize > 0) {
		char *current = ref->s + (ref->currSize - 1);
		return *current;
	}
	else {
		return *(ref->s);
	}
}

int removeItem(struct Stack *ref) {	
	if(ref->currSize > 0) {
		char *current = ref->s + (ref->currSize - 1);
        	*current = (char) 0;
		ref->currSize--;
	}
	else {
		char *current = ref->s;
        	*current = (char) 0;
	}
	return 0;
}

char pop(struct Stack *ref) {
	char result = peek(ref);
	
	if(removeItem(ref) == 0) {
		return result;
	}
	else {
		return (char) 0;
	}
}

int popAll(struct Stack *ref) {
        while(isEmpty(ref) != 0) {
                pop(ref);
        }
        return 0;
}

int main(int argc, char* argv[]) {
	struct Stack *braceStack = createStack(80);
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
		switch(*currentInput) {
			case '{':
			case '[':
			case '(':
			case '<':
				if(lineValid == 0) {
					//printf("Pushing %c to stack\n", *currentInput);
					push(braceStack, *currentInput);
				}
				break;
			case '}':
				if(peek(braceStack) == '{') {
					//printf("Popping %s! ", peek(braceStack));
                                	pop(braceStack);
					//printf("Next: %s\n", peek(braceStack));
                        	}
                        	else {
					//printf("Line invalid\n");
					lineValid = 1;
				}
				break;
			case ']':
				if(peek(braceStack)  == '[') {
					//printf("Popping %s! ", peek(braceStack));
                                	pop(braceStack);
					//printf("Next: %s\n", peek(braceStack));
                        	}
                        	else {
					//printf("Line invalid\n");
					lineValid = 1;
				}
				break;
			case ')':
				if(peek(braceStack) == '(') {
					//printf("Popping %s! ", peek(braceStack));
                                	pop(braceStack);
					//printf("Next: %s\n", peek(braceStack));
                        	}
                        	else { 
					//printf("Line invalid\n");
					lineValid = 1;
				}
				break;
			case '>':
				if(peek(braceStack) == '<') {
					//printf("Popping %s! ", peek(braceStack));
                                	pop(braceStack);
					//printf("Next: %s\n", peek(braceStack));
                        	}
                        	else {
					//printf("Line invalid\n");
					lineValid = 1;
				}
				break;
			case '\n':
				if(lineValid == 0 && isEmpty(braceStack) == 0) {
					fprintf(out, "Y\n");
					//printf("Line validated!\n");
					popAll(braceStack);
					lineValid = 0;
				}
				else {
					fprintf(out, "N\n");
					popAll(braceStack);
					lineValid = 0 ;
				}
				break;
		}
 	}
	
	fclose(in);
	fclose(out);
	return EXIT_SUCCESS;
} 
