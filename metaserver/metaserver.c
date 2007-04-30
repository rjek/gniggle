/*
 * metserver.c
 * This file is part of Gniggle
 *
 * Copyright (C) 2007 - Rob Kendrick <rjek@rjek.com>
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

#define __USE_XOPEN

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <netdb.h>
#include <netinet/in.h>

#define METASERVER_PORT 14547
#define METASERVER_MAX_CLIENTS 32
#define METASERVER_MAX_AGE (5*60)

struct gniggle_server {
	char host[256];
	unsigned int port;
	unsigned int width, height;
	char comment[256];
	time_t last_seen;
	struct gniggle_server *next;
	struct gniggle_server *prev;
};

struct gniggle_server *server_list;

void gniggle_server_add(const char *host, const unsigned int port,
			const unsigned int width, const unsigned int height,
			const char *comment)
{
	struct gniggle_server *s = calloc(sizeof(struct gniggle_server), 1);
	
	strncpy(s->host, host, 255);
	s->port = port;
	
	s->width = width;
	s->height = height;
	
	strncpy(s->comment, comment, 255);
	
	s->last_seen = time(NULL);
	
	if (server_list != NULL)
		server_list->prev = s;
	
	s->next = server_list;
	server_list = s;
}

void gniggle_server_remove(const char *host, const unsigned int port)
{
	struct gniggle_server *c = server_list;
	
	while (c != NULL) {
		if (strncmp(c->host, host, 255) == 0 && c->port == port)
		{			
			c->prev->next = c->next;
			c->next->prev = c->prev;
			free(c);
			
			return;
		}
		
		c = c->next;
	}
}

int gniggle_server_expire(time_t threshold)
{
	struct gniggle_server *c = server_list;
	time_t t = time(NULL) - threshold;
	int expired = 0;
		
	while (c != NULL) {
		if (c->last_seen < t) {
			struct gniggle_server *f = c->next;
			
			c->prev->next = c->next;
			c->next->prev = c->prev;
			free(c);
						
			expired++;
			c = f;
		} else {
			c = c->next;
		}
	}
	
	return expired;
}

static char *url_encode(const char *input)
{
	char *r, buf[4];
	const char *x = input;
	int l = 0;
	
	while (*x != '\0') {
		if (isascii(*x) == 0 || (isalpha(*x) == 0 && isdigit(*x) == 0))
			l += 3;
		else
			l++;
		x++;
	}
	
	r = calloc(l + 1, 1);
	x = input;
	
	while (*x != '\0') {
		if (isascii(*x) == 0 ||
			(isalpha(*x) == 0 && isdigit(*x) == 0)) {
			snprintf(buf, 4, "%%%02x", *x);
			strcat(r, buf);
		}
		else {
			strncat(r, x, 1);
		}
		x++;
	}
	
	return r;
}

void gniggle_server_send_list(int fd)
{
	char buf[2048];
	struct gniggle_server *c = server_list;
	
	while (c != NULL) {
		char *comment = url_encode(c->comment);
		int len = snprintf(buf, 2047, "%s:%d:%d:%d:%s\n",
					c->host, c->port, c->width, c->height,
					comment);
		free(comment);
		write(fd, buf, len);
		
		c = c->next;
	}
}

struct connection {
	int fd;
	char inbuf[2048];
	int inbuflen;
	time_t activity;
};

void server(int serv)
{
	struct connection *conn = calloc(sizeof(struct connection),
						METASERVER_MAX_CLIENTS);

	struct pollfd *fds = calloc(sizeof(struct pollfd), 
					METASERVER_MAX_CLIENTS + 1);
	nfds_t nfds;
	int i;
	
	for (i = 0; i < METASERVER_MAX_CLIENTS; i++) {
		conn[i].fd = -1;
	}
	
	fds[0].fd = serv;
	fds[0].events = POLLIN;
	
	while (true) {
		nfds = 1;
		for (i = 0; i < METASERVER_MAX_CLIENTS; i++) {
			if (conn[i].fd != -1) {
				fds[nfds].fd = conn[i].fd;
				fds[nfds].events = POLLIN;
				nfds++;
			}
		}
		if (poll(fds, nfds, 5000) > 0) {
			if ((fds[0].revents & POLLIN) != 0) {
				/* new connection */
				int nc = accept(serv, NULL, NULL);
				for (i = 0; i < METASERVER_MAX_CLIENTS; i++) {
					if (conn[i].fd == -1) {
						conn[i].fd = nc;
						conn[i].inbuflen = 0;
						conn[i].activity = time(NULL);
						gniggle_server_send_list(nc);
						break;
					}
				}
				fds[0].revents = 0;
			}
		}
	}
}

int main(int argc, char *argv[])
{
	int one = 1;
	int sock = socket(PF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;

        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
        addr.sin_family = AF_INET;
	addr.sin_port = htons(METASERVER_PORT);
	addr.sin_addr.s_addr = 0;
 	
	if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
		fprintf(stderr, "%s: %s while binding\n", argv[0],
			strerror(errno));
		close(sock);
		exit(errno);
	}
	
	if (listen(sock, 128) == -1) {
		close(sock);
		fprintf(stderr, "%s: %s while trying to listen.\n", argv[0],
			strerror(errno));
 		exit(errno);
	}

	gniggle_server_add("gniggle.rjek.com", 1234, 4, 4, "Rob's gniggle server");
	gniggle_server_add("gniggle.geah.org", 1234, 5, 5, "Lesley's gniggle server");

	server(sock);
		
	return 0;
}
