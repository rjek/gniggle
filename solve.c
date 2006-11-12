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
 
#include <stdbool.h>
#include <string.h>
#include <malloc.h>
#include "dictionary.h"
#include "solve.h"

bool gniggle_solve_sufficent_letters(struct gniggle_dictionary *dict,
					const unsigned char *word,
					const unsigned char *grid)
{
	unsigned char *letters = (unsigned char *)strdup((char *)grid);
	int i;
	
	for (i = 0; i < strlen((char *)word); i++) {
		char *o = strchr((char *)letters, word[i]);
		if (o == NULL) {
			free(letters);
			return false;
		}
		*o = '-';
	}
	
	free(letters);
	
	return true;
}
