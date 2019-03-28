#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
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
	int visited;
	struct stack * next;
} Stack;
Stack * top;

Node * add(Node * head) {
	Node * newNode = NULL;
	newNode = (Node *)malloc(sizeof(Node));
	if (newNode == NULL) printf("Error message: allocation error during add");
	newNode -> letter = 'A' + rand() % 26;
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
	/*
	fseek(ptrFile, 0, SEEK_SET);
	// PRINT BOUNDARIES OF LEGAL VALUES
	for (int i = 0; i <= firstValid; i++) fgets(lineRead, sizeof(lineRead), ptrFile);
	//printf("LINE %d: %s", firstValid, lineRead);
	for (int i = firstValid; i < lastValid; i++) fgets(lineRead, sizeof(lineRead), ptrFile);
	//printf("LINE %d: %s", lastValid, lineRead);
	*/
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
	//
	//printf("%s\n", stackString);
	//
	ptrFile = fopen("Collins Scrabble Words (2015).txt", "r");
	if (!ptrFile) printf("Error: file not found");
	for (int i = 0; i < 2; i++) fgets(lineRead, sizeof(lineRead), ptrFile);
	for (int i = 0; i <= testFrom - 2; i++) fscanf(ptrFile, "%s", wordCheck);
	while(!feof(ptrFile)) {
		//printf("%s\n", wordCheck);
		int valid = 1;
		valid = strcmp(stackString, wordCheck);
		if (valid == 0) addAnswer(wordCheck);
		if (valid == 0) printf("VALID: %s == %s\n", stackString, wordCheck);
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
	if (stackSize > 2) validWord(stackString, testFrom);
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

void DFS(Node * head) {
	//char stackString[20];
	//int testFrom = 0;
	for (Node * word = head; word != NULL; word = word -> next) {
		char stackString[20];
		int testFrom = 0;
		push(word);
		//display();
		checkNodes(stackString, testFrom);
	}
}

Node * boggleGraph(int length, int depth) {
	Node * head = NULL;
	head = (Node *)malloc(sizeof(Node));
	if (head == NULL) printf("Error message: allocation failure of head\n");
	head -> letter = 'A' + rand() % 26;
	head -> next = NULL;
	for (int i = 0; i < length * depth - 1; i++) {
		head = add(head);
		if (head == NULL) printf("Error message: allocation failure during push\n");
	}
	Node * index = head;
	for (int y = 0; y < depth; y++) {
		for (int x = 0; x < length; x++) {
			Node * ptr = index;
			if (x == 0 && y == 0) { // top left corner
				index -> w = NULL;
				index -> e = ptr -> next;
				index -> nw = NULL;
				index -> n = NULL;
				index -> ne = NULL;
				for (int i = 0; i < length; i++) ptr = ptr -> next;
				index -> sw = NULL;
				index -> s = ptr;
				index -> se = ptr -> next;
			} else if (x == length - 1 && y == 0) { // top right corner
				index -> w = ptr -> prev;
				index -> e = NULL;
				index -> nw = NULL;
				index -> n = NULL;
				index -> ne = NULL;
				for (int i = 0; i < length; i++) ptr = ptr -> next;
				index -> sw = ptr -> prev;
				index -> s = ptr;
				index -> se = NULL;
				
			} else if (x == 0 && y == depth - 1) { // bottom left corner
				index -> w = NULL;
				index -> e = ptr -> next;
				for (int i = 0; i < length; i++) ptr = ptr -> prev;
				index -> nw = NULL;
				index -> n = ptr;
				index -> ne = ptr -> next;
				index -> sw = NULL;
				index -> s = NULL;
				index -> se = NULL;	
			} else if (x == length - 1 && y == depth - 1) { // bottom right corner
				index -> w = ptr -> prev;
				index -> e = NULL;
				for (int i = 0; i < length; i++) ptr = ptr -> prev;
				index -> nw = ptr -> prev;
				index -> n = ptr;
				index -> ne = NULL;
				index -> sw = NULL;
				index -> s = NULL;
				index -> se = NULL;
			} else if (y == 0) { // first row
				index -> w = ptr -> prev;
				index -> e = ptr -> next;
				index -> nw = NULL;
				index -> n = NULL;
				index -> ne = NULL;
				for (int i = 0; i < length; i++) ptr = ptr -> next;
				index -> sw = ptr -> prev;
				index -> s = ptr;
				index -> se = ptr -> next;
			} else if (x == 0) { // first column
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
			} else if (x == length - 1) { // last column
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
			} else if (y == depth - 1) { // last row
				index -> w = ptr -> prev;
				index -> e = ptr -> next;
				for (int i = 0; i < length; i++) ptr = ptr -> prev;
				index -> nw = ptr -> prev;
				index -> n = ptr;
				index -> ne = ptr -> next;
				index -> sw = NULL;
				index -> s = NULL;
				index -> se = NULL;
			} else { // general case
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
	return head;
}

int score(char * guess) {
	FILE * fp;
	char answer[20];
	int i = 0;
	int points = 0;
	while (guess[i]) {
		guess[i] = toupper(guess[i]);
		i++;
	}
	i = 0;
	fp = fopen("Answer List.txt", "r");
	fscanf(fp, "%s", answer);
	while (!feof(fp)) {
		if (strcmp(guess, answer) == 0) i++;
		fscanf(fp, "%s", answer);
	}
	fclose(fp);

	if (i) {
		switch(strlen(guess)) {
			case 3:
			case 4:
				points = 1;
				break;
			case 5:
				points = 2;
				break;
			case 6:
				points = 3;
				break;
			case 7:
				points = 5;
				break;
			default:
				points = 11;
		}
	}
	return points;
}

void title(void) {
	printf("\n");
	printf(" /$$$$$$$   /$$$$$$   /$$$$$$   /$$$$$$  /$$       /$$$$$$$$\n");
	printf("| $$__  $$ /$$__  $$ /$$__  $$ /$$__  $$| $$      | $$_____/\n");
	printf("| $$  \\ $$| $$  \\ $$| $$  \\__/| $$  \\__/| $$      | $$\n");
	printf("| $$$$$$$ | $$  | $$| $$ /$$$$| $$ /$$$$| $$      | $$$$$\n");
	printf("| $$__  $$| $$  | $$| $$|_  $$| $$|_  $$| $$      | $$__/\n");
	printf("| $$  \\ $$| $$  | $$| $$  \\ $$| $$  \\ $$| $$      | $$\n");
	printf("| $$$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/| $$$$$$$$| $$$$$$$$\n");
	printf("|_______/  \\______/\\ \\______/  \\______/ |________/|________/\n");
	printf("\n");
}

void displayBoard(Node * head, int length, int depth) {
	printf("\n");
	for (int x = 0; x < length; x++) printf("+---");
	printf("+\n");
	for (int y = 0; y < depth; y++) {
		for (int x = 0; x < length; x++) { 
			printf("| %c ", head -> letter);
			head = head -> next;
		}
		printf("|\n");
		for (int x = 0; x < length; x++) printf("+---");
		printf("+\n");
	}
	printf("\n");
}

int sanitizedInt(int validRange, int validLimit, char * buffer, char * message) {
	printf("%s", message);
	scanf("%s", buffer);
	int validInt = 1;
	while (validInt) {
		for (int i = 0; i < strlen(buffer); i++) if (isdigit(buffer[i]) == 0) validInt = 0;
		if (!validInt) {
			printf("Invalid Input\n%s", message);
			scanf("%s", buffer);
			validInt = 1;
		} else if (validRange > atoi(buffer)) {
			printf("Invalid Input\n%s", message);
			scanf("%s", buffer);
			validInt = 1;
		} else if (validLimit > 0 && validLimit < atoi(buffer)) {
			printf("Invalid Input\n%s", message);
			scanf("%s", buffer);
			validInt = 1;
		} else validInt = 0;
	}
	return atoi(buffer);
}

void countDown(int timer, char * message) {
	time_t start_t, end_t;
	time(&start_t);
	time(&end_t);
	while(difftime(end_t, start_t) < timer) time(&end_t);
	printf("%s\n", message);

}

int guessValid(char * buffer) {
	printf("Please input '.' once done entering words\n");
	int scoring = 1;
	int points = 0;
	while (scoring) {
		scanf("%s", buffer);
		points += score(buffer);
		if (strcmp(buffer, ".") == 0) --scoring;
	}
	return points;
}

void freeGraph(Node * head) {
	Node * temp;
	while(head != NULL) {
		temp = head;
		head = head -> next;
		free(temp);
	}
}

int main (void) {
	srand(time(0));
	Node * head = NULL;
	int length = 2;
	int height = 2;
	int playerCount = 2;
	int playerScore[12];
	float timer = 1;
	//time_t start_t, end_t;
	FILE * fp = NULL;
	fp = fopen("Answer List.txt", "w");
	fclose(fp);
	title();
	int playing = 1;
	char multiplayerChoice;
	int multiplayerMenu = 1;
	int multiplayer = 1;
	int multiplayerSetup = 1;
	char buffer[1000];
	while (playing) {
		int menuInput = 0;
		printf("MAIN MENU\n\n");
		printf("1: SINGLEPLAYER MODE (1)\n");
		printf("2: MULTIPLAYER MODE (2-6)\n");
		printf("3: BOGGLE SOLVER\n");
		printf("4: Quit\n\n");
		scanf("%s", buffer);
		printf("\n");
		if (strlen(buffer) == 1) menuInput = buffer[0];
		switch (menuInput) {
			case '1':
				length = sanitizedInt(2, 0, buffer, "Please Enter Length of Boggle Board (length > 1): ");
				height = sanitizedInt(2, 0, buffer, "Please Enter Leight of Boggle Board (height > 1): ");
				timer = sanitizedInt(1, 0, buffer, "Please Enter Timer for Play (timer > 0): ");
				printf("\nSetting Up Board. Loading\n");
				head = boggleGraph(length, height);
				time_t startTime_t, endTime_t;
				time(&startTime_t);
				DFS(head);
				time(&endTime_t);
				printf("\nDFS Algorithm took %lf seconds!\n\n", difftime(endTime_t, startTime_t));
				displayBoard(head, length, height);
				//scanf("%s", buffer);
				countDown(1, "3");
				countDown(1, "2");
				countDown(1, "1");
				countDown(1, "GO");
				countDown(timer, "Time's Up!");
				playerScore[0] = guessValid(buffer);
				printf("You got %d points!\n", playerScore[0]);
				freeGraph(head);
				break;
			case '2':
				for (int i = 0; i < 12; i++) playerScore[i] = 0;
				playerCount = sanitizedInt(2, 6, buffer, "Please Enter Number of Players (2 - 6): ");
				while (multiplayer) {
					if (multiplayerSetup) {
						length = sanitizedInt(2, 0, buffer, "Please Enter Length of Boggle Board (length > 1): ");
						height = sanitizedInt(2, 0, buffer, "Please Enter Height of Boggle Board (height > 1): ");
						timer = sanitizedInt(1, 0, buffer, "Please Enter Timer for Play (timer > 0): ");
					}
					printf("\nSetting Up Board. Loading\n");
					head = boggleGraph(length, height);
					DFS(head);
					displayBoard(head, length, height);
					countDown(1, "3");
					countDown(1, "2");
					countDown(1, "1");
					countDown(1, "GO");
					countDown(timer, "Time's Up!");
					for (int i = 0; i < playerCount; i++) {
						printf("Player #%d:\n", i + 1);
						playerScore[i] = guessValid(buffer);
						printf("Player #%d got %d points!\n\n", i + 1, playerScore[i]);
					}
					int winner = -1;
					int temp = -1;
					for (int i = 0; i < playerCount; i++) {
						if (playerScore[i] > temp) {
							temp = playerScore[i];
							winner = i;
						}
					}
					playerScore[winner + 6] += 1;
					printf("Player #%d wins!\n\nGame Count:\n", winner + 1);
					for (int i = 0; i < playerCount; i++) {
						printf("Player #%d: %d\n", i + 1, playerScore[i + 6]);
					}
					printf("\n");
					freeGraph(head);
					while (multiplayerMenu) {
						printf("1: Play again with same players, same board settings\n");
						printf("2: Play again with same players, new boad settings\n");
						printf("3: Return to main menu\n\n");
						scanf("%s", buffer);
						printf("\n\n");
						if (strlen(buffer) == 1) multiplayerChoice = buffer[0];
						switch (multiplayerChoice) {
							case '1':
								multiplayerSetup = 0;
								multiplayerMenu = 0;
								break;
							case '2':
								multiplayerSetup = 1;
								multiplayerMenu = 0;
								break;
							case '3':
								multiplayerSetup = 1;
								multiplayerMenu = 0;
								multiplayer = 0;
								break;
							default:
								printf("Invalid Input\n");
						}
					}
					multiplayerMenu = 1;
				}
				multiplayer = 1;
				break;
			case '3':
				length = sanitizedInt(2, 0, buffer, "Please Enter Length of Boggle Board (length > 1): ");
				height = sanitizedInt(2, 0, buffer, "Please Enter Height of Boggle Board (height > 1): ");
				head = boggleGraph(length, height);
				Node * userInput = head;
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < length; x++) {
						char letter;
						printf("Input letter for space, (%d, %d): ", x, y);
						scanf(" %c", &letter); //SANITIZE
						letter = toupper(letter);
						userInput -> letter = letter;
						userInput = userInput -> next;
					}
				}
				displayBoard(head, length, height);
				printf("Solving board above. Please Be Patient\n");
				DFS(head);
				freeGraph(head);
				break;
			case '4':
				printf("Thanks for playing!\n");
				--playing;
				break;
			default:
				printf("Invalid Input\n");
		}
		printf("\n");
	}
	
	/* Check to see if any words worth value were found
	char answer[20];
	int exists = 0;
	fp = fopen("Answer List.txt", "r");
	fscanf(fp, "%s", answer);
	while (!feof(fp)) {
		//printf("%s\n", answer);
		fscanf(fp, "%s", answer);
		exists++;
	}
	if (exists == 0) printf("no valid words found\n");
	fclose(fp);
	*/
	return 1;
}
