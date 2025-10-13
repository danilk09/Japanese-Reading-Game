// Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>

#include "node.h"

/**
 * Calculates the points the user achieved based on how much 
 * time the user spent on answering the question.
 * 
 * Parameters: time (int) : The amount of time the user spent
 *                          answering the question.
 * 
 * Returns: score : The number of points the user gained
 */
int calculatePoints(int time)
{
    int score;
    score = (20 - (time/2));
    return score;
}

/**
 * Clears the buffer.
 * 
 * Parameters: None
 * 
 * Returns: Void
 */
void clearBuffer()
{
    char c;
    do
    {
        c = fgetc(stdin);
    } while (c != "\n" && c != EOF);
}

/**
 * Selects the mode of the game depeneding on what the user
 * chose. This will specify what database to take information
 * from as well as the range of numbers the randomNum function
 * should generate.
 * 
 * Parameters: mode (int *) : The mode of the game
 *             user_input (char *) : What the user typed
 *             length (int) : How many characters the user input is
 *             upper_hir (int *) : The max id number that should be
 *                                 achieved from the hirigana database
 *             lower_hir (int *) : The lowest id number that should be
 *                                 achieved from the hirigana database
 *             upper_kat (int *) : The max id number that should be
 *                                 achieved from the katakana database
 *             lower_kat (int *) : The lowest id number that should be
 *                                 achieved from the katakana database
 * 
 * Returns: 0 : The user successfully chose a mode
 *         -1 : The user typed an invalid mode
 */
int chooseMode(int *mode, char *user_input, int length, int *upper_hir, int *lower_hir, int *upper_kat, int *lower_kat)
{
    int ret_val = 0;

    if(strncmp("hirigana characters", user_input, length) == 0)
    {
        *mode = 0;
        *upper_hir = 104;
        *lower_hir = 1;
    }
    else if(strncmp("hirigana words", user_input, length) == 0)
    {
        *mode = 1;
        *upper_hir = 273;
        *lower_hir = 105;
    }
    else if(strncmp("katakana characters", user_input, length) == 0)
    {
        *mode = 2;
        *upper_kat = 0; // Placeholders for when I create katakana database
        *lower_kat = 0;
    }
    else if(strncmp("katakana words", user_input, length) == 0)
    {
        *mode = 3;
        *upper_kat = 0; // Placeholders for when I create katakana database
        *lower_kat = 0;
    }
    else if(strncmp("both characters", user_input, length) == 0)
    {
        *mode = 4;
        *upper_hir = 104; // Placeholders for when I create katakana database
        *upper_kat = 0;
        *lower_hir = 1;
        *lower_kat = 0;
    }    
    else if(strncmp("both words", user_input, length) == 0)
    {
        *mode = 5;
        *upper_hir = 273; // Placeholders for when I create katakana database
        *upper_kat = 0;
        *lower_hir = 105;
        *lower_kat = 0;
    }
    else
    {
        ret_val = -1;
    }

    return ret_val;
}

/**
 * Creates a node that stores information about the current question
 * the user answered. Alwyas adds the node to the end of the linked 
 * list.
 * 
 * Parameters: head (struct node **) : 
 *             tail (struct node **) :
 *             question (char []) :
 *             correct_romaji (char []) :
 *             correct_english (char []) :
 *             user_romaji (char []) :
 *             user_english (char []) :
 *             points (int) :
 *             size (int) :
 * 
 * Returns: 
 */
int createNode(struct node **head, struct node **tail, char question[], char correct_romaji[], char correct_english[], char user_romaji[], char user_english[], int points, int size)
{
    printf("Got here 0\n");
    int ret_val = 0;

    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    if (new_node == NULL)
    {
        ret_val = -1;
    }

    if (ret_val == 0)
    {
        strncpy(new_node->question, question, size);
        strncpy(new_node->correct_romaji, correct_romaji, size);
        strncpy(new_node->correct_english, correct_english, size);
        strncpy(new_node->user_romaji, user_romaji, size);
        strncpy(new_node->user_english, user_english, size);
        new_node->points = points;
        new_node->next = NULL;

        if(*head == NULL && *tail == NULL)
        {
            *head = new_node;
            *tail = new_node;
        }
        else
        {
            (*tail)->next = new_node;
            *tail = (*tail)->next;
        }
    }

    return ret_val;
}

/**
 * Obtains data from the specified database and column and provides
 * it to the function that called getData.
 * 
 * Parameters: both_mode (int) :
 *             random (int) :
 *             database_type (int) :
 *             info (char []) :
 *             size (int) :
 * 
 * Returns: 
 */
