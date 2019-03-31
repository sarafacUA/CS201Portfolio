#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

/*
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif
*/


/* CS201 Portfolio Initial description and notes
 * Alex Rafac
 * 11740097
 *
 * The project I have chosen is to create the game of boggle using the graph, so that the user
 * can ask for an unspecified boggle game size, and the game will search and find valid words
 * that the user can guess within a certain period of time.
 */

// struct for individual nodes on the graph to go through
typedef struct node {
	// main contents of each node; what letter it contains and if it has been visited
	char letter;
	int visited;
	// next and prev are used to initialize and use the nodes as a doubly linked list
	struct node * next;
	struct node * prev;
	// cardinal directions to connect to different nodes as a graph; utilized in DFS
	struct node * nw;
	struct node * n;
	struct node * ne;
	struct node * w;
	struct node * e;
	struct node * sw;
	struct node * s;
	struct node * se;
} Node;

// struct to create a stack to keep tracks of progress through DFS
typedef struct stack {
	Node * nodePtr;
	struct stack * next;
} Stack;
Stack * top;

typedef struct answers {
	char * word;
	int value;
	int visited;
	struct answers * next;
} Answers;
Answers * list;

void addAnswerWord(char * answer) {
	int addWord = 1;
	for (Answers * temp = list; temp != NULL; temp = temp -> next) {
		int longerSize = (strlen(answer) > strlen(temp -> word)) ? strlen(answer) : strlen(temp -> word);
		if (strncmp(answer, temp -> word, longerSize) == 0) addWord = 0;
	}
	if (addWord) {
		Answers * newAnswer = NULL;
		newAnswer = (Answers *)malloc(sizeof(Answers));
		if (newAnswer == NULL) printf("Error message: allocation error during add");
		printf("adding word, %s\n", answer);
		newAnswer -> word = (char *)malloc(strlen(answer) + 1);
		newAnswer -> word = strncpy(newAnswer -> word, answer, strlen(answer) + 1);
		printf("word added is stored as: %s\n", newAnswer -> word);
		if (newAnswer -> word == NULL) printf("Error message: allocation error with string");
		switch (strlen(answer)) {
			case 3:
			case 4:
				newAnswer -> value = 1;
				break;
			case 5: 
				newAnswer -> value = 2;
				break;
			case 6:
				newAnswer -> value = 3;
				break;
			case 7:
				newAnswer -> value = 5;
				break;
			default:
				newAnswer -> value = 11;
		}
		newAnswer -> visited = 0;
		newAnswer -> next = list;
		list = newAnswer;
	}
}

void freeAnswers() {
	Answers * temp;
	if (list == NULL) printf("No valid words were found.\n");
	while (list != NULL) {
		temp = list;
		list = list -> next;
		free(temp -> word);
		free(temp);
	}
}

/* Function: addNode
 * Input: Node pointer to head of doubly linked list
 * Output: New Node pointer to head of doubly linked list
 * Description: this function allocates space and adds in the necessary parts of the
 * struct in order for Node to work as a doubly linked list as well as give the
 * Node a random letter of the alphabet to use for the boggle graph.
 */
Node * addNode(Node * head) {
	Node * newNode = NULL;
	newNode = (Node *)malloc(sizeof(Node));
	if (newNode == NULL) printf("Error message: allocation error during add");
	newNode -> letter = 'A' + rand() % 26;
	newNode -> next = head;
	head -> prev = newNode;
	head = newNode;
	return head;
}

/* Function: push
 * Input: Node pointer to desired letter to add to the stack
 * Output: None
 * Description: The function adds the current Node to the stack in order for the
 * DFS to keep track of how deep the search goes.
 */
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

/* Function: pop
 * Input: None
 * Output: None
 * Description: The function removes the most recent Node pointer added to 
 * the stack in order for the DFS to keep track of how deep the search goes.
 */
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

/* Function: display
 * Input: None
 * Output: None
 * Description: Display the current letter contents of the stack, used 
 * for debugging purposes.
 */
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

/* Function: getStr
 * Input: Pointer to string in which the stack values will be returned to
 * Output: integer value of the string size
 * Description: This function both outputs the size of the string stack as well as
 * translating the stack output back into a valid string to compare against.
 */
int getStr(char * stackString) {
	int count = 0;
	int size = 0;
	Stack * temp;
	if (top == NULL) printf("Stack underflow\n");
	for (temp = top; temp != NULL; temp = temp -> next) count++;
	size = count;
	stackString[count] = '\0';
	for (temp = top; temp != NULL; temp = temp -> next) stackString[--count] = temp -> nodePtr -> letter;
	return size;
}

