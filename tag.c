#include <stdlib.h>
#include <string.h>

#include "tag.h"

Tag *
new_tag(void *name,char *text,int len)
{
	Tag *tag;
	tag=malloc(sizeof(Tag));
	if (tag==NULL)
		exit(1);
	tag->name=name;
	tag->text=text;
	tag->len=len;
	tag->size=0;
	tag->next=NULL;
	tag->sub=NULL;
	return tag;
}
Tag *
clone_tag(Tag *orig) {
	Tag *tag;
	tag=malloc(sizeof(Tag));
	memset(tag,0,sizeof(Tag));
	if (orig->name)
		tag->name=orig->name;
	else {
		if (orig->text && orig->len) {
			tag->size=orig->len;
			tag->size+=32-(tag->size%32);
			tag->text=malloc(tag->size);
			memcpy(tag->text,orig->text,orig->len);
			tag->len=orig->len;
		}
	}
	tag->sub=orig->sub;	// weak ptr
	return tag;
}
inline Tag *
tail_tag(Tag *parent,Tag **ptail)
{
	Tag *tag;
	if (ptail && *ptail) return *ptail;
	if (!parent->sub) return NULL;
	for(tag=parent->sub;tag->next!=NULL;)
		tag=tag->next;
	return tag;
}

Tag *
add_text(Tag *tag,char *text,int len)
{
	char *tmp;
	int size;
	if (tag->text && tag->size==0 && tag->text+tag->len==text) {
		tag->len+=len;
		return tag;
	}
	size=tag->len+len;
	size+=32-(size%32);
	tmp=malloc(sizeof(size));
	if (tmp==NULL)
		exit(0);
	if (tag->text && tag->len)
		memcpy(tmp,tag->text,tag->len);
	memcpy(tmp+tag->len,text,len);
	if (tag->text && tag->size)
		free(tag->text);
	tag->text=tmp;
	tag->len+=len;
	tag->size=size;
	return tag;
}
/* разметить тег и поместить его ПЕРВЫМ в parent
*/
Tag *mark(void *name,char *text,char *end,Tag *parent) {
	Tag *prev;
	Tag *tag;
	int len;
	len=end-text;
	if (parent==NULL)
		return NULL;
	prev=parent->sub;
	if (prev==NULL) {
		tag=new_tag(name,text,len);
		parent->sub=tag;
	} else {
		if (prev->name==NULL && name==NULL) {
			/* предыдущий тег-текст и размечаемый тоже текст */
			/* просто конкетенация */
			add_text(prev,text,len);
			tag=prev;
		} else {
			tag=new_tag(name,text,len);
			tag->next=prev;
			parent->sub=tag;
		}
	}
	return tag;
}
int mark_len(void *tokname,int len,char *text,char **endptr,Tag *toptag) {
	if (!text || !len) return 1;
	if (toptag) mark(tokname,text,text+len,toptag);
	if (endptr) (*endptr)=text+len;
	return 0;
}
int mark_cmp(void *tokname,char *c,	char *text,char **endptr,Tag *toptag) {
	int len;
	if (!text) return 1;
	len=strlen(c);
	if (strncmp(c,text,len)!=0) return 1;
	if (toptag) mark(tokname,text,text+len,toptag);
	if (endptr) (*endptr)=text+len;
	return 0;
}
int mark_spn(void *tokname,char *c,char *text,char **endptr,Tag *toptag) {
	int len;
	if (!text) return 1;
	len=strspn(text,c);
	if (!len) return 1;
	if (toptag) mark(tokname,text,text+len,toptag);
	if (endptr) (*endptr)=text+len;
	return 0;
}
int mark_cspn(void *tokname,char *c,char *text,char **endptr,Tag *toptag) {
	int len;
	if (!text) return 1;
	len=strcspn(text,c);
	if (!len) return 1;
	if (toptag) mark(tokname,text,text+len,toptag);
	if (endptr) (*endptr)=text+len;
	return 0;
}
int mark_ctype(void *tokname,int (*check)(int),char *text,char **endptr,Tag *toptag) {
	int len;
	if (!text || check==NULL || !check(*text)) return 1;
	for(len=0;text[len] && check(text[len]);)
		len++;
	return mark_len(tokname,len,text,endptr,toptag);
}

