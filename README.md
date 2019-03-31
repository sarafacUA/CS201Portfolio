# CS201Portfolio 

This project is a demonstration of graph based search functionality, Depth First Search, 
through the lens of Boggle. The user is able to play the game boggle with multiple modes of
play, allowing the user to input an arbitrary board size (the minimum board size that the
user may create is a 2x2 boggle board). The following describes the main functionality of 
the program.

## Dictionary File

The dictionary file used with this program is the Collins Scrabble 2015 Words only 
text file. It can be found with the link provided.

[Link to the website in which the user can download the dictionary text file]https://boardgames.stackexchange.com/questions/38366/latest-collins-scrabble-words-list-in-text-file

Be sure to download the words only text file that is provided on the page. Once downloaded, 
rename the text file 'BoggleDictionary', and put it in the program folder.

## User Interface

Upon execution of the program, a title appears for the game followed by a main menu 
interface allowing the user to input a single character as indicated by the number beside 
the option. Valid inputs to proceed are '1', '2', '3', and '4'. Depending on the option 
selected, the program will prompt the user to input the length of the board selected, the 
height of the board selected, the time in which the user is allowed to write answers down, 
the number of players that will compete against each other, and even the letters to make up
the boggle board. Upon completion of the timer in single player and multiplayer mode, the 
program will ask users in input which words they found in the graph presented, giving them
points based on if the word matches a valid word found by the program. The player will enter
'.' in order for the game to end or for the next player to type in their answers. For the 
multiplayer game specifically, the program offers 3 options once the game is complete: Play 
the game again with the same players and the same board settings, play the game again with 
different board settings, or return to the main menu. Valid inputs to proceed here are '1', 
'2', and '3', akin to the main menu interface. Upon completion of singleplayers, 
multiplayer, and the boggle solver, the program will return to the main menu. Inputing the 
option for exit will quit out of the program.

## Boggle Board Graph Creation

The graph is created first by initializing a doubly linked list (DLL) with the total length
equal to the length and height dimensions asked by the program and randomized characters. 
Then, the graph runs through the DLL and fills in necessary pointers to other parts of the 
graph in the form of cardinal directions. In this way, each Node of the graph is constructed
so that it simulates how a player can search through a boggle board with direct pointers to
any next Node. Using the graph as a DLL, the program can print out the board for the user 
to see and utilize by going through the list and separating itself visiblity using the 
dimensions the user gives.

## Depth First Search Creation

The Depth first search process uses the cardinal direction pointers made for the boggle 
board graph in order to recursively look through the board at each node in the graph, 
checking to see how far deep into the graph the user can go until it reaches a point where 
the string found no longer can match to any legal dictionary word or that there are no 
other legal Nodes that can be pointed, whether that be because they have all been visited 
to, or that the pointers are NULL. The depth first search process keeps track of the 
current depth string by using a stack of Nodes. The search will check each available option 
until there is no more valid options, in which it pops the last Node put onto the stack, 
going back in the search to check if previous unchecked Node that were available to select 
on the list can find valid nodes to push onto the stack. When a string is found that is at 
least 3 characters long, the program will check to see if the string matches a word found in 
the provided user dictionary -- adding it the list of answers so long that it is not a 
duplicate answer.

