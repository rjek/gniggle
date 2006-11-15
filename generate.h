/*
 * generate.h
 * This file is part of Gniggle
 *
 * Copyright (C) 2006 - Rob Kendrick <rjek@rjek.com
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

/* generate a cube by emulating a real set of Boggle dice.  width * height
 * must equal 16, as there are only 16 dice.
 */
unsigned char *gniggle_generate_real(unsigned int width, unsigned int height);

/* letter arrangements for real Boggle cubes
 *
 *    1. A E A N E G      9. W N G E E H
 *    2. A H S P C O     10. L N H N R Z
 *    3. A S P F F K     11. T S T I Y D
 *    4. O B J O A B     12. O W T O A T
 *    5. I O T M U C     13. E R T T Y L
 *    6. R Y V D E L     14. T O E S S I
 *    7. L R E I X D     15. T E R W H V
 *    8. E I U N E S     16. N U I H M Qu
 */
 
#define GNIGGLE_CUBE1 "aeaneg"
#define GNIGGLE_CUBE2 "ahspco"
#define GNIGGLE_CUBE3 "aspffk"
#define GNIGGLE_CUBE4 "objoab"
#define GNIGGLE_CUBE5 "iotmuc"
#define GNIGGLE_CUBE6 "ryvdel"
#define GNIGGLE_CUBE7 "lreixd"
#define GNIGGLE_CUBE8 "eiunes"
#define GNIGGLE_CUBE9 "wngeeh"
#define GNIGGLE_CUBE10 "lnhnrz"
#define GNIGGLE_CUBE11 "tstiyd"
#define GNIGGLE_CUBE12 "owtoat"
#define GNIGGLE_CUBE13 "erttyl"
#define GNIGGLE_CUBE15 "terwhv"
#define GNIGGLE_CUBE16 "nuihmq"

#endif /* __GENERATE_H__ */
