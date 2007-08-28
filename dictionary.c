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
#include "dictionary.h"

struct gniggle_dictionary_hash_e {
	unsigned char *word;
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

inline unsigned char *gniggle_dictionary_trim_qu(const unsigned char *word)
{
	int i, len = strlen(word);
	unsigned char *r = calloc(len + 1, 1);
	unsigned char *p = r;
	
	for (i = 0; i < strlen(word); i++) {
		*p++ = word[i];
		if (word[i] == 'q')
			i++;
	}
	
	return r;
}

inline unsigned char *gniggle_dictionary_restore_qu(const unsigned char *word)
{
	int qs = 0, i;
	unsigned char *r;
	unsigned char *b;
	
	for (i = 0; i < strlen((char *)word); i++)
		if (word[i] == 'q')
			qs++;
	
	if (qs == 0)
		return strdup((char *)word);
	
	r = calloc(strlen((char *)word) + qs + 1, 1);
	
	b = r;
	for (i = 0; i < strlen((char *)word); i++) {
		*b = word[i];
		if (*b == 'q') {
			*(b + 1) = 'u';
			b++;
		}
		b++;
	}
	
	return r;
}

bool gniggle_dictionary_word_qualifies(const unsigned char *word, 
					const int maxlen)
{
	unsigned char *qu;
	int i, len = strlen((char *)word);
	
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

static inline unsigned int gniggle_dictionary_fnv(const unsigned char *word)
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
				const unsigned char *word)
{
	unsigned char *nqu;
	
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
				const unsigned char *word)
{
	unsigned int hash = gniggle_dictionary_fnv(word);
	unsigned int bucket = hash % (dict->hashsize);
	
	struct gniggle_dictionary_hash_e *e = dict->hash[bucket];
	
	while (e != NULL) {
		if (strcmp((char *)word, (char *)e->word) == 0)
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
			
const unsigned char *gniggle_dictionary_next(
				struct gniggle_dictionary_iter *iter)
{
	const unsigned char *r;
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
	FILE *fh = fopen(filename, "wb");
	int l;
	uint32_t magic = 0x12345678;
	unsigned char fl;
	int testhash;

	if (fh == NULL)
		return -1;

	fwrite("GNIGDICT", 8, 1, fh); /* identifier */
	fwrite(&magic, sizeof(magic), 1, fh); /* endian detection word */

	testhash = gniggle_dictionary_fnv("GNIGGLE v1");
	fwrite(&testhash, sizeof(testhash), 1, fh); /* same-hash check */

	fl = sizeof(unsigned char);
	fwrite(&fl, sizeof(fl), 1, fh); /* size of unsigned char */
	
	fl = sizeof(unsigned short);
	fwrite(&fl, sizeof(fl), 1, fh); /* size of short */

	fwrite(&dict->nwords, sizeof(dict->nwords), 1, fh);
	fwrite(&dict->hashsize, sizeof(dict->hashsize), 1, fh);
	
	for (l = 0; l < dict->hashsize; l++) {
		unsigned short chainsize = 0; 
		struct gniggle_dictionary_hash_e *e = dict->hash[l];

		while (e != NULL) {
			chainsize++;
			e = e->next;
		}

		fwrite(&chainsize, sizeof(chainsize), 1, fh);

		if (chainsize != 0) {
			unsigned char sl;
		  	e = dict->hash[l];
			while (e != NULL) {
				sl = strlen(e->word);
				fwrite(&sl, sizeof(sl), 1, fh);
				fwrite(e->word, sl, 1, fh);
				e = e->next;
			}

		}
		
	}

	fclose(fh);
}

struct gniggle_dictionary *gniggle_dictionary_undump(const unsigned x,
    					const unsigned y, const char *filename)
{
	FILE *fh = fopen(filename, "rb");
	uint32_t magic;
	unsigned char fl;
	unsigned char head[8];
	int l, testhash;

	struct gniggle_dictionary *d;

	if (fh == NULL)
		return NULL;

	fread(head, 8, 1, fh);
	if (strncmp(head, "GNIGDICT", 8) != 0) {
		fclose(fh);
		return NULL;
	}

	fread(&testhash, sizeof(testhash), 1, fh);
	if (gniggle_dictionary_fnv("GNIGGLE v1") != testhash) {
		fclose(fh);
		return NULL;
	}

	fread(&magic, sizeof(uint32_t), 1, fh);
	if (magic != 0x12345678) {
		fclose(fh);
		return NULL;
	}

	fread(&fl, sizeof(fl), 1, fh);
	if (fl != sizeof(unsigned char)) {
		fclose(fh);
		return NULL;
	}

	fread(&fl, sizeof(fl), 1, fh);
	if (fl != sizeof(unsigned short)) {
	 	fclose(fh);
		return NULL;
	}

	d = calloc(sizeof(struct gniggle_dictionary), 1);

	fread(&d->nwords, sizeof(d->nwords), 1, fh);
	fread(&d->hashsize, sizeof(d->hashsize), 1, fh);

	d->gx = x;
	d->gy = y;


	d->hash = calloc(sizeof(struct gniggle_dictionary_hash_e **),
	    			d->hashsize);

	for (l = 0; l < d->hashsize; l++) {
		unsigned short chainsize;
		int w;
		fread(&chainsize, sizeof(chainsize), 1, fh);
		for (w = 0; w < chainsize; w++) {
			struct gniggle_dictionary_hash_e *e;
			unsigned char wl;
			e = calloc(sizeof(
				struct gniggle_dictionary_hash_e), 1);
			fread(&wl, sizeof(wl), 1, fh);
			e->word = calloc(wl + 1, 1);
			fread(e->word, wl, 1, fh);
			e->next = d->hash[l];
			d->hash[l] = e;
		}
	}

	fclose(fh);
	return d;
}

struct gniggle_dictionary *gniggle_dictionary_new_from_file(
					const unsigned int x,
					const unsigned int y,
					const unsigned int hashsize,
					const char *filename)
{
	FILE *fh = fopen(filename, "rb");
	unsigned char head[8];
	uint32_t magic;

	if (fh == NULL)
		return NULL;

	fread(head, 8, 1, fh);
	fread(&magic, sizeof(magic), 1, fh);

	if ((strncmp(head, "GNIGDICT", 8) == 0) && magic == 0x12345678) {
	  	fclose(fh);
		return gniggle_dictionary_undump(x, y, filename);
	} else {
		struct gniggle_dictionary *d;
		fclose(fh);
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
	unsigned char word[BUFSIZ];
	int count = 0;
	
	if (fh == NULL)
		return -1;
	
	while (!feof(fh)) {
		fscanf(fh, "%s", word);
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
	const unsigned char *iterword;
	
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
