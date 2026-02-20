#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

int create_table(char[], char **);
int insert_data(char[], char[]);

int main(int argc, char *argv[])
{
    char db_name[] = "katakana.db";
    char filename[] = "../katakana.txt";
    char *error_message = NULL;
    int ret_val = 0;

    if (create_table(db_name, &error_message) == -1)
    {
        printf("Error creating table\n");
        ret_val = -1;
    }
    else
    {
        printf("Successfully created database table\n");
    }

    if (ret_val == 0 && insert_data(db_name, filename) == -1)
    {
        printf("Error inserting data\n");
        ret_val = -1;
    }
    else if (ret_val == 0)
    {
        printf("Successfully inserted data\n");
    }

    return ret_val;
}

int create_table(char db_name[], char **error_message)
{
    char *sql = "CREATE TABLE IF NOT EXISTS katakana_mode ("
                "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                "Katakana TEXT NOT NULL, "
                "Romaji TEXT NOT NULL);";
    int ret_val = 0;
    int rc;
    sqlite3 *db = NULL;

    rc = sqlite3_open(db_name, &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    rc = sqlite3_exec(db, sql, NULL, 0, error_message);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", *error_message);
        sqlite3_free(*error_message);
        sqlite3_close(db);
        ret_val = -1;
    }

    sqlite3_close(db);
    return ret_val;
}

int insert_data(char db_name[], char filename[])
{
    char katakana[50];
    char romaji[50];
    int ret_val = 0;
    int rc;
    sqlite3 *db = NULL;
    sqlite3_stmt *stmt = NULL;

    char *sql = "INSERT INTO katakana_mode (Katakana, Romaji) VALUES (?, ?);";

    FILE *in_file = fopen(filename, "r");
    if (in_file == NULL)
    {
        fprintf(stderr, "Error opening file: %s\n", filename);
        return -1;
    }

    rc = sqlite3_open(db_name, &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        fclose(in_file);
        return -1;
    }

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Error preparing statement: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        fclose(in_file);
        return -1;
    }

    while (1)
    {
        if (fgets(katakana, sizeof(katakana), in_file) == NULL) break;
        if (fgets(romaji,   sizeof(romaji),   in_file) == NULL) break;

        sqlite3_bind_text(stmt, 1, katakana, -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, romaji,   -1, SQLITE_TRANSIENT);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            fprintf(stderr, "Execution failed: %s\n", sqlite3_errmsg(db));
            ret_val = -1;
            break;
        }

        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    fclose(in_file);

    return ret_val;
}