/* Function: validPath
 * Input: The letter char to check against dictionary, the int depth of current DFS, and 
 * the int lineCount to jump the dictionary search to the appropriate location before 
 * searching through
 * Output: The firstValid int found while searching the dictionary file
 * Description: This function takes searches for the current letter given at the depth of 
 * that the DFS algorithm is currently at as well as starting from the previous found valid
 * word that the string thus far contains. The function returns the first instance in which
 * a word is found in the dictionary file where the contents of the DFS stack matches the
 * beginning of a word. If no word can be found in the dictionary file with the given 
 * inputs, the output returns 0, indicating a miss in the dictionary.
 */
int validPath(char letter, int depth, int lineCount) {
	char lineRead[100];
	int firstValid = 0;;
	FILE * ptrFile = NULL;
	ptrFile = fopen("BoggleDictionary.txt", "r");
	if (!ptrFile) printf("error: file not found");
	fscanf(ptrFile, "%s", lineRead);
	while (lineRead[0] != 'A') fscanf(ptrFile, "%s", lineRead);
	for (int i = 0; i < lineCount; i++) fscanf(ptrFile, "%s", lineRead);
	char previousDepth = '.';
       	if (depth - 1 > - 1) previousDepth = lineRead[depth - 1];
	while(!feof(ptrFile)) {
		if (previousDepth != lineRead[depth - 1] && previousDepth != '.') break;
		if (lineRead[depth] == letter) if (firstValid == 0) firstValid = lineCount;
		fscanf(ptrFile, "%s", lineRead);
		lineCount++;
	}
	fclose(ptrFile);
	return firstValid;
}

/* Function: validWord
 * Input: the String stackString test in the DFS and the int testFrom point in the
 * dictionary file
 * Output: None
 * Description: This function checks through the dictionary, utilizing the testFrom
 * int given to start at first valid instance of the string searched in the DFS in
 * order see if that string matches a word in its entirety in the dictionary file.
 */
void validWord(char * stackString, int testFrom) {
	char lineRead[100];
	FILE * ptrFile = NULL;
	if (top == NULL) printf("Stack underflow\n");
	ptrFile = fopen("BoggleDictionary.txt", "r");
	if (!ptrFile) printf("Error: file not found");
	fscanf(ptrFile, "%s", lineRead);
	while (lineRead[0] != 'A') fscanf(ptrFile, "%s", lineRead);
	for (int i = 0; i < testFrom; i++) fscanf(ptrFile, "%s", lineRead);
	while(!feof(ptrFile)) {
		int valid = 1;
		int largerSize = (strlen(stackString) > strlen(lineRead) ? strlen(stackString) : strlen(lineRead));
		valid = strncmp(stackString, lineRead, largerSize);
		if (valid == 0) addAnswerWord(lineRead);
		if (valid < 0) break;
		fscanf(ptrFile, "%s", lineRead);	
	}
	fclose(ptrFile);
}

/* Function: checkNodes
 * Input: the String stackString to use in dictionary file checks and the int startCheck
 * to jump to the first occurance of that string for use in dictionary file checks
 * Output: None
 * Description: This function goes looks at an individual node in the graph, checking
 * to see if the node will has the potential to become a word or is a word itself from
 * the stack. The function than recursively calls itself as it checks each cardinal 
 * direction node found in the Node struct. The function returns by removing the node
 * found once all states have been checked, thus creating a full depth first search.
 */
void checkNodes(char * stackString, int startCheck) {
	int stackSize = getStr(stackString);
	int testFrom = validPath(top -> nodePtr -> letter, stackSize - 1, startCheck);
	if (stackSize > 2 && testFrom > 0) validWord(stackString, testFrom);
	top -> nodePtr -> visited = 1;
	if (testFrom != 0) {
		if (top -> nodePtr -> nw != NULL && top -> nodePtr -> nw -> visited == 0) {
			push(top -> nodePtr -> nw);
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> n != NULL && top -> nodePtr -> n -> visited == 0) {
			push(top -> nodePtr -> n);
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> ne != NULL && top -> nodePtr -> ne -> visited == 0) {
			push(top -> nodePtr -> ne);
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> w != NULL && top -> nodePtr -> w -> visited == 0) {
			push(top -> nodePtr -> w);
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> e != NULL && top -> nodePtr -> e -> visited == 0) {
			push(top -> nodePtr -> e);
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> sw != NULL && top -> nodePtr -> sw -> visited == 0) {
			push(top -> nodePtr -> sw);
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> s != NULL && top -> nodePtr -> s -> visited == 0) {
			push(top -> nodePtr -> s);
			checkNodes(stackString, testFrom);
		}
		if (top -> nodePtr -> se != NULL && top -> nodePtr -> se -> visited == 0) {
			push(top -> nodePtr -> se);
			checkNodes(stackString, testFrom);
		}
	}
	top -> nodePtr -> visited = 0;
	pop();
}

