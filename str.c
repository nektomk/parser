#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "str.h"

Str *
str_init(Str *str) {
	assert(str!=NULL);
	memset(str,0,sizeof(Str));
	return str;
}

Str *
str_alloc(void) {
	Str *str;
	str=malloc(sizeof(Str));
	return str_init(str);
}

void
str_add(Str *str,char *s,int len) {
	assert(str!=NULL);
	if (s==NULL || s[0]==0) return;
	if (len<0)
		len=strlen(s);
	if (len==0)
		return;
	if (str->len+len+1<str->size) {
		char *tmp;
		int size;
		size=str->len+len+1;
		size+=32-(size%32);
		tmp=malloc(size);
		memcpy(tmp+str->len,s,len);
		if (str->s) {
			memcpy(tmp,str->s,str->len);
			free(str->s);
		}
		str->s=tmp;
		str->size=size;
		str->len+=len;
	} else {
		memcpy(str->s+str->len,s,len);
		str->len+=len;
	}
}
char *
str_s(Str *str) {
	assert(str!=NULL);
	if (str->s==NULL) return "";
	str->s[str->len]=0;
	return str->s;
}
void
str_clear(Str *str) {
	if (str->s) free(str->s);
	str->s=NULL;
	str->len=0;
	str->size=0;
}
void
str_free(Str *str) {
	if (str==NULL) return;
	if (str->s) free(str->s);
	free(str);
}
