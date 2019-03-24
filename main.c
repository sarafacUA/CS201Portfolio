#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* CS201 Portfolio Initial description and notes
 * Alex Rafac
 * 11740097
 *
 * The project I have chosen is to create the game of boggle using the graph, so that the user
 * can ask for an unspecified boggle game size, and the game will search and find valid words
 * that the user can guess within a certain period of time.
 */

typedef struct node {
	int letter;
	int visited;
	// pointers used to create a doubly linked list
	struct node * next;
	struct node * prev;
	// pointers that create the graph used
	struct node * nw;
	struct node * n;
	struct node * ne;
	struct node * w;
	struct node * e;
	struct node * sw;
	struct node * s;
	struct node * se;
	
} Node;

Node * push(Node * head, int random) {
	Node * newNode = NULL;
	newNode = (Node *)malloc(sizeof(Node));
	if (!random) {
		newNode -> letter = 'A' + rand() % 26;
	} else {
		newNode -> letter = random;
	}
	printf("%c\n", newNode -> letter);
	newNode -> next = head;
	head -> prev = newNode;
	head = newNode;
	return head;
}

int main (void) {
	srand(time(0));
	int length = 0;
	int depth = 0;
	printf("Test to determine size of boggle board user wants\n");
	printf("Enter length of board: ");
	scanf("%d", &length);
	printf("Enter depth of board: ");
	scanf("%d", &depth);
	
	printf("first pass of declarations; creating linked list\n");
	
	Node * head = NULL;
	head = (Node *)malloc(sizeof(Node));
	if (head == NULL) {
		printf("Error message: allocation failure of head");
		return -1;
	}
	head -> letter = 'A' + rand() % 26;
	head -> next = NULL;
	printf("%c\n", head -> letter);
	
	for (int i = 0; i < length * depth - 1; i++) {
		head = push(head, 0);
		if (head == NULL) {
			printf("Error message: allocation failure during push");
		}
	}
	int count = 0;
	for (Node * ptr = head; ptr != NULL; ptr = ptr -> next) {
		printf("[ %c ] ", ptr -> letter);
		count++;
		if (count % length == 0) printf("\n");
	}
	/*
	Node * ptr = head;
	printf("\n");
	do {
		ptr = ptr -> next;
	} while (ptr -> next != NULL);
	count = 0;
	while (ptr != NULL) {
		printf("[ %c ] ", ptr -> letter);
		count++;
		if (count % length == 0) printf("\n");
		ptr = ptr -> prev;
	}
	*/
	printf("second pass of declarations; changing pointers for completed graph\n");
	
	Node * index = head;
	for (int y = 0; y < depth; y++) {
		for (int x = 0; x < length; x++) {
			Node * ptr = index;
			if (x == 0 && y == 0) {
				// top left corner
				index -> w = NULL;
				index -> e = ptr -> next;
				index -> nw = NULL;
				index -> n = NULL;
				index -> ne = NULL;
				for (int i = 0; i < length; i++) ptr = ptr -> next;
				index -> sw = NULL;
				index -> s = ptr;
				index -> se = ptr -> next;
			}
			else if (x == length - 1 && y == 0) {
				// top right corner
				index -> w = ptr -> prev;
				index -> e = NULL;
				index -> nw = NULL;
				index -> n = NULL;
				index -> ne = NULL;
				for (int i = 0; i < length; i++) ptr = ptr -> next;
				index -> sw = ptr -> prev;
				index -> s = ptr;
				index -> se = NULL;
				
			}
			else if (x == 0 && y == depth - 1) {
				// bottom left corner
				index -> w = NULL;
				index -> e = ptr -> next;
				for (int i = 0; i < length; i++) ptr = ptr -> prev;
				index -> nw = NULL;
				index -> n = ptr;
				index -> ne = ptr -> next;
				index -> sw = NULL;
				index -> s = NULL;
				index -> se = NULL;	
			}
			else if (x == length - 1 && y == depth - 1) {
				// bottom right corner
				index -> w = ptr -> prev;
				index -> e = NULL;
				for (int i = 0; i < length; i++) ptr = ptr -> prev;
				index -> nw = ptr -> prev;
				index -> n = ptr;
				index -> ne = NULL;
				index -> sw = NULL;
				index -> s = NULL;
				index -> se = NULL;
			}
			else if (y == 0) {
				// first row
				index -> w = ptr -> prev;
				index -> e = ptr -> next;
				index -> nw = NULL;
				index -> n = NULL;
				index -> ne = NULL;
				for (int i = 0; i < length; i++) ptr = ptr -> next;
				index -> sw = ptr -> prev;
				index -> s = ptr;
				index -> se = ptr -> next;
			}
			else if (x == 0) {
				// first column
				index -> w = NULL;
				index -> e = ptr -> next;
				for (int i = 0; i < length; i++) ptr = ptr -> prev;
				index -> nw = NULL;
				index -> n = ptr;
				index -> ne = ptr -> next;
				for (int i = 0; i < length * 2; i++) ptr = ptr -> next;
				index -> sw = NULL;
				index -> s = ptr;
				index -> se = ptr -> next;
			}
			else if (x == length - 1) {
				// last column
			       	index -> w = ptr -> prev;
			       	index -> e = NULL;
			       	for (int i = 0; i < length; i++) ptr = ptr -> prev;
			       	index -> nw = ptr -> prev;
			       	index -> n = ptr;
			       	index -> ne = NULL;
			       	for (int i = 0; i < length * 2; i++) ptr = ptr -> next;
			       	index -> sw = ptr -> prev;
			       	index -> s = ptr;
			       	index -> se = NULL;
			}
			else if (y == depth - 1) {
				// last row
				index -> w = ptr -> prev;
				index -> e = ptr -> next;
				for (int i = 0; i < length; i++) ptr = ptr -> prev;
				index -> nw = ptr -> prev;
				index -> n = ptr;
				index -> ne = ptr -> next;
				index -> sw = NULL;
				index -> s = NULL;
				index -> se = NULL;
			}
			else {
				// general case
				index -> w = ptr -> prev;
				index -> e = ptr -> next;
				for (int i = 0; i < length; i++) ptr = ptr -> prev;
				index -> nw = ptr -> prev;
				index -> n = ptr;
				index -> ne = ptr -> next;
				for (int i = 0; i < length * 2; i++) ptr = ptr -> next;
				index -> sw = ptr -> prev;
				index -> s = ptr;
				index -> se = ptr -> next;
			}
			index = index -> next;
		}
	}

	FILE * ptrFile;
	ptrFile = fopen("Collins Scrabble Words (2015).txt", "r");
	if (!ptrFile) {
		printf("Error message: File could not be found or read");
		return -3;
	}

	// fgets(stringRecieved, stringSize?, ptrFile);

	/*
	// DEBUG CODE FOR 3X3 GRAPH
	index = head;	
	printf("TOP LEFT CORNER: e: %c s: %c se: %c\n", 
			index -> e -> letter, index -> s -> letter, index -> se -> letter);
	index = index -> next;
	printf("TOP MIDDLE: w: %c e: %c sw: %c s: %c se: %c\n",
			index -> w -> letter, index -> e -> letter,
			index -> sw -> letter, index -> s -> letter, index -> se -> letter);
	index = index -> next;
	printf("TOP RIGHT CORNER: w: %c sw: %c s: %c\n", 
			index -> w -> letter, index -> sw -> letter, index -> s -> letter);
	index = index -> next;
	printf("LEFT SIDE:  n: %c ne: %c e: %c s: %c se: %c\n",
			index -> n -> letter, index -> ne -> letter, index -> e -> letter,
			index -> s -> letter, index -> se -> letter);
	index = index -> next;
	printf("MIDDLE: nw: %c n: %c ne: %c w: %c e: %c sw: %c s: %c se: %c\n",
			index -> nw -> letter, index -> n -> letter, index -> ne -> letter,
			index -> w -> letter, index -> e -> letter,
			index -> sw -> letter, index -> s -> letter, index -> se -> letter);
	index = index -> next;
	printf("RIGHT SIDE: nw: %c n: %c w: %c sw: %c s: %c\n",
			index -> nw -> letter, index -> n -> letter, index -> w -> letter,
			index -> sw -> letter, index -> s -> letter);
	index = index -> next;
	printf("BOTTOM LEFT CORNER: n: %c ne: %c e: %c\n", 
			index -> n -> letter, index -> ne -> letter, index -> e -> letter);
	index = index -> next;
	printf("BOTTOM MIDDLE: nw: %c n: %c ne: %c w: %c e: %c\n",
			index -> nw -> letter, index -> n -> letter, index -> ne -> letter,
			index -> w -> letter, index -> e -> letter);
	index = index -> next;
	printf("BOTTOM RIGHT CORNER: nw: %c n: %c w: %c\n", 
			index -> nw -> letter, index -> n -> letter, index -> w -> letter);
	*/	
	return 1;

}
