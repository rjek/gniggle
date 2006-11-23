/*
 * generate.h
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

#ifndef __GENERATE_H__
#define __GENERATE_H__

/* predefined letter distributions */

#define GNIGGLE_ALPHABET	"abcdefghijkmlnopqrstuvwxyz"
#define GNIGGLE_BOGGLE		"aaaaaabbcc" \
				"dddeeeeeee" \
				"eeeeffgghh" \
				"hhhiiiiiij" \
				"kllllmmnnn" \
				"nnnooooooo" \
				"ppqrrrrrss" \
				"sssstttttt" \
				"tttuuuvvww" \
				"wxyyyz"
				
#define GNIGGLE_SCRABBLE	"aaaaaaaaab" \
				"bccddddeee" \
				"eeeeeeeeef" \
				"fggghhiiii" \
				"iiiiijklll" \
				"lmmnnnnnno" \
				"oooooooppq" \
				"rrrrrrssss" \
				"ttttttuuuu" \
				"vvwwxyyz"	

/* generate a cube by selecting width * height letters at random from a
 * distribution string.  Some are predefined for convienance:
 *   GNIGGLE_ALPHABET: flat distribution.  all letters are as likely as others
 *   GNIGGLE_BOGGLE: distribution found in real Boggle.  Note that this can
 *                   still produce boards that are impossible in real Boggle.
 *   SNIGGLE_SCRABBLE: distribution found in Scrabble.
 */
unsigned char *gniggle_generate_simple(const unsigned char *distribution,
					unsigned int width,
					unsigned int height);

/* generate a cube by emulating a real set of Boggle dice.  width * heigh
 * must equal either 16 (for original Boggle) or 25 (for Boggle Deluxe), or
 * NULL will be returned.
 */
unsigned char *gniggle_generate_real(unsigned int width, unsigned int height);

#endif /* __GENERATE_H__ */
