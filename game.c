/*
 * game.c
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

#include <string.h>
#include <stdlib.h>
#include "game.h"
#include "dictionary.h"
#include "solve.h"

unsigned int gniggle_game_word_score(gniggle_score_style style,
					const unsigned char *word)
{
	int l = strlen(word), i;
	
	/* count the number of Qs in this string, as they class as two
	 * letters.
	 */
	 
	for (i = 0; i < strlen(word); i++)
		if (word[i] == 'q')
			l++;
	
	switch (style) {
	case gniggle_score_traditional:
		if (l == 3 || l == 4) return 1;
		if (l == 5) return 2;
		if (l == 6) return 3;
		if (l == 7) return 5;
		if (l > 7) return 11;
		break;
	case gniggle_score_letters:
		return l - 2;
		break;
	case gniggle_score_multiply:
		if (l > 3)
			return (l - 2) * 2;
		else
			return 1;
		break;
	}
	
	return 0;
}

struct gniggle_game *gniggle_game_new(bool generate, const unsigned char *type,
					unsigned int width,
					unsigned int height,
					struct gniggle_dictionary *dict)
{
	struct gniggle_game *r = calloc(sizeof(struct gniggle_game), 1);
	
	r->width = width;
	r->height = height;
	r->dict = dict;
	r->found = gniggle_dictionary_new(width, height, 0);
	r->score = 0;
	
	if (generate == false)
		r->grid = strdup(type);
	else {
		if (type == NULL)
			r->grid = gniggle_generate_real(width, height);
		else
			r->grid = gniggle_generate_simple(type, width, height);
	}
	
	r->answers = gniggle_game_get_answers(r);
	
	return r;
}

void gniggle_game_delete(struct gniggle_game *game)
{
	free(game->grid);
	free(game->answers);
	gniggle_dictionary_delete(game->found);
	
	free(game);
}

static int gniggle_game_answers_sort(const void *p1, const void *p2)
{
	return strcmp(* (char * const *) p1, * (char * const *) p2);
}

const unsigned char **gniggle_game_get_answers(struct gniggle_game *game)
{
	/* we allocate enough space for 64 words, including the sentinal.
	 * we can extend this if there are more words.
	 */
	const unsigned char **r = calloc(sizeof(char *), 64);
	struct gniggle_dictionary_iter *iter;
	const unsigned char *word;
	unsigned int room = 63, found = 0;
	
	iter = gniggle_dictionary_iterator(game->dict);
	
	while ((word = gniggle_dictionary_next(iter)) != NULL) {
		if (gniggle_solve_word_on_grid(word, game->grid,
				game->width, game->height, NULL) == true) {
				r[found] = word;
				found++;
				room--;
				if (room == 0) {
					r = realloc(r, (found + 64) * 
							sizeof(char *));
					room = 63;
				}
						
		}
	}
	
	r[found] = NULL;
	
	qsort(r, found, sizeof(char *), gniggle_game_answers_sort);
	
	free(iter);
	
	return r;
}

int gniggle_game_try_word(struct gniggle_game *game,
					const unsigned char *word) {
					
	unsigned char *nqu = gniggle_dictionary_trim_qu(word);
	unsigned int score;
	
	if (gniggle_solve_word_on_grid(nqu, game->grid, game->width,
					game->height, NULL) == false)
	{
		free(nqu);
		return 0;	
	}
	
	if (gniggle_dictionary_lookup(game->found, nqu) == true) {
		free(nqu);
		return -1;
	}
	
	if (gniggle_dictionary_lookup(game->dict, nqu) == false) {
		free(nqu);
		return -2;
	}
	
	gniggle_dictionary_add(game->found, nqu);
	
	score = gniggle_game_word_score(gniggle_score_traditional, nqu);
	game->score += score;
	free(nqu);
	
	return score;
}

#ifdef TEST_RIG
#include <stdio.h>

int main(int argc, char *argv[])
{
	struct gniggle_dictionary *d;
	struct gniggle_game *g;
	unsigned char word[BUFSIZ];
	const unsigned char **answers;
	int i, score = 0, wscore, j;
	unsigned int path[64];
	
	FILE *dict = fopen("dict", "r");
	
	d = gniggle_dictionary_new(4, 4, 0);
	while (fscanf(dict, "%s", word) == 1)
		gniggle_dictionary_add(d, word);
	fclose(dict);
	
	g = gniggle_game_new((argc > 1) ? false : true, (argc > 1) ? argv[1] : NULL, 4, 4, d);
	
	for (i = 0; i < 16; i += 4) {
		printf("%c %c %c %c\n", g->grid[i], g->grid[i + 1], g->grid[i + 2], g->grid[i + 3]);
	}
	
	printf("\n");
	
	answers = gniggle_game_get_answers(g);

	for (i = 0; i >= 0 ; i++) {
		if (answers[i] == NULL)
			i = -2;
		else {
			wscore = gniggle_game_word_score(gniggle_score_traditional, answers[i]);
			printf("%s ( ");
			gniggle_solve_word_on_grid(answers[i], g->grid, 4, 4, path);
			for (j = 0; j < (strlen(answers[i]) * 2); j += 2)
				printf("%d x %d  ", path[j], path[j + 1]);
			printf(") (%d points)\n", wscore);
			score += wscore;
		}
	}
	
	printf("total score: %d\n", score);
	
	free(answers);
	gniggle_game_delete(g);
	gniggle_dictionary_delete(d);

}

#endif
