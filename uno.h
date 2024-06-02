#define DECKSIZE 108
#define CSIZE 19
#define TSIZE 8
#define MAX_USERS 10

typedef struct User {
	char *uname;
	char *ustack[DECKSIZE];
	int cardp;
} User;

/* All the uno.c card manipulation functions that are to be used in uno.c */
void collect_cstack(char *stack[], char *s, int cn);
void collect_tstack(char *stack[], char *s, int cn);
void collect_wstack(char *stack[], char *s1, char *s2, int cn);
void collect_deck(char *deck[], char *stack[], int cn);
void append_user(User *users[MAX_USERS], int *nusers);
void free_users(User *users[MAX_USERS], int nusers);
void take_card(User *user, char *deck[]);
char **put_card(User *user, int card_id, int cn, char *table[]);
int card_is_compatible(char *ucard, char *tbcard);
void shuffle_deck(char *deck[], int cn);
void print_stack(char *stack[], int cn, int is_vertical);
int stack_size(char *stack[]);
