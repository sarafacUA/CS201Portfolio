#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#include "boggle.h"

int main (void) {
	srand(time(0));
	Dictionary * dict = NULL;
	dict = setDictionary(); // removes proper nouns and words with '
	Node * head = NULL;
	int length = 2; // minimum length
	int height = 2; // minimum height
	int playerCount = 2; // minimum player count
	int playerScore[12]; // player scores and win counts for up to 6 players
	float timer = 1;
	title();
	int playing = 1;
	// multiplayer menu registers
	char multiplayerChoice;
	int multiplayerMenu = 1;
	int multiplayer = 1;
	int multiplayerSetup = 1;
	char buffer[1000]; // buffer for user inputs
	while (playing) { // Main Menu while loop
		int menuInput = 0;
		printf("MAIN MENU\n\n");
		printf("1: SINGLEPLAYER MODE (1)\n");
		printf("2: MULTIPLAYER MODE (2-6)\n");
		printf("3: BOGGLE SOLVER\n");
		printf("4: Quit\n\n");
		scanf("%999s", buffer);
		printf("\n");
		if (strlen(buffer) == 1) menuInput = buffer[0];
		switch (menuInput) {
			case '1': // single player mode
				length = sanitizedInt(2, 0, buffer, "Please Enter Length of Boggle Board (length > 1): ");
				height = sanitizedInt(2, 0, buffer, "Please Enter Height of Boggle Board (height > 1): ");
				timer = sanitizedInt(1, 0, buffer, "Please Enter Timer for Play (timer > 0): ");
				printf("\nSetting Up Board. Loading\n");
				head = boggleGraph(length, height);
				depthFirstSearch(dict, head);
				displayBoard(head, length, height);
				countDown(1, "Count Down Starts in: 3");
				countDown(1, "2");
				countDown(1, "1");
				countDown(1, "GO");
				countDown(timer, "Time's Up!");
				playerScore[0] = guessValid(buffer);
				printf("You got %d points!\n", playerScore[0]);
				freeGraph(head);
				displayAnswers();
				freeAnswers();
				break;
			case '2': // multiplayer mode
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
					depthFirstSearch(dict, head);
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
					displayAnswers();
					freeAnswers();
					while (multiplayerMenu) {
						printf("1: Play again with same players, same board settings\n");
						printf("2: Play again with same players, new boad settings\n");
						printf("3: Return to main menu\n\n");
						scanf("%s", buffer);
						printf("\n\n");
						if (strlen(buffer) == 1) multiplayerChoice = buffer[0];
						switch (multiplayerChoice) {
							case '1': // play again, same settings
								multiplayerSetup = 0;
								multiplayerMenu = 0;
								break;
							case '2': // play again, new settings
								multiplayerSetup = 1;
								multiplayerMenu = 0;
								break;
							case '3': // return to main menu
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
			case '3': // boggle solver
				length = sanitizedInt(2, 0, buffer, "Please Enter Length of Boggle Board (length > 1): ");
				height = sanitizedInt(2, 0, buffer, "Please Enter Height of Boggle Board (height > 1): ");
				head = boggleGraph(length, height);
				Node * userInput = head; // look through and set each node in graph to user define letter
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
				time_t start_t, end_t; // find time of board solve specifically for this mode
				time(&start_t);
				depthFirstSearch(dict, head);
				time(&end_t);
				int time = difftime(end_t, start_t);
				printf("Graph algortithm took %d seconds to solve!\n", time);
				displayAnswers();
				freeAnswers();
				freeGraph(head);
				break;
			case '4': // quit
				printf("Thanks for playing!\n");
				--playing;
				break;
			default:
				printf("Invalid Input\n");
		}
		printf("\n");
	}
	return 1;
}
