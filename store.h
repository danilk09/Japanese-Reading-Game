struct node
{
    char            question[50];
    char            correct_answer[50];
    char            user_answer[50];
    int             points[10];
    struct node*   next;
} node;