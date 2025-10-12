#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#include "node.h"

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
    char c;
    do
    {
        c = fgetc(stdin);
    } while (c != "\n" && c != EOF);
}

/*
Selects the mode of the game depeneding on what the user
chose. This will specify what database to take information
from.
*/
int chooseMode(int *mode, char *user_input, int length, int *upper_hir, int *lower_hir, int *upper_kat, int *lower_kat)
{
    int ret_val = 0;

    if(strncmp("hirigana characters", user_input, length))
    {
        mode = 0;
        *upper_hir = 104;
        *lower_hir = 1;
    }
    else if(strncmp("hirigana words", user_input, length))
    {
        mode = 1;
        *upper_hir = 273;
        *lower_hir = 105;
    }
    else if(strncmp("katakana characters", user_input, length))
    {
        mode = 2;
        *upper_kat = 0; // Placeholders for when I create katakana database
        *lower_kat = 0;
    }
    else if(strncmp("katakana words", user_input, length))
    {
        mode = 3;
        *upper_kat = 0; // Placeholders for when I create katakana database
        *lower_kat = 0;
    }
    else if(strncmp("both characters", user_input, length))
    {
        mode = 4;
        *upper_hir = 0; // Placeholders for when I create katakana database
        *upper_kat = 0;
        *lower_hir = 0;
        *lower_kat = 0;
    }    
    else if(strncmp("both words", user_input, length))
    {
        mode = 5;
        *upper_hir = 0; // Placeholders for when I create katakana database
        *upper_kat = 0;
        *lower_hir = 0;
        *lower_kat = 0;
    }
    else
    {
        ret_val = -1;
    }

    return ret_val;
}

/*
Creates a node that stores information about the current
question the user answered. Alwyas adds the node to the end 
of the list.
*/
int createNode(struct node *head, struct node *tail, int id, int mode, int points, char user_answer[], int size)
{
    int ret_val = 0;
    struct node *iter = head;

    struct node *new_node = (struct node *)malloc(sizeof(struct node));
    if (new_node == NULL)
    {
        ret_val = -1;
    }

    if (ret_val == 0)
    {
        new_node->id = id;
        new_node->mode = mode;
        new_node->points = points;
        strncpy(new_node->user_answer, user_answer, size);
        new_node->next = NULL;

        if(head == NULL && tail == NULL)
        {
            head = &new_node;
            tail = &new_node;
        }
        else
        {
            tail->next = &new_node;
        }
    }

    return ret_val;
}

int getData(int both_mode, int random, int database_type, char info[], int size)
{
    int ret_val = 0;
    sqlite3 *db;
    sqlite3_stmt *stmt;
    char db_name[20];
    char *error_message;
    int exit;
    char *sql;

    if (both_mode == 0)
    {
        strcpy(db_name, "hirigana.db");
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
        strcpy(db_name, "katakana.db");
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
                strncpy(info, sqlite3_column_text(stmt, 0), size);
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

/*
Generates a random number. This number will be used to
select a random question from the selected database.
*/
int randomNum(int mode, int upper_hir, int lower_hir, int upper_kat, int lower_kat, int *both_mode)
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
void results(struct node *head, int final_score)
{

}

/*
Changes the given input into all lower case letters by
changing the characters ASCII value.
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