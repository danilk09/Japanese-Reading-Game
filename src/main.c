#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "store.h"

void clearBuffer();
void chooseMode(int *, char *, int);
int randomNum(int, int);
void toLower(char *, int);

int main(int argc, char* argv[])
{
    int i, random, length, mode, curr_score, total_score = 0, total_questions = 10, boolean = 0;
    int start_time, end_time, total_time;
    char user_input[10];
    char question[50];
    char correct_answer[50];
    char user_answer[50];
    node *head = NULL;
    node *tail = NULL;

    printf("Welcome to the Japanese Reading Game!\n"
    "Type and enter the romaji version of the Japanese word as quickly as possible\n"
    "The quicker you type, the more points you gain!\n"
    "Please choose one of the game options below to start the game and determine what\n"
    "type of questions will be asked\n"
    "Hirigana | Katakana | Both\n");

    while(boolean == 0)
    {
        fgets(user_input, 10, stdin);
        length = strlen(user_input) - 1;
        toLower(user_input, length);
        chooseMode(&mode, user_input, length);
        clearBuffer();
    }

    printf("Game starts in...\n");
    for(i = 3; i > 0; i--)
    {
        sleep(1);
        printf("%d", i);
    }
    printf("Start!\n"
    "_______________________\n")

    for(i = 0; i < total_questions; i++)
    {
        random = randomNum(100, 1); // Temporary placeholder for generating a random number

        question = TEMP; // Store the random question from the database in the char array
        correct_answer = TEMP; // Store the correct answer of the random question from the database

        start_time = time(NULL);

        printf("Question %d: %s\n", i + 1, question) // Prints a random question from the database

        fgets(user_answer, 50, stdin);

        end_time = time(NULL);

        total_time = difftime(end_time, start_time);

        if(strcmp(user_answer, correct_answer) == 0)
        {
            curr_score = calculatePoints(total_time);
            total_score += curr_score;
        }
        else
        {
            curr_score = 0;
        }


        createNode(head, tail, question, correct_answer, user_answer, curr_score);
    }

    results(head, total_score);
    
}

/*
Calculates the points the user achieved based on how much time the
user spent on answering the question
*/
int calculatePoints(int time)
{
    int score;
    score = (20 - (time/2));
    return score;
}

/*
Clears the buffer
*/
void clearBuffer()
{

}

/*
Selects the mode of the game depeneding on what the user
chose. This will specify what database to take information
from.
*/
void chooseMode(int *mode, char *user_input, int length)
{
    if(strncmp("hirigana", user_input, length))
    {
        mode = 0;
    }
    else if(strncmp("katakana", user_input, length))
    {
        mode = 1;
    }
    else if(strncmp("both", user_input, length))
    {
        mode = 2;
    }
    else
    {
        printf("Please select a valid option\n");
    }
}

/*
Creates a node that stores information about the current
question the user answered. Alwyas adds the node to the end 
of the list.
*/
void *createNode(node *head, node *tail, char question[], char correct_answer[], char user_answer[], int points[])
{
    node *iter = end;

    node *new_node = (node *)malloc(sizeof(node));
    new_node->quetsion = question;
    new_node->correct_answer = correct_answer;
    new_node->user_answer = user_answer;
    new_node->points = points;
    new_node->next = NULL;

    if(start == NULL && tail == NULL)
    {
        head = &new_node;
        tail = &new_node;
    }
    else
    {
        tail->next = &new_node;
    }
}

/*
Generates a random number. This number will be used to
select a random question from the selected database.
*/
int randomNum(int upper, int lower)
{
    int rand;

    return rand;
}

/*
Prints the results of each question, the correct answer
to the question, the user's answer to the question, how
many points the user gained for each question, as well as
their final score.
*/
void results(node *head, int final_score)
{

}

/*
Changes the given input into all lower case letters by
changing the characters ASCII value.
*/
void toLower(char *input, int size)
{
    int i;
    
    for(i = 0; i < size; i++)
    {
        if(input[i] > 40 && input[i] < 91)
        {
            input[i] += 32;
        }
    } 
}