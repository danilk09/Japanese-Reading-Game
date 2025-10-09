#include <sqlite3.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    char filename[20] = "katakana.db";

    sqlite3* db;
    int exit = 0;
    char *sql = "CREATE TABLE katakana_mode ("
                "ID INT PRIMARY KEY NOT NULL, "
                "Katakana TEXT NOT NULL, "
                "English TEXT NOT NULL );";
    char *errormessage;

    exit = sqlite3_open(filename, &db);
    exit = sqlite3_exec(db, sql, NULL, 0, &errormessage);

    if (exit)
    {
        printf("Error opening database\n");
        printf("%s\n", errormessage);
        return -1;
    }
    else
    {
        printf("Opened database\n");
    }

    sqlite3_close(db);
    return 0;
}