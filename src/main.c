#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <windows.h>

#include "functions.h"

int main(int argc, char* argv[])
{
    int i, random, database_type, length, upper_hir, lower_hir, upper_kat, lower_kat, mode, both_mode, curr_score;
    int ret_val = 0, total_score = 0, boolean = 0, total_questions = 10;
    int start_time, end_time, total_time;
    char user_input[10];
    char question[50];
    char user_romaji[50];
    char user_english[50] = "\0";
    char correct_romaji[50];
    char correct_english[50] = "\0";
    struct node *head = NULL;
    struct node *tail = NULL;

    SetConsoleOutputCP(CP_UTF8);
    setlocale(LC_ALL, "en_US.UTF-8");

    printf("Welcome to the Japanese Reading Game!\n"
        "=====================================\n"
        "You can either practice hirigana and katakana characters or reading hirigana and katakana words.\n"
        "Type and enter the romaji version of the Japanese character/word as quickly as possible\n"
        "The quicker you type, the more points you gain! Each question is worth up to 20 points.\n"
        "Bonus questions about the meaning of the word in English will be provided at the end!\n"
        "Note: I made the English meanings as general as possible. If the game says you got it\n"
        "wrong when you really didn't, then please forgive me!\n"
        "Please choose one of the game options below to start the game and determine what\n"
        "type of questions will be asked\n"
        "Hirigana Characters | Hirigana Words | Katakana Characters | Katakana Words | Both Characters | Both Words\n\n");

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

    printf("\nGame starts in...\n");
    for(i = 3; i > 0; i--)
    {
        sleep(1);
        printf("%d...\n", i);
    }
    printf("Start!\n"
    "_______________________\n");

    for(i = 0 && ret_val == 0; i < total_questions; i++)
    {
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

        database_type = 0;
        if (ret_val == 0 && getData(both_mode, random, database_type, question, 50) == -1)
        {
            printf("Error with obtaining the question from the database\n");
            ret_val = -1;
        }

        database_type = 1;
        if (ret_val == 0 && getData(both_mode, random, database_type, correct_romaji, 50) == -1)
        {
            printf("Error with obtaining the correct romaji from the database\n");
            ret_val = -1;
        }

        if (ret_val == 0 && (mode == 1 || mode == 3 || mode == 5))
        {
            database_type = 2;
            if (getData(both_mode, random, database_type, correct_english, 50) == -1)
            {
                printf("Error with obtaining the correct English from the database\n");
                ret_val = -1;
            }
        }

        if (ret_val == 0)
        {
            printf("\nQuestion %d: %s", i + 1, question); // Prints a random question from the database

            start_time = time(NULL);

            fgets(user_romaji, 50, stdin);

            end_time = time(NULL);

            total_time = difftime(end_time, start_time);
            printf("Time spent: %d\n", total_time);
            
            toLower(user_romaji, strlen(user_romaji));
            //clearBuffer();

            if(strncmp(user_romaji, correct_romaji, 50) == 0)
            {
                curr_score = calculatePoints(total_time);
                total_score += curr_score;
            }
            else
            {
                curr_score = 0;
            }

            if (mode == 1 || mode == 3 || mode == 5)
            {
                printf("Bonus Question! English meaning:\n");

                start_time = time(NULL);

                fgets(user_english, 50, stdin);

                end_time = time(NULL);

                total_time = difftime(end_time, start_time);

                toLower(user_english, strlen(user_english));
                //clearBuffer();

                if (strncmp(user_english, correct_english, 50) == 0)
                {
                    curr_score += calculatePoints((int)total_time);
                    total_score += curr_score;
                }
                else
                {
                    curr_score += 0;
                }
            }

            createNode(&head, &tail, question, correct_romaji, correct_english, user_romaji, user_english, curr_score, 50);
        }
    }

    results(head, total_score, mode);
    return ret_val;
}