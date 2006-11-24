/*
 * cli.c
 * This file is part of Gniggle
 *
 * Copyright (C) 2006 - Rob Kendrick <rjek@rjek.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to
 * do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "game.h"

static void usage(char *argv[])
{
	printf("%s [options]\n", argv[0]);
	printf("Options are:\n");
	printf("   -x width\n");
	printf("   -y height\n");
	printf("   -d dictionary\n");
	printf("   -g grid contents\n");
}

static void show_cube(const unsigned char *grid, unsigned int x, unsigned int y)
{
	unsigned int xx, yy, zz;
	
	printf("\n  +");
	for (zz = 0; zz < x; zz++)
		printf("---+");
	
	for (yy = 0; yy < y; yy++) {
		printf("\n  |");
		for (xx = 0; xx < x; xx++) {
			char c = grid[(yy * x) + xx];
			if (c == 'q')
				printf(" Qu|");
			else
				printf(" %c |", toupper(c));
		}
		printf("\n  +");
		for (zz = 0; zz < x; zz++)
			printf("---+");	
	}
		
	printf("\n\n");
}

int main(int argc, char *argv[])
{
	unsigned int width = 4, height = 4;
	unsigned char *grid = NULL, *dictionary = NULL, word[BUFSIZ];
	int a, w = 0;
	struct gniggle_game *g;
	struct gniggle_dictionary *d;
	bool quit = false;
	unsigned int score = 0, mscore = 0;
	const unsigned char **answers;
	FILE *f;
	
	if (argc > 1) {
		for (a = 1; a < argc; a++) {
			if (argv[a][0] == '-') {
				/* all need a parameter */
				if (a == argc) {
					usage(argv);
					exit(1);
				}
				
				switch(argv[a][1]) {
				case 'x':
					width = atoi(argv[a + 1]);
					a++;
					break;
				case 'y':
					height = atoi(argv[a + 1]);
					a++;
					break;
				case 'd':
					dictionary = strdup(argv[a + 1]);
					a++;
					break;
				case 'g':
					grid = strdup(argv[a + 1]);
					a++;
					break;
				default:
					usage(argv);
					exit(1);
				}
			}
		}
	}

	if (dictionary == NULL)
		dictionary = strdup("/usr/share/dict/words");
			
	if (grid == NULL) {
		switch (width * height) {
		case 16:
		case 25:
			grid = gniggle_generate_real(width, height);
			break;
		default:
			grid = gniggle_generate_simple(GNIGGLE_BOGGLE,
							width, height);
			break;
		}
	}
		
	printf("loading dictionary... "); fflush(stdout);
	d = gniggle_dictionary_new(width, height, 0);
	if (gniggle_dictionary_load_file(d, dictionary) == -1) {
		fprintf(stderr, "unable to open %s\n", dictionary);
		exit(1);
	}
	
	printf("%d words.\n", gniggle_dictionary_size(d));
	
	g = gniggle_game_new(false, grid, width, height, d);
	
	show_cube(grid, width, height);
	
	printf("Enter a . (a dot) on a line of its own to give up.\n");
	printf("An empty line will print out the cube again.\n\n");
	
	do {
		printf("(%d) :", score); fflush(stdout);
		fgets(word, BUFSIZ, stdin);
		if (word[strlen(word) - 1] == '\n')
			word[strlen(word) - 1] = '\0';
			
		if (word[0] == '\0')
			show_cube(grid, width, height);
		else if (strcmp(word, ".") == 0)
			quit = true;
		else {
			int ll;
		  	int wscore;
			
			for (ll = 0; ll < strlen(word); ll++)
				word[ll] = tolower(word[ll]);

			wscore = gniggle_game_try_word(g, word);

			switch (wscore) {
			case 0:
				printf("That's not on the board.\n");
				break;
			case -1:
				printf("You've already guessed that!\n");
				break;
			case -2:
				printf("Sorry, I don't know that word.\n");
				break;
			default:
				printf("Yes! %d point%s.\n", wscore,
					wscore == 1 ? "" : "s");
				score += wscore;
				break;
			}
		}
		fflush(stdout);
	} while (quit == false);
	
	printf("Finding words you missed...\n"); fflush(stdout);
	answers = g->answers;
	
	for (a = 0; a >= 0 ; a++) {
		if (answers[a] == NULL)
			a = -2;
		else {
			if (gniggle_dictionary_lookup(g->found, answers[a])
				== false) {
				unsigned char *qu =
				     gniggle_dictionary_restore_qu(answers[a]);
				printf("%s\t\t", qu);
				free(qu);
				mscore += gniggle_game_word_score(
					gniggle_score_traditional, answers[a]);
				w += 1;
				if (w >= 5) {
					printf("\n");
					w = 0;
				}
				
			}
		}
	}
	
	printf("\nYour score: %d.  You missed out on %d point%s.\n",
			score, mscore, mscore == 1 ? "" : "s");
		
				
}
