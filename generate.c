/*
 * generate.c
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
 
#include "config.h"
#include "generate.h"
#include <stdbool.h>
#include <string.h>

unsigned char *gniggle_generate_simple(const unsigned char *distribution,
					unsigned int width,
					unsigned int height)
{
	unsigned char *r = calloc((width * height) + 1, 1);
	unsigned int i, l = (unsigned int)strlen(distribution) - 1;
	
	GNIGGLE_RAND_SEED;
	
	for (i = 0; i < (width * height); i++) {
		r[i] = distribution[GNIGGLE_RAND(0, l)];
	}
	
	return r;
}

static char *gniggle_boggle_cubes[] = {
	"aeaneg", "ahspco", "aspffk", "objoab", 
	"iotmuc", "ryvdel", "lreixd", "eiunes", 
	"wngeeh", "lnhnrz", "tstiyd", "owtoat",
	"erttyl", "toessi", "terwhv", "nuihmq"
};

unsigned char *gniggle_generate_real(unsigned int width, unsigned int height)
{
	unsigned char *r;
	int i;
	bool used[16];
	
	if ((width * height) != 16)
		return NULL;

	r = calloc(17, 1);
	
	for (i = 0; i < 16; i++)
		used[i] = false;		
	
	GNIGGLE_RAND_SEED;
	
	for (i = 0; i < 16; i++) {
		int c;
		do {
			c = GNIGGLE_RAND(0, 15);
		} while (used[c] == true);
		
		used[c] = true;
		
		r[i] = gniggle_boggle_cubes[c][GNIGGLE_RAND(0, 5)];
	}
	
	return r;
}

#ifdef TEST_RIG
#include <stdio.h>
int main(int argc, char *argv[])
{
	printf("%s\n", gniggle_generate_simple(GNIGGLE_ALPHABET, 4, 4));
	printf("%s\n", gniggle_generate_simple(GNIGGLE_BOGGLE, 4, 4));
	printf("%s\n", gniggle_generate_simple(GNIGGLE_SCRABBLE, 4, 4));
	printf("%s\n", gniggle_generate_real(4, 4));
	return 0;
}
#endif
