#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* collect_colorstack: takes a given empty array of pointers and fills it
 * with pointers to char of cards that belong to it.
 * E.g. before the function call:
 * 	red = {}
 * 	after function call:
 * 	red = {"red_1", "red_2", "red_3", ... , "red_9"} */
void collect_cstack(char *stack[], char *s, int cn)
{
	int i;
	for (i = 0; i < cn; i++, stack++) {
		*stack = malloc(sizeof(char) * (strlen(s) + 3));
		char *temp = *stack;

		if (i < 10) {
			strcpy(temp, s);
			temp += strlen(s);
			*temp++ = '_';
			*temp++ = '0' + i;
			*temp = '\0';
		} else {
			strcpy(temp, s);
			temp += strlen(s);
			*temp++ = '_';
			*temp++ = '0' + (i - 9);
			*temp = '\0';
		}
	}
}

/* same as collect collect_cstack(), but with tool-cards instead of color-cards */
void collect_tstack(char *stack[], char *s, int cn)
{
	char *colors[] = {"red", "green", "blue", "yellow"}, **pcolors = colors;
	int i;
	for (i = 0; i < cn; i++, stack++) {
		if ((i != 0) && i%2 == 0)
			pcolors++;

		*stack = malloc(sizeof(char) * (strlen(s) + strlen(*pcolors) + 2));
		char *temp = *stack;

		strcpy(temp, *pcolors);
		temp += strlen(*pcolors);
		*temp++ = '_';
		strcpy(temp, s);
		temp += strlen(s);
		*temp = '\0';
	}

}

/* same as collect_cstack(), but with wild- and wild_draw-cards */
void collect_wstack(char *stack[], char *s1, char *s2, int cn)
{
	char *alt[] = {s1, s2}, **palt = alt;
	int i;
	for (i = 0; i < cn; i++, stack++) {
		if (i > 3)
			palt = alt + 1;
		*stack = malloc(sizeof(char) * (strlen(*palt) + 1));
		char *temp = *stack;

		strcpy(temp, *palt);
	}
}

char **pdeck = NULL;

/* collect_deck: copy given stack to deck; stack remains unmodified */
void collect_deck(char *deck[], char *stack[], int cn)
{
	static int deck_flag = 0;
	if (!deck_flag) {
		pdeck = deck;
		deck_flag = 1;
	}
	while (cn-- > 0) {
		size_t length = strlen(*stack) + 1;
		if ((*pdeck = malloc(sizeof(char) * length)) == NULL) {
			fprintf(stderr, "Memory allocation failed\n");
			exit(1);
		}
		strcpy(*pdeck++, *stack++);
	}
	*pdeck = NULL;
}

void append_user(User *users[MAX_USERS], int *nusers)
{
	if (*nusers >= MAX_USERS) {
		printf("Max user limit exceeded\n");
		return;
	} else {
		char name[100];
		users[*nusers] = malloc(sizeof(User));
		
		printf("(add) Create a username: ");
		fgets(name, 100, stdin);
		name[strlen(name)-1] = '\0';

		users[*nusers]->name = malloc(sizeof(char) * strlen(name) + 1);
		strcpy(users[*nusers]->name, name);

		printf("New user added: %s\n", users[*nusers]->name);
		memset(users[*nusers]->ustack, 0, sizeof(users[*nusers]->ustack));
		users[*nusers]->cardp = 0;
		++*nusers;
	}
}

void free_users(User *users[MAX_USERS], int nusers)
{
	int i, j;
	for (i = 0; i < nusers; i++) {
		free(users[i]->name);
		users[i]->name = NULL;
		while (j = 0; j < users[i]->cardp; j++) {
			free(users[i]->ustack[j]);
			users[i]->ustack[j] = NULL;
		}
		free(users[i]);
		users[i] = NULL;
	}
}

/* take_card: pops top pointer from deck and inserts it to user */
void take_card(User *user, char *deck[])
{
	if ((user->ustack[cardp] = malloc(sizeof(char) * (strlen(*--pdeck) + 1))) == NULL) {
		fprintf(stderr, "Memory allocation failed\n");
		exit(1);
	}
	strcpy(user->ustack[cardp++], *pdeck);
	free(*pdeck);
	
	*pdeck = NULL;
	user->ustack[cardp] = NULL;
}

