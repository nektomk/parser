#ifndef TAG_H
#define TAG_H 1
#include <stdlib.h>
typedef struct Tag {
	void *text;
	size_t size;
	size_t len;

	char *name;
	struct Tag *next;
	struct Tag *sub;
} Tag;

Tag *new_tag(void *name,char *text,int len);
Tag *clone_tag(Tag *);

Tag *mark(void *name,char *from,char *to,Tag *toptag);

int mark_len(void *tokname,int len,char *text,char **endptr,Tag *toptag);
int mark_cmp(void *tokname,char *c,	char *text,char **endptr,Tag *toptag);
int mark_spn(void *tokname,char *c,char *text,char **endptr,Tag *toptag);
int mark_cspn(void *tokname,char *c,char *text,char **endptr,Tag *toptag);

int mark_ctype(void *tokname,int (*check)(int),char *text,char **endptr,Tag *toptag);

#endif // TAG_H

