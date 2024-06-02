#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "uno.h"

char *deck[DECKSIZE];
char *red[CSIZE], *green[CSIZE], *blue[CSIZE], *yellow[CSIZE];
char *skip[TSIZE], *rev[TSIZE], *draw[TSIZE];
char *wild[TSIZE];

User *users[10];

char *table[DECKSIZE];

int main()
{
	srand(time(NULL));

	collect_cstack(red, "red", CSIZE);
	collect_cstack(green, "green", CSIZE);
	collect_cstack(blue, "blue", CSIZE);
	collect_cstack(yellow, "yellow", CSIZE);

	collect_tstack(skip, "skip", TSIZE);
	collect_tstack(rev, "reverse", TSIZE);
	collect_tstack(draw, "draw", TSIZE);
	collect_wstack(wild, "wild", "wild_draw", TSIZE);

	/* array of pointers to arrays (of pointers) */
	char *(*color_stacks[])[CSIZE] = {&red, &green, &blue, &yellow};
	char *(*tool_stacks[])[TSIZE] = {&skip, &rev, &draw, &wild};

	int i;
	for (i = 0; i < 4; i++) { /* generate the whole deck, unshuffled */
		collect_deck(deck, *color_stacks[i], CSIZE);
		collect_deck(deck, *tool_stacks[i], TSIZE);
	}

	extern char **pdeck, **ptable;

	int nusers = 0, user_id, deck_locator, card_scope, ntable = 0;
	int nuser_cards[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	
	int random_pick = rand() % 108 + 1;
	/* this inhibits initializing the table with a wild card */
	while (strcmp(deck[random_pick - 1], "wild") == 0 || strcmp(deck[random_pick - 1], "wild_draw") == 0)
		random_pick = rand() % 108 + 1;
	pdeck = put_card(pdeck, random_pick, pdeck - deck, table);
	ntable++;

	int run = 1;
	printf("\033[2J\033[H\n\n\n");


	char commands[300] = "Options:\nl: show this list\na: add user\ns: shuffle deck\nt: take card\n\
p: put card\nr: print your stack\nd: display deck cards\nT: show table\nq: quit\n";
	printf("%s", commands);
	
	char action[3];
	while (run && printf("\n>> ") && scanf("%s", &action)) {
		printf("\033[2J\033[H\n\n"); /* clear the screen consistently */
		switch (action[0]) {
		case 'l':
			printf("%s", commands);
			break;
		case 's':
			shuffle_deck(deck, pdeck - deck);
			break;
		case 't':
			if (nusers > 0) {
				if (nusers > 1) {
					printf("(take) Enter user_id [1-%d]: ", nusers);
					scanf("%d", &user_id);
				} else
					user_id = 1;
				user_id--;
				if (user_id >= 0 && user_id < nusers) {
					users[user_id] = take_card(users[user_id], deck);
					nuser_cards[user_id]++;
				} else
					printf("User_id undefined, please enter an inbound id\n");		
			} else
				printf("No users current, type 'a' to add a user\n");
			break;
		case 'p':
			int card_index;

			if (nusers > 0) {
				if (nusers > 1) {
					printf("(put) Enter user_id [1-%d]: ", nusers);
					scanf("%d", &user_id);
				} else
					user_id = 1;
				user_id--;

				if (user_id >= 0 && user_id < nusers) {
					printf("Enter card location: ");
					scanf("%d", &card_index);
					if (card_index <= 0 || card_index > nuser_cards[user_id])
						printf("Location not within user_hand bounds.\n");
					else if (card_is_compatible(*(users[user_id] - nuser_cards[user_id] + (card_index - 1)), *(ptable - 1))) {
						printf("%s\n|\n|\nv\n%s\n", *(users[user_id] - nuser_cards[user_id] + ( card_index - 1)), *(ptable - 1));
						users[user_id] = put_card(users[user_id], card_index, nuser_cards[user_id], table);
						nuser_cards[user_id]--;
						ntable++;
					} else
						printf("Incompatible card. Use different card or draw.\n");
				} else
					printf("User_id undefined, please enter an inbound id.\n");
			} else
				printf("No users current, type 'a' to add a user\n");
			break;
		case 'r':
			if (nusers > 0) {
				if (nusers > 1) {
					printf("(reveal) Enter user_id [1-%d]: ", nusers);
					scanf("%d", &user_id);
				} else
					user_id = 1;
				user_id--;
				if (user_id >= 0 && user_id < nusers) {
					print_stack(users[user_id] -= nuser_cards[user_id], nuser_cards[user_id], 0);
					users[user_id] += nuser_cards[user_id];
				} else
					printf("User_id undefined, please enter an inbound id\n");
			} else
				printf("No users current, type 'a' to add a user\n");
			break;
		case 'd':
			printf("Enter card-location in deck: ");
			scanf("%d", &deck_locator);
			printf("Enter number of cards you'd like to show: ");
			scanf("%d", &card_scope);
			if (deck_locator > 0 && deck_locator <= (pdeck - deck) && deck_locator + card_scope <= ((pdeck - deck) + 1) && card_scope >= 0)
				while (card_scope-- > 0)
					printf("%s\n", deck[deck_locator++ - 1]);
			else
				printf("location is not within deck bounds.\n");
			break;
		case 'T':
			print_stack(table, ntable, 0);
			break;
		case 'a':
			append_user(users, &nusers);
			break;
		case 'q':
			run = 0;
			break;
		default:
			printf("l: list commands\n");
			break;
		}


	}
	

	for (i = 0; i < CSIZE; i++) {
		free(red[i]);
		free(green[i]);
		free(blue[i]);
		free(yellow[i]);
	}

	for (i = 0; i < TSIZE; i++) {
		free(skip[i]);
		free(rev[i]);
		free(draw[i]);
		free(wild[i]);
	}

	free_users(users, nusers);

	for (i = 0; i < DECKSIZE; i++)
		free(deck[i]);

	for (i = 0; i < ntable; i++)
		free(table[i]);

	return 0;
}
