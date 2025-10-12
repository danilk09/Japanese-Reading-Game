/*
Contains the function prototypes of the functions defined in functions.c
*/

#include "node.h"

int calculatePoints(int);
void clearBuffer();
int chooseMode(int *, char *, int, int*, int*, int *, int *);
int createNode(struct node *, struct node *, char [], char [], char [], char [], char [], int, int);
int getData(int, int, int, char [], int);
int randomNum(int, int, int, int, int, int *);
void results(struct node *, int, int);
void toLower(char [], int);