/*
 * game.h
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
 
#ifndef __GAME_H__
#define __GAME_H__

#include "dictionary.h"
#include "generate.h"
#include <stdbool.h>

typedef enum {
	gniggle_score_traditional,
	gniggle_score_letters,
	gniggle_score_multiply
} gniggle_score_style;

struct gniggle_game {
	unsigned int width;
	unsigned int height;
	unsigned char *grid;
	unsigned int score;
	const unsigned char **answers;
	struct gniggle_dictionary *dict;
	struct gniggle_dictionary *found;
};

/* returns the score of a word using the specified scoring style.
 * gniggle_score_traditional: Normal Boggle-style scoring
 * gniggle_score_letters: A word scores 1 point for each letter past the first
 *				two.  (3 = 1, 4 = 2, 5 = 3, etc)
 * gniggle_score_multiply: A word scores 1 point for each letter past the first
 *				two, multipled by two. (3 = 1, 4 = 4, 5 = 6)
 */
unsigned int gniggle_game_word_score(gniggle_score_style style,
					const unsigned char *word);

/* create a new game.  If generate is false, type is a string for the grid,
 * left to right, top to bottom.  If it is true, a random game is generated,
 * where type is used to select one of the letter distributions defined in
 * generate.h, or NULL to emulate a real Boggle dice set.
 */
struct gniggle_game *gniggle_game_new(bool generate, const unsigned char *type,
					unsigned int width,
					unsigned int height,
					struct gniggle_dictionary *dict);

/* deletes an existing game, and frees all memory assoicated with it. */
void gniggle_game_delete(struct gniggle_game *game);

/* returns a string array with all valid words for this game.  It is the
 * caller's responsibility to free this.
 */
const unsigned char **gniggle_game_get_answers(struct gniggle_game *game);

/* add a word to the list of words found by the user.  It returns the word's
 * score, zero if the word is not on the board, -1 if the word has already
 * been guessed, or -2 if the word is not in the dictionary.
 */
int gniggle_game_try_word(struct gniggle_game *game,
					const unsigned char *word);
#endif /* __GAME_H__ */
