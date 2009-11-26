/*
 * cli.c
 * This file is part of Gniggle
 *
 * Copyright (C) 2006 - Rob Kendrick <rjek@rjek.com>
 * Copyright (C) 2007 - James Shaw <js102@zepler.net>
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
	printf("   -c dictionary dump to create\n");
}

static int cube_index(
		const unsigned int xx,
		const unsigned int yy,
		const unsigned int width,
		const unsigned int height,
		const unsigned int rotation) {
	switch (rotation) {
	case 0:
	default:
		return (yy * width) + xx;
	case 1:
		return (height * (width - xx - 1)) + yy;
	case 2:
		return (width * (height - yy)) - xx - 1;
	case 3:
		return (height * (xx + 1)) - yy - 1;
	}
}

static void show_cube(const char *grid, unsigned int x, unsigned int y,
			unsigned int rotation)
{
	unsigned int xx, yy, zz;
	if (rotation % 2 == 1) {
		unsigned int t;
		/* need to swap x and y when rotated 90 or 270 degrees */
		t = y;
		y = x;
		x = t;
	}
	
	printf("\n  +");
	for (zz = 0; zz < x; zz++)
		printf("---+");
	
	for (yy = 0; yy < y; yy++) {
		printf("\n  |");
		for (xx = 0; xx < x; xx++) {
			char c = grid[cube_index(xx, yy, x, y, rotation)];
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
	unsigned int rotation = 0;
	char *dump = NULL, *grid = NULL, *dictionary = NULL;
	char word[BUFSIZ];
	int a, w = 0;
	struct gniggle_game *g;
	struct gniggle_dictionary *d;
	bool quit = false;
	unsigned int score = 0, mscore = 0;
	const char **answers;
	
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
				case 'c':
					dump = strdup(argv[a + 1]);
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
	d = gniggle_dictionary_new_from_file(width, height, 0, dictionary);
	if (d == NULL) {
		fprintf(stderr, "unable to open %s\n", dictionary);
		exit(1);
	}

	printf("%d words.\n", gniggle_dictionary_size(d));

	if (dump != NULL) {
		printf("dumping dictionary... "); fflush(stdout);
	  	gniggle_dictionary_dump(d, dump);
		printf("done.\n");
		gniggle_dictionary_delete(d);
		exit(1);
	}	

	g = gniggle_game_new(false, grid, width, height, d);
	
	show_cube(grid, width, height, rotation);
	
	printf("Enter a . (a dot) on a line of its own to give up.\n");
	printf("Enter 'r' to rotate the cube.\n");
	printf("An empty line will print out the cube again.\n\n");
	
	do {
		printf("(%d) :", score); fflush(stdout);
		if (fgets(word, BUFSIZ, stdin) == NULL)
		{
			printf("Error reading from stdin.\n");
			exit(EXIT_FAILURE);
		}

		if (word[strlen(word) - 1] == '\n')
			word[strlen(word) - 1] = '\0';
			
		if (word[0] == '\0')
			show_cube(grid, width, height, rotation);
		else if (strcmp(word, ".") == 0)
			quit = true;
		else if (strcmp(word, "r") == 0) {
			rotation = (rotation + 1) % 4;
			show_cube(grid, width, height, rotation);
		} else {
			size_t ll;
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
				char *qu =
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
		
	exit(EXIT_SUCCESS);
}
