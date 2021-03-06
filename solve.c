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
#include <stdio.h>
#include "dictionary.h"
#include "solve.h"

bool gniggle_solve_sufficent_letters(const char *word, const char *grid)
{
	char *letters = strdup(grid);
	size_t i;
	
	for (i = 0; i < strlen(word); i++) {
		char *o = strchr(letters, word[i]);
		if (o == NULL) {
			free(letters);
			return false;
		}
		*o = '-';
	}
	
	free(letters);	
	return true;
}

#define GRIDOFFSET(ROW, COL) ((ROW * width) + (COL))
#define GRID(ROW,COL) grid[((ROW) * width) + (COL)]

static bool gniggle_solve_look(const char *word, char *grid,
				unsigned int width, unsigned int height,
				unsigned int x, unsigned int y,
				unsigned int *path)
{
	char eaten;
	signed int sx, sy;
	
	if (word[0] == '\0')
		return true;

	eaten = GRID(x, y);
	GRID(x, y) = '-';	
	
	for (sx = -1; sx < 2; sx++) {
		for (sy = -1; sy < 2; sy++) {
			if ( (sx + x >= 0) && (y + sy >= 0) 
				&& (x + sx < width)
				&& (y + sy < height)
			   ) {
				if (GRID(x + sx, y + sy) == word[0]) {
					if (path != NULL) {
						path[0] = y + 1;
						path[1] = x + 1;
					}
					if (gniggle_solve_look(word + 1, grid,
						width, height,
						x + sx, y + sy,
						path ? path + 2 : NULL) ==
							true) {
						return true;
					}
				}
			}
		}
	}
	
	GRID(x, y) = eaten;
	
	return false;
}

bool gniggle_solve_word_on_grid(const char *word,
				const char *grid,
				const unsigned int width,
				const unsigned int height,
				unsigned int *path)
{
	char firstchar = word[0];
	char *wordc;
	char *gridc;
	unsigned int x, y;
	
	if (gniggle_solve_sufficent_letters(word, grid) == false)
		return false;
		
	wordc = strdup(word + 1);
	gridc = strdup(grid);
	
	for (x = 0; x < width; x++) {
		for (y = 0; y < height; y++) {
			if (GRID(x, y) == firstchar) {
				if (gniggle_solve_look(wordc, gridc,
					width, height, x, y, path) == true) {
					free(wordc);
					free(gridc);
					return true;	
				}
			}
		}
	}
	
	free(wordc);
	free(gridc);
	
	return false;
}

#ifdef TEST_RIG
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
 
	unsigned int width, height;
	unsigned char *grid;
	unsigned char word[BUFSIZ];
	
	width = (unsigned int)atoi(argv[1]);
	height = (unsigned int)atoi(argv[2]);
	grid = (unsigned char *)argv[3];
	
	while (scanf("%s", word) == 1) {
		bool found = gniggle_solve_word_on_grid(
			word, grid, width, height);
		if (found)
			printf("%s\n", word);
			
	}
	
	return 0;
}
#endif
