/*
 * dictionary.h
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

#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <stdbool.h>

struct gniggle_dictionary;
struct gniggle_dictionary_iter;

/* returns true if a word can be considered a legal play should the letters
 * be available
 */
bool gniggle_dictionary_word_qualifies(const unsigned char *word, 
					const int maxlen);

/* create a new dictionary for a grid of x by y.  You can also select a hash
 * size for the number of buckets to use.  Using zero here uses a largish
 * default value
 */
struct gniggle_dictionary *gniggle_dictionary_new(const unsigned int x,
					const unsigned int y,
					const unsigned int hashsize);

/* deletes a dictionary from memory, including its hash table */
void gniggle_dictionary_delete(struct gniggle_dictionary *dict);

/* adds a word to a dictionary, making sure first that it isn't already there */
void gniggle_dictionary_add(struct gniggle_dictionary *dict,
				const unsigned char *word);
				
/* returns true if 'word' is in dictionary.  The word must be lower case. */
bool gniggle_dictionary_lookup(struct gniggle_dictionary *dict,
				const unsigned char *word);

/* return an interator structure to be passed to the next function.  It is
 * undefined what happens if you add to the dictionary while iterating it.
 */				
struct gniggle_dictionary_iter *gniggle_dictionary_iterator(
				struct gniggle_dictionary *dict);

/* returns the next word from the dictionary via an iterator, or NULL if there
 * are no more words.  Words are returned in hash order, and are unsorted.
 */				
const unsigned char *gniggle_dictionary_next(
				struct gniggle_dictionary_iter *iter);

/* deletes an iterator from memory once you are done with it */
void gniggle_dictionary_iterator_delete(struct gniggle_dictionary_iter *iter);

#endif /* __DICTIONARY_H__ */