int getData(int both_mode, int random, int database_type, char info[], int size)
{
    int ret_val = 0;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char db_name[100];
    char *database_info;
    int exit;
    char *sql;

    if (both_mode == 0)
    {
        strcpy(db_name, "../../storage/output/hirigana.db");
        if (database_type == 0)
        {
            sql = "SELECT Hirigana from hirigana_mode WHERE id = ?";
        }
        else if (database_type == 1)
        {
            sql = "SELECT Romaji from hirigana_mode WHERE id = ?";
        }
        else if (database_type == 2)
        {
            sql = "SELECT English from hirigana_mode WHERE id = ?";
        }
        else
        {
            ret_val = -1;
        }
    }
    else
    {
        strcpy(db_name, "../../storage/output/katakana.db");
        if (database_type == 0)
        {
            sql = "SELECT Katakana from katakana_mode WHERE id = ?";
        }
        else if (database_type == 1)
        {
            sql = "SELECT Romaji from katakana_mode WHERE id = ?";
        }
        else if (database_type == 2)
        {
            sql = "SELECT English from katakana_mode WHERE id = ?";
        }
        else
        {
            ret_val = -1;
        }
    }

    exit = sqlite3_open(db_name, &db);
    if (exit)
    {
        printf("Got here: %s\n", sqlite3_errmsg(db));
        ret_val = -1;
    }

    if (ret_val == 0)
    {
        exit = sqlite3_prepare_v2(db, sql, -1., &stmt, NULL);
        if (exit != SQLITE_OK)
        {
            sqlite3_close(db);
            ret_val = -1;
        }

        if (ret_val == 0)
        {
            sqlite3_bind_int(stmt, 1, random);
            exit = sqlite3_step(stmt);
            if (exit != SQLITE_ROW)
            {
                sqlite3_close(db);
                ret_val = -1;
            }
            else
            {
                database_info = sqlite3_column_text(stmt, 0);
                strncpy(info, database_info, size);
            }

            if (ret_val == 0)
            {
                sqlite3_finalize(stmt);
                sqlite3_close(db);
            }
        }
    }

    return ret_val;
}

/**
 * Generates a random number within a specified range. This number will be 
 * used to select a random id from the selected database.
 * 
 * Parameters: mode (int) :
 *             upper_hir (int) :
 *             lower_hir (int) :
 *             upper_kat (int) :
 *             lower_kat (int) :
 *             both_mode (int *) :
 * 
 * Returns: 
 */
int randomNum(int mode, int upper_hir, int lower_hir, int upper_kat, int lower_kat, int *both_mode)
{
    int random_num;

    srand(time(NULL));

    if (mode == 0 || mode == 1)
    {
        random_num = rand() % (upper_hir - lower_hir + 1) + lower_hir;
        *both_mode = 0;
    }
    else if (mode == 2 || mode == 3)
    {
        random_num = rand() % (upper_kat - lower_kat + 1) + lower_kat;
        *both_mode = 1;
    }
    else if (mode == 4 || mode == 5)
    {
        random_num = rand();
        if (random_num % 2 == 0)
        {
            *both_mode = 0;
            random_num = rand() % (upper_hir - lower_hir + 1) - lower_hir;
        }
        else
        {
            *both_mode = 1;
            random_num = rand() % (upper_kat - lower_kat + 1) - lower_kat;
        }
    }
    else
    {
        random_num = -1;
    }

    return random_num;
}

/**
 * Prints the results of each question, the correct answer to the question, the user's 
 * answer to the question, how many points the user gained for each question, as well as 
 * their final score.
 * 
 * Parameters: head (struct node *) :
 *             final_score (int) :
 *             mode (int) :
 * 
 * Returns: 
 */
void results(struct node *head, int final_score, int mode)
{
    int i = 0;
    struct node *iter = head;

    printf("\nHere Are Your Results\n"
           "=====================\n");

    while (iter != NULL)
    {
        printf("\nQuestion %d: %s", i + 1, iter->question);
        printf("Correct Romaji: %s", iter->correct_romaji);
        printf("Your Romaji Answer: %s", iter->user_romaji);
        if (mode == 1 || mode == 3 || mode == 5)
        {
            printf("Correct English Meaning: %s", iter->correct_english);
            printf("Your English Meaning Answer: %s", iter->user_english);
        }
        printf("Points for Question %d: %d\n", i + 1, iter->points);

        iter = iter->next;
        i += 1;
    }

    printf("\nYour Final Score is: %d\n", final_score);
    printf("Thank You For Playing!");
}

/**
 * Changes the given input into all lower case letters by changing the 
 * characters ASCII value.
 * 
 * Parameters: input (char []) :
 *             size (int) :
 * 
 * Returns: 
 */
void toLower(char input[], int size)
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