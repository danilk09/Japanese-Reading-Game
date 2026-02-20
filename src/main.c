// Header Files
#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "functions.h"

/**
 * Main runs the code for executing the Japanese Reading Game.
 * It lets the user select the mode of the game. The game consists
 * of 10 questions about hirigana/katakana characters/words and asks
 * the users for the romaji version or the meaning of the word in
 * English. The faster they answer, the more points they gain.
 * 
 * Parameters: argc (int) : 
 *             argv (char *[]) : 
 * 
 * Returns: 0 : Main ran successfully
 *         -1 : Some error occured while main ran
 */
int main(int argc, char* argv[])
{
    // Variable Declarations
    int i, random, length;
    int database_type, mode, both_mode;
    int upper_hir, lower_hir, upper_kat, lower_kat;
    int start_time, end_time, total_time;
    int curr_score = 0, total_score = 0;
    int ret_val = 0, boolean = 0, total_questions = 10;

    char user_input[30];
    char question[50];
    char user_romaji[50];
    char correct_romaji[50];

    struct node *head = NULL;
    struct node *tail = NULL;

    setlocale(LC_ALL, "en_US.UTF-8");

    setbuf(stdout, NULL);
    setvbuf(stdout, NULL, _IONBF, 0);

    printf("\nWelcome to the Japanese Reading Game!\n"
        "=====================================\n"
        "You can either practice hirigana and katakana characters or reading hirigana and katakana words.\n"
        "Type and enter the romaji version of the Japanese character/word as quickly as possible!\n"
        "The quicker you type, the more points you gain! Each question is worth up to 20 points.\n"
        "Answering the question in 2 seconds or less will give you the full 20 points, and every second\n"
        "after that will decrease the points gained by 2 until you reach eitehr 1 point for a correct answer\n"
        "or 0 points for an incorrect answer.\n"
        "Please choose one of the game options below to start the game and determine what\n"
        "type of questions will be asked\n"
        "Hiragana Characters | Hiragana Words | Katakana Characters | Katakana Words | Both Characters | Both Words\n\n");

    // Ensures the user selected a valid mode
    while(boolean == 0)
    {
        fgets(user_input, 30, stdin);
        length = strlen(user_input) - 1;
        toLower(user_input, length);
        if (chooseMode(&mode, user_input, length, &upper_hir, &lower_hir, &upper_kat, &lower_kat) == -1)
        {
            printf("Please select a valid mode: \n");
        }
        else
        {
            boolean = 1;
        }
        //clearBuffer();
    }

    printf("   Start!\n"
           "============\n");

    // Loop for asking each question
    for(i = 0; i < total_questions && ret_val == 0; i++)
    {
        // Generates a random number depending on the mode selected
        // This number relates to the id of the database
        if (mode == 0 || mode == 1)
        {
            random = randomNum(mode, upper_hir, lower_hir, 0, 0, &both_mode); 
        }
        else if (mode == 2 || mode == 3)
        {
            random = randomNum(mode, 0, 0, upper_kat, lower_kat, &both_mode); 
        }
        else
        {
            random = randomNum(mode, upper_hir, lower_hir, upper_kat, lower_kat, &both_mode);
        }

        if (random == -1)
        {
            ret_val = -1;
        }

        // Obtaining information stored in the database depending on the id randomly generated
        database_type = 0;
        if (ret_val != 0 || getData(both_mode, random, database_type, question, 50) == -1)
        {
            printf("Error with obtaining the question from the database\n");
            ret_val = -1;
        }

        database_type = 1;
        if (ret_val != 0 || getData(both_mode, random, database_type, correct_romaji, 50) == -1)
        {
            printf("Error with obtaining the correct romaji from the database\n");
            ret_val = -1;
        }

        if (ret_val == 0)
        {
            printf("\nQuestion %d: %s\n", i + 1, question);
            fflush(stdout);

            // Finds out how much time the user spent answering the question
            start_time = time(NULL);
            fgets(user_romaji, 50, stdin);
            end_time = time(NULL);
            total_time = difftime(end_time, start_time);
            
            toLower(user_romaji, strlen(user_romaji));
            //clearBuffer();

            // Determines how many points the user gained
            if(strncmp(user_romaji, correct_romaji, 50) == 0)
            {
                curr_score = calculatePoints(total_time);
                total_score += curr_score;
            }
            else
            {
                curr_score = 0;
            }

            // Stores information about this specific question into a node associated with a linked list
            // Used for printing the results
            createNode(&head, &tail, question, correct_romaji, user_romaji, curr_score, 50);
        }
    }

    // Prints the results of the game
    results(head, total_score);
    return ret_val;
}