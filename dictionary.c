/*
 * dictionary.c
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
#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <ctype.h>
#include <zlib.h>
#include "dictionary.h"

struct gniggle_dictionary_hash_e {
	char *word;
	struct gniggle_dictionary_hash_e *next;
};

struct gniggle_dictionary {
	unsigned int nwords;		/* number of words loaded */
	unsigned int gx;		/* grid width */
	unsigned int gy;		/* grid height */
	unsigned int hashsize;		/* number of hash buckets */
	struct gniggle_dictionary_hash_e **hash;	/* hash table */
};

struct gniggle_dictionary_iter {
	struct gniggle_dictionary *dict;/* dictionary we're iterating */
	unsigned int bucket;		/* current bucket */
	struct gniggle_dictionary_hash_e *entry; /* last hash entry */	
};

char *gniggle_dictionary_trim_qu(const char *word)
{
	size_t i, len = strlen(word);
	char *r = calloc(len + 1, 1);
	char *p = r;
	
	for (i = 0; i < strlen(word); i++) {
		*p++ = word[i];
		if (word[i] == 'q')
			i++;
	}
	
	return r;
}

char *gniggle_dictionary_restore_qu(const char *word)
{
	size_t qs = 0, i;
	char *r;
	char *b;
	
	for (i = 0; i < strlen(word); i++)
		if (word[i] == 'q')
			qs++;
	
	if (qs == 0)
		return strdup(word);
	
	r = calloc(strlen(word) + qs + 1, 1);
	
	b = r;
	for (i = 0; i < strlen(word); i++) {
		*b = word[i];
		if (*b == 'q') {
			*(b + 1) = 'u';
			b++;
		}
		b++;
	}
	
	return r;
}

bool gniggle_dictionary_word_qualifies(const char *word, 
					const int maxlen)
{
	char *qu;
	int i, len = strlen(word);
	
	/* Word is at least three characters long */
	if (len < 3)
		return false;
		
	/* Word is entirely lower-case */
	for (i = 0; i < len; i++)
		if (isupper(word[i]))
			return false;
			
	/* If word contains a Q, it is followed by a U */
	for (i = 0; i < len; i++)
		if (word[i] == 'q' && word[i + 1] != 'u')
			return false;
			
	/* Word is made up only of letters (no punctuation) */
	for (i = 0; i < len; i++)
		if (isalpha(word[i]) == 0)
			return false;
	
	/* Convert qu to just q when considering other tests */
	qu = gniggle_dictionary_trim_qu(word);
	len = strlen(qu);
	free(qu);
		
	/* Word is not longer than the maximum length (which is the total
	 * number of letters on the board, if by some marvel they can all be
	 * be used to spell a word
	 */
	if (len > maxlen)
		return false;
	
	return true;	
}

unsigned int gniggle_dictionary_fnv(const char *word)
{
	unsigned int z = 0x01000193;

	if (word == NULL)
		return 0;

	while (*word) {
		z *= 0x01000193;
		z ^= *word++;
	}

	return z;
}

struct gniggle_dictionary *gniggle_dictionary_new(const unsigned int x,
					const unsigned int y,
					const unsigned int hashsize)
{
	struct gniggle_dictionary *r = calloc(sizeof(struct gniggle_dictionary),
						1);
	r->gx = x;
	r->gy = y;
	r->hashsize = (hashsize == 0) ? 7919 : hashsize;
	r->hash = calloc(sizeof(struct gniggle_dictionary_hash_e **),
			r->hashsize);
	
	return r;	
}

void gniggle_dictionary_delete(struct gniggle_dictionary *dict)
{
	unsigned int i;
	
	for (i = 0; i < dict->hashsize; i++) {
		struct gniggle_dictionary_hash_e *e = dict->hash[i];
		while (e != NULL) {
			struct gniggle_dictionary_hash_e *n = e;
			free(e->word);
			e = e->next;
			free(n);
		}	
	}
	
	free(dict->hash);
	free(dict);
}

