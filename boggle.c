#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "boggle.h"

/* Function: setDictionary
 * Input: None
 * Output: Pointer to head of dictionary linked list
 * Description: This function reads in the file that the user puts into the
 * project folder, then puts all the words found in that dictionary text file
 * into nodes on a list. This improves runtime and only necessitates the text
 * file be read only once
 */
Dictionary * setDictionary() {
	Dictionary * head = NULL;
	char initialWord[100];
	FILE * fp;
	fp = fopen("BoggleDictionary.txt", "r");
	fscanf(fp, "%s", initialWord);
	if (isupper(initialWord[0]) == 0 && strlen(initialWord) > 2) {
		int a = 0;
		int fails = 0;
		while(initialWord[a]) {
			if (initialWord[a] == '\'') {
				fails = 1;
				break;
			}
			initialWord[a] = toupper(initialWord[a]);
			a++;
		}
		if (fails == 0) {
			head = (Dictionary *)malloc(sizeof(Dictionary));
			if (head == NULL) printf("Error message: allocation error during dictionary add");
			head -> word = (char *)malloc(strlen(initialWord) + 1);
			strncpy(head -> word, initialWord, strlen(initialWord) + 1);
			head -> next = NULL;
		}
	}
	while(!feof(fp)) {
		fscanf(fp, "%s", initialWord);
		if (isupper(initialWord[0]) == 0 && strlen(initialWord) > 2) {
			int i = 0;
			int fails = 0;
			while (initialWord[i]) {
				if (initialWord[i] == '\'') {
					fails = 1;
					break;
				}
				initialWord[i] = toupper(initialWord[i]);
				i++;
			}
			if (fails == 0) {
				Dictionary * temp = NULL;
				temp = (Dictionary *)malloc(sizeof(Dictionary));
				if (temp == NULL) printf("Error message: allocation error during dictionary add");
				temp -> word = (char *)malloc(strlen(initialWord) + 1);
				strncpy(temp -> word, initialWord, strlen(initialWord) + 1);
				temp -> next = head;
				head = temp;
			}
		}
	}
	fclose(fp);
	return head;
}

/* Function: addAnswerWord
 * Input: String answer which has been proven valid
 * Output: None
 * Description: This function takes a match found while looking through the
 * boggle graph, and adds it to this linked list, so that it can avoid
 * duplicates, calculate the value of that chosen word to use for scoring, and
 * watches out for multiple of the same user responses.
 */
