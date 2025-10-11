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
                "Romaji TEXT NOT NULL );";
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

/*
int i = 0;
char word[]; // wchar ??
while(feof == 0)
{
    fgets(word, 100, outfile);

    // Do this or find a way where the sql statement takes a variable

    strcpy(word, ""); // Extra stuff needed for a sql statement

    strcpy(sql, word); // Create the completed sql statement

    if (i % 2 == 0)
    {
        sqlite3.exec() // Insert to the hirigana portion of the database
    }
    else
    {
        sqlite3.exec() // Insert to the english portion of the database
    }
}

*/