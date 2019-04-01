#ifndef BOGGLE_H
#define BOGGLE_H

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

// struct to read in dictionary file for faster runtimes
typedef struct dictionary {
	char * word;
	struct dictionary * next;
} Dictionary;

// struct to create a stack to keep track of progress through DFS
typedef struct stack {
	Node * nodePtr;
	struct stack * next;
} Stack;
Stack * top;

// struct to store the answers found recursively through DFS
typedef struct answers {
	char * word;
	int value;
	int visited;
	struct answers * next;
} Answers;
Answers * list;

extern Dictionary * setDictionary();
extern void addAnswers(char *);
extern void displayAnswers();
extern int guessValid(char *);
extern void freeAnswers();
extern void push(Node *);
extern void pop();
extern void display();
extern Node * addNode(Node *);
extern void getStr(char *);
extern int checkDictionary(Dictionary *, char *, Stack *, Answers *);
extern void checkNodes(Dictionary *, char *);
extern void depthFirstSearch(Dictionary *, Node *);
extern Node * boggleGraph(int, int);
extern void displayBoard(Node *, int, int);
extern void freeGraph(Node *);
extern void title();
extern int sanitizedInt(int, int, char *, char *);
extern char sanitizedChar(char *);
extern void countDown(int, char *);

#endif
