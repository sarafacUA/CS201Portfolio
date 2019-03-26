#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif



/* CS201 Portfolio Initial description and notes
 * Alex Rafac
 * 11740097
 *
 * The project I have chosen is to create the game of boggle using the graph, so that the user
 * can ask for an unspecified boggle game size, and the game will search and find valid words
 * that the user can guess within a certain period of time.
 */

typedef struct node {
	char letter;
	int visited;
	int visitedFrom;
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

typedef struct stack {
	Node * nodePtr;
	struct stack * next;
} Stack;
Stack * top;

Node * add(Node * head, int random) {
	Node * newNode = NULL;
	newNode = (Node *)malloc(sizeof(Node));
	if (!random) {
		newNode -> letter = 'A' + rand() % 26;
	} else {
		newNode -> letter = random;
	}
	newNode -> next = head;
	head -> prev = newNode;
	head = newNode;
	return head;
}

void push(Node * word) {
	Stack * temp = NULL;
	temp = (Stack *)malloc(sizeof(Stack));
	if (temp == NULL) {
		printf("stack overflow\n");
	}
	temp -> nodePtr = word;
	temp -> next = top;
	top = temp;
}

void pop() {
	Stack * temp;
	if (top == NULL) {
		printf("stack underflow\n");
	}
	temp = top;
	top = top -> next;
	temp -> next = NULL;
	free(temp);
}

void display() {
	Stack * temp;
	if (top == NULL) {
		printf("stack underflow\n");
	}
	temp = top;
	while (temp != NULL) {
		printf("%c", temp -> nodePtr -> letter);
		temp = temp -> next;
	}
	printf("\n");
}

int getStr(char * stackString) {
	int count = 0;
	int size = 0;
	Stack * temp;
	if (top == NULL) printf("Stack underflow\n");
	for (temp = top; temp != NULL; temp = temp -> next) count++;
	size = count;
	for (int i = 19; i >= count; i--) stackString[i] = '\0';
	for (temp = top; temp != NULL; temp = temp -> next) stackString[--count] = temp -> nodePtr -> letter;
	//printf("PATH: %s\n", stackString);
	return size;
}

int validPath(char letter, int depth, int f) {
	//printf("testing %c with depth %d and starting at line %d\n", letter, depth, f);
	char lineRead[70];
	int lineCount = 2;
	int firstValid = 0;;
	int lastValid = 0;
	FILE * ptrFile = NULL;
	ptrFile = fopen("Collins Scrabble Words (2015).txt", "r");
	if (!ptrFile) printf("error: file not found");
	for (int i = 0; i < 2; i++) fgets(lineRead, sizeof(lineRead), ptrFile);
	for (int i = 0; i < f; i++) fgets(lineRead, sizeof(lineRead), ptrFile); //JUMP TO FIRST LEGAL CONDITION
	char previousDepth = '.';
       	if (depth - 1 > -1) previousDepth = lineRead[depth - 1];
	//printf("previous depth should be %c\n", previousDepth);
	lineCount += f;
	while(!feof(ptrFile)) {
		fgets(lineRead, sizeof(lineRead), ptrFile);
		if (previousDepth != lineRead[depth - 1] && previousDepth != '.') break;
		if (lineRead[depth] == letter) {
			if (firstValid == 0) firstValid = lineCount;
			//printf("LINE %d: %s\n", lineCount, lineRead);
		}
		if (lineRead[depth] != letter && firstValid != 0 && lastValid == 0) lastValid = lineCount - 1;
		lineCount++;
	}
	if (firstValid != 0 && lastValid == 0) lastValid = lineCount - 1;
	fseek(ptrFile, 0, SEEK_SET);
	// PRINT BOUNDARIES OF LEGAL VALUES
	for (int i = 0; i <= firstValid; i++) fgets(lineRead, sizeof(lineRead), ptrFile);
	//printf("LINE %d: %s", firstValid, lineRead);
	for (int i = firstValid; i < lastValid; i++) fgets(lineRead, sizeof(lineRead), ptrFile);
	//printf("LINE %d: %s", lastValid, lineRead);
	fclose(ptrFile);
	//printf("\n");
	return firstValid;
}

void addAnswer(char * word) {
	char existingWord[20];
	int duplicate = 1;
	FILE * fp = NULL;
	//printf("valid word found, adding %s\n", word);
	fp = fopen("Answer List.txt", "a+");
	if (!fp) printf("Error: no file found");
	fseek(fp, 0, SEEK_SET);
	fscanf(fp, "%s", existingWord);
	while(!feof(fp)) {
		//printf("%s\n", existingWord);
		//printf("comparing %s against %s\n", word, existingWord);
		duplicate = strcmp(word, existingWord);
		if (duplicate == 0) break;
		fscanf(fp, "%s", existingWord);
	}
	if (duplicate != 0) fprintf(fp, "%s\r\n", word);
	fclose(fp);
}

void validWord(char * stackString, int testFrom) {
	char lineRead[70];
	char wordCheck[20];
	FILE * ptrFile = NULL;
	int count = 0;
	Stack * temp;
	if (top == NULL) printf("Stack underflow\n");
	for (temp = top; temp != NULL; temp = temp -> next) count++;
	for (int i = 19; i >= count; i--) stackString[i] = '\0';
	for (temp = top; temp != NULL; temp = temp -> next) stackString[--count] = temp -> nodePtr -> letter;
	//printf("STRING TO COMPARE IS: %s\n", stackString);
	ptrFile = fopen("Collins Scrabble Words (2015).txt", "r");
	if (!ptrFile) printf("Error: file not found");
	for (int i = 0; i < 2; i++) fgets(lineRead, sizeof(lineRead), ptrFile);
	for (int i = 0; i <= testFrom - 2; i++) fscanf(ptrFile, "%s", wordCheck);
	while(!feof(ptrFile)) {
		//printf("%s\n", wordCheck);
		int valid = 1;
		valid = strcmp(stackString, wordCheck);
		if (valid == 0) addAnswer(wordCheck);
		//if (valid == 0) printf("VALID: %s == %s\n", stackString, wordCheck);
		if (valid < 0) break;
		//printf("comparing %s against %s, got value %d\n", stackString, wordCheck, valid);
		fgets(lineRead, sizeof(lineRead), ptrFile);
		fscanf(ptrFile, "%s", wordCheck);	
	}
	fclose(ptrFile);
}

void checkNodes(char * stackString, int startCheck) {
	int stackSize = getStr(stackString);
	int testFrom = validPath(top -> nodePtr -> letter, stackSize - 1, startCheck);
	validWord(stackString, testFrom);
	top -> nodePtr -> visited = 1;
	if (testFrom != 0) {
		if (top -> nodePtr -> nw != NULL && top -> nodePtr -> nw -> visited == 0) {
		//	printf("NorthWest open\n");
			push(top -> nodePtr -> nw);
		//	display();
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> n != NULL && top -> nodePtr -> n -> visited == 0) {
		//	printf("North open\n");
			push(top -> nodePtr -> n);
		//	display();
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> ne != NULL && top -> nodePtr -> ne -> visited == 0) {
		//	printf("NorthEast open\n");
			push(top -> nodePtr -> ne);
		//	display();
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> w != NULL && top -> nodePtr -> w -> visited == 0) {
		//	printf("West open\n");
			push(top -> nodePtr -> w);
		//	display();
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> e != NULL && top -> nodePtr -> e -> visited == 0) {
		//	printf("East open\n");
			push(top -> nodePtr -> e);
		//	display();
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> sw != NULL && top -> nodePtr -> sw -> visited == 0) {
		//	printf("SouthWest open\n");
			push(top -> nodePtr -> sw);
		//	display();
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> s != NULL && top -> nodePtr -> s -> visited == 0) {
		//	printf("South open\n");
			push(top -> nodePtr -> s);
		//	display();
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> se != NULL && top -> nodePtr -> se -> visited == 0) {
		//	printf("SouthEast open\n");
			push(top -> nodePtr -> se);
		//	display();
			checkNodes(stackString, testFrom);
		}
	}
	//printf("nowhere left to go, going back\n");
	top -> nodePtr -> visited = 0;
	pop();
	//display();
}

void DFS(Node * word) {
	char stackString[20];
	int testFrom = 0;
	push(word);
	//display();
	checkNodes(stackString, testFrom);
}


int main(void) {
	srand(time(0));
	int length = 0;
	int depth = 0;
	float timeDuration;
	FILE * fp = NULL;
	fp = fopen( "Answer List.txt", "w");
	fclose(fp);
	printf("Test to determine size of boggle board user wants\n");
	printf("Enter length of board: ");
	scanf("%d", &length);
	printf("Enter depth of board: ");
	scanf("%d", &depth);
	printf("Enter timer duration for guessing: ");
	scanf("%f", &timeDuration);
	printf("\n");	
	Node * head = NULL;
	head = (Node *)malloc(sizeof(Node));
	if (head == NULL) {
		printf("Error message: allocation failure of head");
		return -1;
	}
	head -> letter = 'A' + rand() % 26;
	head -> next = NULL;
	
	for (int i = 0; i < length * depth - 1; i++) {
		head = add(head, 0);
		if (head == NULL) {
			printf("Error message: allocation failure during push");
		}
	}
	int count = 0;
	for (Node * ptr = head; ptr != NULL; ptr = ptr -> next) {
		printf("[ %c ] ", ptr -> letter);
		count++;
		if (count % length == 0) printf("\n\n");
	}
	//printf("second pass of declarations; changing pointers for completed graph\n");
	
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
			index -> visited = 0;
			index -> visitedFrom = 0;
			index = index -> next;
		}
	}
	for (Node * word = head; word != NULL; word = word -> next) DFS(word);
	
	char answer[20];
	int exists = 0;
	fp = fopen("Answer List.txt", "r");
	fscanf(fp, "%s", answer);
	while (!feof(fp)) {
		printf("%s\n", answer);
		fscanf(fp, "%s", answer);
		exists++;
	}
	if (exists == 0) printf("no valid words found\n");
	fclose(fp);
	
	time_t start_t;
	time_t end_t;
	printf("\nstarting timer to solve boggle board\n");
	time(&start_t);
	time(&end_t);
	while (difftime(end_t, start_t) < timeDuration) time(&end_t);
	double diff_t = difftime(end_t, start_t);
	printf("clock took %f seconds\n", diff_t);
	
	printf("\nEnter words found now. type '.' once done\n");
	char guess[30];
	int scoring = 1;
	exists = 0;
	int points = 0;
	while (scoring) {
		scanf("%s", guess);
		fp = fopen("Answer List.txt", "r");
		fscanf(fp, "%s", answer);
		while (!feof(fp)) {
			if (strcmp(guess, answer) == 0) {
				exists++;
				printf("answer found\n");
			}
			fscanf(fp, "%s", answer);
		}
		fclose(fp);
		if (exists > 0) {
			if (strlen(guess) < 2) points = points;
			if (strlen(guess) > 2) ++points;
			if (strlen(guess) > 4) ++points;
			if (strlen(guess) > 5) ++points;
			if (strlen(guess) > 6) points += 2;
			if (strlen(guess) > 7) points += 6;
		}
		if (strcmp(guess, ".") == 0) --scoring;
	}
	printf("\nscore is: %d\n", points);

	return 1;
}
