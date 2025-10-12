struct node
{
    char            question[50];
    char            correct_romaji[50];
    char            correct_english[50];
    char            user_romaji[50];
    char            user_english[50];
    int             points;
    struct node*    next;
};