char **ptable = NULL;

/* put_card: pops card in card_id location of user and inserts it to next free location of table
 * put_card() only gets called when card_is_compatible returns 1 */
char **put_card(User *user, int card_id, int cn, char *table[])
{
	static int table_flag = 0;
	if (!table_flag) {
		ptable = table;
		table_flag = 1;
	}

	card_id--;

	user -= cn;
	user += card_id;
	if ((*ptable = malloc(sizeof(char) * (strlen(*user) + 1))) == NULL) {
		fprintf(stderr, "Memory allocation failed.\n");
		exit(1);
	}
	strcpy(*ptable++, *user);

	while (*(user+1) != NULL) {
		*user = realloc(*user, sizeof(char) * (strlen(*(user+1)) + 1));
		strcpy(*user, *(user + 1));
		*user++;
	}
	free(*user);
	*user = NULL;
	*ptable = NULL;

	return user;
}

/* card_is_compatible: returns 1 if card can be put to table according to uno rules,
 * and 0 otherwise */
int card_is_compatible(char *ucard, char *tbcard)
{
	int i;

	int red = 0, green = 0, blue = 0, yellow = 0;
	static char color;

	for (i = 0; ucard[i] != '_'; i++)
		;
	char *umid = ucard + i;
	for (i = 0; tbcard[i] != '_'; i++)
		;
	char *tbmid = tbcard + i;

	if ((strcmp(umid, tbmid) == 0 && strcmp(tbcard, "wild_draw") != 0) || strncmp(ucard, tbcard, umid - ucard) == 0)
		return 1;
	else if (strcmp(ucard, "wild") == 0 || strcmp(ucard, "wild_draw") == 0) {
		printf("Choose color [R/G/B/Y]: ");
		while (color != 'R' && color != 'G' && color != 'B' && color != 'Y')
			scanf("%c", &color);
		return 1;
	} else if (strcmp(tbcard, "wild") == 0 || strcmp(tbcard, "wild_draw") == 0) {
		switch (color) {
			case 'R':
				return (strncmp(ucard, "red", umid - ucard) == 0) ? 1 : 0;
				color = '\0';
				break;
			case 'G':
				return (strncmp(ucard, "green", umid - ucard) == 0) ? 1 : 0;
				color = '\0';
				break;
			case 'B':
				return (strncmp(ucard, "blue", umid - ucard) == 0) ? 1 : 0;
				color = '\0';
				break;
			case 'Y':
				return (strncmp(ucard, "yellow", umid - ucard) == 0) ? 1 : 0;
				color = '\0';
				break;
			default:
				printf("It's physically impossible to reach this line of code\n");
				break;
		}
	}
	return 0;
}

/* shuffle_deck: uses Fisher-Yates algorithm to shuffle the array deck unbiasedly */
void shuffle_deck(char *deck[], int cn)
{

	int i;

	char *temp;

	for (i = 0; i < cn - 1; i++) {
		int j = rand() % (cn - i) + i;
		temp = deck[i];
		deck[i] = deck[j];
		deck[j] = temp;
	}
}

/* print_stack: print every string of given stack to standard output
 * if is_vertical is true, one string will be printed per line, otherwise
 * they will be printed 7 strings per line for convenience and readability */
void print_stack(char *stack[], int cn, int is_vertical)
{
	int i;
	if (is_vertical)
		for (i = 1; i <= cn; i++)
			printf("%s\n", *stack++);
	else {
		for (i = 1; i <= cn; i++)
			printf("%s%2c", *stack++, (i % 7 == 0) ? '\n' : ' ');
		putchar('\n');
	}
}

/* stack_size: return number of cards in a given stack.
 * Note that it only works if the last pointer element is null */
int stack_size(char *stack[])
{
	int len = 0;
	while (stack[len] != NULL)
		len++;
	return len;
}
