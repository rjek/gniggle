/*
 * solve.c
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

#ifndef __SOLVE_H__
#define __SOLVE_H__

#include <stdbool.h>
#include "dictionary.h"

/* returns true if there are sufficent letters on the grid for a specific
 * word.  It does not check if the word is a valid play, simply if it's
 * possible for it to be so
 */
bool gniggle_solve_sufficent_letters(const unsigned char *word,
					const unsigned char *grid);

/* returns true if 'word' is on the grid represented by the string 'grid'.  The
 * grid string is a character per cube, left to right, top to bottom.  If you
 * also want to know the route of the found word, pass in a pointer to an
 * array of integers that has at least enough room for two integer per letter
 * in your word.  This will be filled in with co-ordinates, even numbered
 * entries being x, and odd being y.  If you're not interested, just pass NULL.
 */
bool gniggle_solve_word_on_grid(const unsigned char *word,
				const unsigned char *grid,
				const unsigned int width,
				const unsigned int height,
				unsigned int *path);
#endif /* __SOLVE_H__ */
