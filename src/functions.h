/*
Contains the function prototypes of the functions defined in functions.c
*/

int calculatePoints(int);
void clearBuffer();
int chooseMode(int *, char *, int, int*, int*, int *, int *);
int createNode(struct node *, struct node *, int, int, int, char [], int);
int getData(int, int, int, char [], int);
int randomNum(int, int, int, int, int, int *);
void results(struct node *, int);
void toLower(char *, int);