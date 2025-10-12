#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

int create_table(char [], sqlite3 *, char *, int);
int insert_data(char [], char [], sqlite3 *, int);

int main(int argc, char *argv[])
{
    char db_name[20] = "hirigana.db";
    char filename[20] = "../hirigana.txt";
    char *error_messgae;
    sqlite3 *db;
    int ret_val = 0, exit = 0;

    if (create_table(db_name, db, error_messgae, exit) == -1)
    {
        printf("Error creating table\n");
        ret_val = -1;
    }
    else
    {
        printf("Successfully created database table\n");
    }

    if (insert_data(db_name, filename, db, exit) == -1)
    {
        printf("Error inserting data\n");
        ret_val = -1;
    }
    else
    {
        printf("Successfully inserted data\n");
    }

    return ret_val;
}

int create_table(char db_name[], sqlite3 *db, char *error_message, int exit)
{
    char *sql = "CREATE TABLE hirigana_mode ("
                "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                "Hirigana TEXT NOT NULL, "
                "Romaji TEXT NOT NULL, "
                "English TEXT NOT NULL );";
    int ret_val = 0;

    exit = sqlite3_open(db_name, &db);

    if (exit)
    {
        ret_val = -1;
    }
    
    if (ret_val == 0)
    {
        exit = sqlite3_exec(db, sql, NULL, 0, &error_message);

        if (exit)
        {
            printf("%s\n", error_message);
            sqlite3_close(db);
            ret_val = -1;
        }
    }

    if (ret_val == 0)
    {
        sqlite3_close(db);
    }

    return ret_val;
}

int insert_data(char db_name[], char filename[], sqlite3 *db, int exit)
{
    char hirigana[50];
    char romaji[50];
    char english[50];
    int ret_val = 0;

    sqlite3_stmt *stmt;

    char *sql = "INSERT INTO hirigana_mode (Hirigana, Romaji, English) VALUES (?, ?, ?);";

    FILE *in_file;
    in_file = fopen(filename, "r");
    if (in_file == NULL)
    {
        printf("Filename: %s\n", filename);
        printf("error opening file\n");
        ret_val = -1;
    }

    exit = sqlite3_open(db_name, &db);

    if (exit)
    {
        ret_val = -1;
    }

    if (ret_val == 0)
    {
        exit = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
        if (exit != SQLITE_OK)
        {
            printf("error preparing\n");
            sqlite3_close(db);
            ret_val = -1;
        }

        while (ret_val != -1)
        {
            if (fgets(hirigana, 50, in_file) == NULL) 
            {
                break;
            }
            if (fgets(romaji, 50, in_file) == NULL) 
            {
                break;
            }
            if (fgets(english, 50, in_file) == NULL) 
            {
                break;
            }

            sqlite3_bind_text(stmt, 1, hirigana, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, romaji, -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, english, -1, SQLITE_STATIC);

            exit = sqlite3_step(stmt);
            if (exit != SQLITE_DONE)
            {
                fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
                sqlite3_close(db);
                ret_val = -1;
            }   

            if (ret_val == 0)
            {
                sqlite3_reset(stmt);
                sqlite3_clear_bindings(stmt);
            }
        }
        if (ret_val == 0)
        {
            sqlite3_finalize(stmt);
        }
    }

    if (ret_val == 0)
    {
        sqlite3_close(db);
        fclose(in_file);
    }

    return ret_val;
}