void gniggle_dictionary_add(struct gniggle_dictionary *dict,
				const char *word)
{
	char *nqu;
	
	if (gniggle_dictionary_word_qualifies(word, dict->gx * dict->gy)
		== false)
		return;
		
	nqu = gniggle_dictionary_trim_qu(word);
	
	if (gniggle_dictionary_lookup(dict, nqu) == false) {
		struct gniggle_dictionary_hash_e *e = calloc(
				sizeof(struct gniggle_dictionary_hash_e), 1);
		unsigned int hash = gniggle_dictionary_fnv(nqu);
		unsigned int bucket = hash % (dict->hashsize);
		
		e->word = nqu;
		e->next = dict->hash[bucket];
		dict->hash[bucket] = e;
		dict->nwords++;
	} else {
		free(nqu);
	}
}
				
bool gniggle_dictionary_lookup(struct gniggle_dictionary *dict,
				const char *word)
{
	unsigned int hash = gniggle_dictionary_fnv(word);
	unsigned int bucket = hash % (dict->hashsize);
	
	struct gniggle_dictionary_hash_e *e = dict->hash[bucket];
	
	while (e != NULL) {
		if (strcmp(word, e->word) == 0)
			return true;
		e = e->next;
	}
	
	return false;
}

unsigned int gniggle_dictionary_size(struct gniggle_dictionary *dict)
{
	return dict->nwords;
}
	
struct gniggle_dictionary_iter *gniggle_dictionary_iterator(
				struct gniggle_dictionary *dict)
{
	struct gniggle_dictionary_iter *r = calloc(
				sizeof(struct gniggle_dictionary_iter), 1);
	
	r->dict = dict;
	r->bucket = 0;
	r->entry = dict->hash[0];
	
	return r;
}
			
const char *gniggle_dictionary_next(struct gniggle_dictionary_iter *iter)
{
	const char *r;
	while (iter->entry == NULL) {
		iter->bucket += 1;
		if (iter->bucket == iter->dict->hashsize)
			return NULL;
		iter->entry = iter->dict->hash[iter->bucket];
	}
	
	r = iter->entry->word;
	iter->entry = iter->entry->next;
	return r;
}

void gniggle_dictionary_iterator_delete(struct gniggle_dictionary_iter *iter)
{
	free(iter);
}

int gniggle_dictionary_dump(struct gniggle_dictionary *dict,
    					const char *filename)
{
  	gzFile fh = gzopen(filename, "wb");
#	define WRITE(d, s) gzwrite(fh, (d), (s))
	size_t l;
	uint32_t magic = 0x12345678;
	unsigned char fl;

	if (fh == NULL)
		return -1;

	gzsetparams(fh, Z_BEST_COMPRESSION, Z_DEFAULT_STRATEGY);

	WRITE("GNIGDICT", 8); /* identifier */
	WRITE(&magic, sizeof(magic)); /* endian detection word */

	fl = sizeof(unsigned char);
	WRITE(&fl, sizeof(fl)); /* size of unsigned char */
	
	fl = sizeof(unsigned short);
	WRITE(&fl, sizeof(fl)); /* size of short */

	WRITE(&dict->nwords, sizeof(dict->nwords));
	WRITE(&dict->hashsize, sizeof(dict->hashsize));
	
	for (l = 0; l < dict->hashsize; l++) {
		unsigned short chainsize = 0; 
		struct gniggle_dictionary_hash_e *e = dict->hash[l];

		while (e != NULL) {
			chainsize++;
			e = e->next;
		}

		WRITE(&chainsize, sizeof(chainsize));

		if (chainsize != 0) {
			unsigned char sl;
		  	e = dict->hash[l];
			while (e != NULL) {
				sl = strlen(e->word);
				WRITE(&sl, sizeof(sl));
				WRITE(e->word, sl);
				e = e->next;
			}

		}
		
	}
	gzclose(fh);
#undef WRITE

	return 0;
}

