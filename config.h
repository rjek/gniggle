/*
 * config.h
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
 
#ifndef __GNIGGLE_CONFIG_H__
#define __GNIGGLE_CONFIG_H__

#include <stdlib.h>
#include <unistd.h>

/*#ifdef _POSIX_VER */
/* TODO: make this work :) */
	#include <stdlib.h>
	#include <time.h>
	#define GNIGGLE_RAND_SEED srand(((unsigned int)time(NULL)))
	#define GNIGGLE_RAND(min,max) (rand() % ((max) + 1))
/*#endif*/

#ifndef GNIGGLE_RAND_SEED
	#error GNIGGLE_RAND_SEED not defined, unable to generate random games
#endif

#endif /* __GNIGGLE_CONFIG_H__ */