void addAnswers(char * answer) {
	int addWord = 1;
	for (Answers * temp = list; temp != NULL; temp = temp -> next) {
		int longerSize = (strlen(answer) > strlen(temp -> word)) ? strlen(answer) : strlen(temp -> word);
		if (strncmp(answer, temp -> word, longerSize) == 0) addWord = 0;
	}
	if (addWord) {
		Answers * newAnswer = NULL;
		newAnswer = (Answers *)malloc(sizeof(Answers));
		if (newAnswer == NULL) printf("Error message: allocation error during add");
		newAnswer -> word = (char *)malloc(strlen(answer) + 1);
		newAnswer -> word = strncpy(newAnswer -> word, answer, strlen(answer) + 1);
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

/* Function: displayAnswers
 * Input: None
 * Output: None
 * Description: This function displays the list of answers found and stored in
 * the Answers struct, and is used for the boggle solver mode of the program.
 */
void displayAnswers() {
	printf("\nThe answer(s) for the following boggle board are:\n");
	int i = 0;
	for (Answers * temp = list; temp != NULL; temp = temp -> next) {
		printf("Worth %2d point(s) is: %s\n", temp -> value, temp -> word);
		i++;
	}
	printf("For a total of %d answer(s)!\n", i);
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
		scanf("%999s", buffer);
		int i = 0;
		while(buffer[i]) {
			buffer[i] = toupper(buffer[i]);
			i++;
		}
		for (Answers * temp = list; temp != NULL; temp = temp -> next) {
			int longerSize = (strlen(temp -> word) > strlen(buffer) ? strlen(temp -> word) : strlen(buffer));
			if (strncmp(temp -> word, buffer, longerSize) == 0 && temp -> visited == 0) {
				printf("correct!\n");
				points += temp -> value;
				temp -> visited = 1;
			}
		}
		if (strcmp(buffer, ".") == 0) --scoring;
	}
	for (Answers * temp = list; temp != NULL; temp = temp -> next) temp -> visited = 0;
	return points;
}

/* Function: freeAnswers
 * Input: None
 * Output: None
 * Description: This function frees the memory that was used to create the
 * answer list.
 */
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

/* Function: push
 * Input: Node pointer to desired letter to add to the stack
 * Output: None
 * Description: The function adds the current Node to the stack in order for the
 * DFS to keep track of how deep the search goes.
 */
void push(Node * word) {
	Stack * newStack = NULL;
	newStack = (Stack *)malloc(sizeof(Stack));
	if (newStack == NULL) {
		printf("stack overflow\n");
	}
	newStack -> nodePtr = word;
	newStack -> next = top;
	top = newStack;
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
	if (top == NULL) printf("stack underflow\n");
	temp = top;
	while (temp != NULL) {
		printf("%c", temp -> nodePtr -> letter);
		temp = temp -> next;
	}
	printf("\n");
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

/* Function: getStr
 * Input: Pointer to string in which the stack values will be returned to
 * Output: integer value of the string size
 * Description: This function both outputs the size of the string stack as well as
 * translating the stack output back into a valid string to compare against.
 */
void getStr(char * stackString) {
	Stack * temp;
	int count = 0;
	if (top == NULL) printf("Stack underflow\n");
	for (temp = top; temp != NULL; temp = temp -> next) count++;
	stackString[count] = '\0';
	for (temp = top; temp != NULL; temp = temp -> next) stackString[--count] = temp -> nodePtr -> letter;
}

/* Function: checkDictionary
 * Input: Pointer to the head of the dictionary struct, String of the search
 * values at this point in time, Pointer to the top of the stack
 * Output: None
 * Description: This function checks to see if the current concatenated values
 * in the depth first search is the start of a word, or is a word itself.
 */
int checkDictionary(Dictionary * dict, char * stackString) {
	for (Dictionary * temp = dict; temp != NULL; temp = temp -> next) {
		if (strlen(stackString) > 2) {
			int largerWord = (strlen(temp -> word) > strlen(stackString) ? strlen(temp -> word) : strlen(stackString));
			if (strncmp(temp -> word, stackString, largerWord) == 0) {
				addAnswers(temp -> word);
				break;
			}
		}
	}
	for (Dictionary * temp = dict; temp != NULL; temp = temp -> next) if (strncmp(temp -> word, stackString, strlen(stackString)) == 0) return 1;
	return 0;
}

/* Function: checkNodes
 * Input: The pointer to the head of the dictionary struct, the string found by
 * turning the stack around backwards
 * Description: This function goes looks at an individual node in the graph, checking
 * to see if the node will has the potential to become a word or is a word itself from
 * the stack. The function than recursively calls itself as it checks each cardinal
 * direction node found in the Node struct. The function returns by removing the node
 * found once all states have been checked, thus creating a full depth first search.
 */
void checkNodes(Dictionary * dict, char * stackString) {
	getStr(stackString);
	int testFrom = checkDictionary(dict, stackString);
	top -> nodePtr -> visited = 1;
	if (testFrom) {
		if (top -> nodePtr -> nw != NULL && top -> nodePtr -> nw -> visited == 0) {
			push(top -> nodePtr -> nw);
			checkNodes(dict, stackString);
		}
		if (top -> nodePtr -> n != NULL && top -> nodePtr -> n -> visited == 0) {
			push(top -> nodePtr -> n);
			checkNodes(dict, stackString);
		}
		if (top -> nodePtr -> ne != NULL && top -> nodePtr -> ne -> visited == 0) {
			push(top -> nodePtr -> ne);
			checkNodes(dict, stackString);
		}
		if (top -> nodePtr -> w != NULL && top -> nodePtr -> w -> visited == 0) {
			push(top -> nodePtr -> w);
			checkNodes(dict, stackString);
		}
		if (top -> nodePtr -> e != NULL && top -> nodePtr -> e -> visited == 0) {
			push(top -> nodePtr -> e);
			checkNodes(dict, stackString);
		}
		if (top -> nodePtr -> sw != NULL && top -> nodePtr -> sw -> visited == 0) {
			push(top -> nodePtr -> sw);
			checkNodes(dict, stackString);
		}
		if (top -> nodePtr -> s != NULL && top -> nodePtr -> s -> visited == 0) {
			push(top -> nodePtr -> s);
			checkNodes(dict, stackString);
		}
		if (top -> nodePtr -> se != NULL && top -> nodePtr -> se -> visited == 0) {
			push(top -> nodePtr -> se);
			checkNodes(dict, stackString);
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
void depthFirstSearch(Dictionary * dict, Node * head) {
	for (Node * word = head; word != NULL; word = word -> next) {
		char stackString[100];
		push(word);
		checkNodes(dict, stackString/*, top*//*, list*/);
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
	scanf("%999s", buffer);
	int validInt = 1;
	while (validInt) {
		for (int i = 0; i < strlen(buffer); i++) if (isdigit(buffer[i]) == 0) validInt = 0;
		if (!validInt) {
			printf("\nInvalid Input\n\n%s", message);
			scanf("%999s", buffer);
			validInt = 1;
		} else if (validRange > atoi(buffer)) {
			printf("\nInvalid Input\n\n%s", message);
			scanf("%999s", buffer);
			validInt = 1;
		} else if (validLimit > 0 && validLimit < atoi(buffer)) {
			printf("\nInvalid Input\n\n%s", message);
			scanf("%999s", buffer);
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
	scanf("%999s", buffer);
	int validChar = 1;
	while(validChar) {
		if (!(strlen(buffer) == 1 && isalpha(buffer[0]))) {
			printf("\nInvalid Input\n\nPlease Enter valid letter: ");
			scanf("%999s", buffer);
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