struct gniggle_dictionary *gniggle_dictionary_undump(const unsigned x,
    					const unsigned y, const char *filename)
{
	gzFile fh = gzopen(filename, "rb");
#	define READ(p, s) gzread(fh, (p), (s))
	uint32_t magic;
	unsigned char fl;
	char head[8];
	size_t l;

	struct gniggle_dictionary *d;

	if (fh == NULL)
		return NULL;

	READ(head, 8);
	if (strncmp(head, "GNIGDICT", 8) != 0) {
		gzclose(fh);
		return NULL;
	}

	READ(&magic, sizeof(uint32_t));
	if (magic != 0x12345678) {
		gzclose(fh);
		return NULL;
	}

	READ(&fl, sizeof(fl));
	if (fl != sizeof(unsigned char)) {
		gzclose(fh);
		return NULL;
	}

	READ(&fl, sizeof(fl));
	if (fl != sizeof(unsigned short)) {
	 	gzclose(fh);
		return NULL;
	}

	d = calloc(sizeof(struct gniggle_dictionary), 1);

	READ(&d->nwords, sizeof(d->nwords));
	READ(&d->hashsize, sizeof(d->hashsize));

	d->gx = x;
	d->gy = y;


	d->hash = calloc(sizeof(struct gniggle_dictionary_hash_e **),
	    			d->hashsize);

	for (l = 0; l < d->hashsize; l++) {
		unsigned short chainsize;
		int w;
		READ(&chainsize, sizeof(chainsize));
		for (w = 0; w < chainsize; w++) {
			struct gniggle_dictionary_hash_e *e;
			unsigned char wl;
			e = calloc(sizeof(
				struct gniggle_dictionary_hash_e), 1);
			READ(&wl, sizeof(wl));
			e->word = calloc(wl + 1, 1);
			READ(e->word, wl);
			e->next = d->hash[l];
			d->hash[l] = e;
		}
	}
	gzclose(fh);
#undef READ
	return d;
}

struct gniggle_dictionary *gniggle_dictionary_new_from_file(
					const unsigned int x,
					const unsigned int y,
					const unsigned int hashsize,
					const char *filename)
{
	FILE *fh = fopen(filename, "rb");
	unsigned char head[2];

	if (fh == NULL)
		return NULL;

	if (fread(head, 2, 1, fh) < 1) {
		fclose(fh);
		return NULL;
	}
	fclose(fh);

	if (head[0] == 0x1f && head[1] == 0x8b) {	/* gzip header? */
		return gniggle_dictionary_undump(x, y, filename);
	} else {
		struct gniggle_dictionary *d;
		d = gniggle_dictionary_new(x, y, hashsize);
		gniggle_dictionary_load_file(d, filename);
		return d;
	}
	
	return NULL;
}


int gniggle_dictionary_load_file(struct gniggle_dictionary *dict,
					const char *filename)
{
	FILE *fh = fopen(filename, "r");
	char word[BUFSIZ];
	int count = 0;
	
	if (fh == NULL)
		return -1;
	
	while (!feof(fh)) {
		if (fscanf(fh, "%s", word) < 1)
			break;
		gniggle_dictionary_add(dict, word);
		count++;
	}
	
	fclose(fh);
	
	return count;
}

#ifdef TEST_RIG
#include <stdio.h>

int main(int argc, char *argv[])
{
	struct gniggle_dictionary *dict = gniggle_dictionary_new(10, 10, 0);
	struct gniggle_dictionary_iter *iter;
	unsigned char word[BUFSIZ];
	const char *iterword;
	
	FILE *d = fopen("./word.list", "r");
	
	while (!feof(d)) {
		fscanf(d, "%s", word);
		gniggle_dictionary_add(dict, word);
	}
	
	fclose(d);
	
	iter = gniggle_dictionary_iterator(dict);
	
	do {
		iterword = gniggle_dictionary_next(iter);
		if (iterword != NULL) {
			printf("%s\n", iterword);
		}
	} while (iterword != NULL);
	
	gniggle_dictionary_iterator_delete(iter);
	gniggle_dictionary_delete(dict);
	
	return 0;
}
#endif