/* Function: DFS
 * Input: Node pointer to the head of the graph (top left corner)
 * Output: None
 * Description: This function initializes the necessary components needed to undergo
 * the recursive algorithm needed for a depth first search for each node found in 
 * the doubly linked list.
 */
void DFS(Node * head) {
	for (Node * word = head; word != NULL; word = word -> next) {
		char stackString[100];
		int testFrom = 0;
		push(word);
		checkNodes(stackString, testFrom);
	}
}

/* Function: boggleGraph
 * Input: the dimensions of the desired graph, int length and int height
 * Output: Node pointer to the head of the finalized graph
 * Description: This function calls for the creations of a doubly linked list, then
 * transforms that list into a fully fleshed out graph with pointers to each cardinal 
 * direction in relation to the node itself as well as intializing the visited int
 * necessary for DFS checking and boggle. All of this is done with the desired user
 * dimensions for the boggle board they want to create.
 */
Node * boggleGraph(int length, int height) {
	Node * head = NULL;
	head = (Node *)malloc(sizeof(Node));
	if (head == NULL) printf("Error message: allocation failure of head\n");
	head -> letter = 'A' + rand() % 26;
	head -> next = NULL;
	for (int i = 0; i < length * height - 1; i++) {
		head = addNode(head);
		if (head == NULL) printf("Error message: allocation failure during push\n");
	}
	Node * index = head;
	for (int y = 0; y < height; y++) {
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
			} else if (x == 0 && y == height - 1) { // bottom left corner
				index -> w = NULL;
				index -> e = ptr -> next;
				for (int i = 0; i < length; i++) ptr = ptr -> prev;
				index -> nw = NULL;
				index -> n = ptr;
				index -> ne = ptr -> next;
				index -> sw = NULL;
				index -> s = NULL;
				index -> se = NULL;	
			} else if (x == length - 1 && y == height - 1) { // bottom right corner
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
			} else if (y == height - 1) { // last row
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
			index = index -> next;
		}
	}
	return head;
}

void displayAnswers() {
	printf("\nThe answers for the follow boggle board are:\n");
	for (Answers * temp = list; temp != NULL; temp = temp -> next) printf("Worth %2d point(s) is: %s\n", temp -> value, temp -> word);
	printf("\n");
}


/* Function: title
 * Input: None
 * Output: None
 * Description: This is a pretty title for the game with ASCII art :)
 */
void title() {
	printf("\n");
	printf(" /$$$$$$$   /$$$$$$   /$$$$$$   /$$$$$$  /$$       /$$$$$$$$\n");
	printf("| $$__  $$ /$$__  $$ /$$__  $$ /$$__  $$| $$      | $$_____/\n");
	printf("| $$  \\ $$| $$  \\ $$| $$  \\__/| $$  \\__/| $$      | $$\n");
	printf("| $$$$$$$ | $$  | $$| $$ /$$$$| $$ /$$$$| $$      | $$$$$\n");
	printf("| $$__  $$| $$  | $$| $$|_  $$| $$|_  $$| $$      | $$__/\n");
	printf("| $$  \\ $$| $$  | $$| $$  \\ $$| $$  \\ $$| $$      | $$\n");
	printf("| $$$$$$$/|  $$$$$$/|  $$$$$$/|  $$$$$$/| $$$$$$$$| $$$$$$$$\n");
	printf("|_______/  \\______/  \\______/  \\______/ |________/|________/\n");
	printf("\n");
}

/* Function: displayBoard
 * Input: Node pointer to the head, and the integer dimensions, int length and int height
 * Output: None
 * Description: This graph takes the graph created as well as the dimensions that user
 * input for the graph in order to output a graph for the user to see with ASCII art
 */
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

/* Function: sanitizedInt
 * Input: Int validRange for lower bound, int validLimt for the upper bound, String buffer
 * to read the inputs the user enters, and String message for the unique message to ask
 * Output: Returns the sanitized int for valid user ints
 * Description: The function ensures that the integer that the user needs to input is correct
 * in order to avoid human errors in the program's functionality. This function also asks for
 * the range of integer values for as user values require some necessary valid range.
 */
