/**
 * Contains the struct definition of a node and
 * what information it holds. 
 */

struct node
{
    char            question[50];
    char            correct_romaji[50];
    char            user_romaji[50];
    int             points;
    struct node*    next;
};