int sanitizedInt(int validRange, int validLimit, char * buffer, char * message) {
	printf("%s", message);
	scanf("%s", buffer);
	int validInt = 1;
	while (validInt) {
		for (int i = 0; i < strlen(buffer); i++) if (isdigit(buffer[i]) == 0) validInt = 0;
		if (!validInt) {
			printf("\nInvalid Input\n\n%s", message);
			scanf("%s", buffer);
			validInt = 1;
		} else if (validRange > atoi(buffer)) {
			printf("\nInvalid Input\n\n%s", message);
			scanf("%s", buffer);
			validInt = 1;
		} else if (validLimit > 0 && validLimit < atoi(buffer)) {
			printf("\nInvalid Input\n\n%s", message);
			scanf("%s", buffer);
			validInt = 1;
		} else validInt = 0;
	}
	return atoi(buffer);
}

/* Function: sanitizedChar
 * Input: String buffer for the function to scan user inputs
 * Output: Returns the sanitized char for valid user characters
 * Description: This function reads the user function until a valid character is entered.
 */
char sanitizedChar(char * buffer) {
	scanf("%s", buffer);
	int validChar = 1;
	while(validChar) {
		if (!(strlen(buffer) == 1 && isalpha(buffer[0]))) {
			printf("\nInvalid Input\n\nPlease Enter valid letter: ");
			scanf("%s", buffer);
		} else --validChar;
	}
	return buffer[0];
}

/* Function: countDown
 * Input: Int timer for the ammount of time wait and String message for count down message
 * Output: None
 * Description: This function waits a certain amount of time as given by the integer value
 * the user input in order to create a timer, displaying a message once the timer ends.
 */
void countDown(int timer, char * message) {
	time_t start_t, end_t;
	time(&start_t);
	time(&end_t);
	while(difftime(end_t, start_t) < timer) time(&end_t);
	printf("%s\n", message);

}

/* Function: guessValid
 * Input: String of the buffer for user inputs to be scanned
 * Output: Int points totalled from the score function
 * Description: This function allows for the user to enter the words they found or think
 * may be valid, waiting for the '.' character to be input once the user is done.
 */
int guessValid(char * buffer) {
	printf("Please input '.' once done entering words\n");
	int scoring = 1;
	int points = 0;
	while (scoring) {
		scanf("%s", buffer);
		int i = 0;
		while(buffer[i]) {
			buffer[i] = toupper(buffer[i]);
			i++;
		}
		//points += score(buffer);
		for (Answers * temp = list; temp != NULL; temp = temp -> next) {
			int longerSize = (strlen(temp -> word) > strlen(buffer) ? strlen(temp -> word) : strlen(buffer));
			if (strncmp(temp -> word, buffer, longerSize) == 0 && temp -> visited == 0) {
				printf("CORRECT!!\n");
				points += temp -> value;//score(buffer);
				temp -> visited = 1;
				printf("current points: %d\n", points);
			}
		}
		if (strcmp(buffer, ".") == 0) --scoring;
	}
	for (Answers * temp = list; temp != NULL; temp = temp -> next) temp -> visited = 0;
	return points;
}

/* Function: freeGraph
 * Input: Node pointer to the head of the graph
 * Output: None
 * Description: This function clears the boggle graph created and user from memory.
 */
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
				//time_t startTime_t, endTime_t;
				//time(&startTime_t);
				DFS(head);
				//time(&endTime_t);
				//printf("\nDFS Algorithm took %lf seconds!\n\n", difftime(endTime_t, startTime_t));
				displayBoard(head, length, height);
				countDown(1, "Count Down Starts in: 3");
				countDown(1, "2");
				countDown(1, "1");
				countDown(1, "GO");
				countDown(timer, "Time's Up!");
				displayAnswers();
				playerScore[0] = guessValid(buffer);
				printf("You got %d points!\n", playerScore[0]);
				freeGraph(head);
				freeAnswers();
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
					countDown(1, "Count Down Starts in: 3");
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
					freeAnswers();
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
						letter = sanitizedChar(buffer);
						letter = toupper(letter);
						userInput -> letter = letter;
						userInput = userInput -> next;
					}
				}
				displayBoard(head, length, height);
				printf("Solving board above. Please Be Patient\n");
				time_t start_t, end_t;
				time(&start_t);
				DFS(head);
				time(&end_t);
				int time = difftime(end_t, start_t);
				printf("Graph algortithm took %d seconds to solve!\n", time);
				displayAnswers();
				freeAnswers();
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

	/* TO DO LIST:
	 // FALL UNDER SAME TO DO ISH
	 * 1) Sanitize inputs for amounts larger than buffer
	 * 2) move functions out of main and create separate function file, header file
	 * 3) Test with different dictionary files?
	 * 4) create makefile
	 * 4) add additional comments
	 * 5) create video "how to" and include link to it in readme
	 */

	return 1;
}
