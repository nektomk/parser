#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "parser.h"
#include "type.h"
#include "debug.h"

char test_comment[]=
"# end-of-line comment \n\
";
char test[]="{}";
char test_numeric[]="10 20 0.20 1.2E3 21.93e-5";
char test_string[]="'hello' 'another string' 'string whith \\resc'";
char test_subst[]="\"hello\" \"another string\" \"string whith \\resc\"";
char test_subst2[]="\"$hello tram-pam-pam\" \"-> [ command arg1 arg2 ; cmd2 ] <-\"";
char test_list[]="{1\n\
}";
char test_var1[]="$name $(name)";
char text_setvar1[]="set name value";
char test_var2[]="$name(index) $(name index)";
char test_setvar2[]="set name index(index2) value";

char test_var3[]="$name(index)(index2) $name(index index2) $(name index index2)";
char test_var[]="$name $name(index) $name(index1 index2) $(name) $(name index)";
char test_tuple[]="$$varname $($varname index)";

char app_1[]="command arg1 arg2";

void parse_file(char *filename,Tag *tag) {
	char *text;
	struct stat st;
	size_t size;
	int fd;
	if (stat(filename,&st)!=0) {
		fprintf(stderr,"unable to stat %s\n",filename);
		exit(1);
	}
	fd=open(filename,O_RDONLY);
	if (fd<0) {
		fprintf(stderr,"unable to open %s for read\n",filename);
		exit(1);
	}
	text=malloc(st.st_size+1);
	if (text==NULL)
		exit(1);
	size=read(fd,text,st.st_size);
	if (size<0) {
		fprintf(stderr,"error reading from %s \n",filename);
		exit(1);
	}
	text[size]=0;
	parse_code(text,NULL,tag);
	close(fd);
}
static void
print_tag(Tag *tag,int level) {
	int t;
	if (!tag) return;
	for(t=0;t<level;t++) {
		putchar('\t');
	}
	if (tag->name) printf("%s:",tag->name);
	if (tag->text && tag->len) printf("\"%.*s\"",tag->len,tag->text);
	putchar('\n');
	if (tag->name && tag->sub) {
		for(tag=tag->sub;tag;tag=tag->next) {
			print_tag(tag,level+1);
		}
	}
}
static void
xml_text(char *text,size_t len) {
	size_t t;
	for(t=0;t<len && text[t] ;t++) {
		switch(text[t]) {
			case '&': printf("&amp;"); break;
			case '<': printf("&lt;");break;
			case '>': printf("&gt;");break;
			case '\n': putchar('\n');break;
			default : putchar(text[t]);
		}
	}
}
/* инверси€ односв€зного списка дочерних тегов */
static void
R(Tag *parent) {
	Tag *tag,*next;
	tag=parent->sub;
	parent->sub=NULL;
	while(tag) {
		next=tag->next;
		tag->next=parent->sub;
		parent->sub=tag;
		tag=next;
	}
}

static void
print_xml_tag(Tag *tag,int level) {
	if (!tag) return;
	if (tag->name) printf("<%s>",*(char **)tag->name);
	if (tag->text && tag->len) xml_text(tag->text,tag->len);
	if (tag->name && tag->sub) {
		Tag *child;
		R(tag);
		for(child=tag->sub;child;child=child->next) {
			print_xml_tag(child,level+1);
		}
	}
	if (tag->name) printf("</%s>",*(char **)tag->name);
}
static void
print_xml(Tag *tag,int line) {
	printf("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	printf("<?xml-stylesheet type=\"text/xsl\" href=\"lang.xsl\"?>\n");
	print_xml_tag(tag,line);
}
#include "str.h"
char *tag_text(Tag *tag,Str *str) {
	if (tag==NULL || tag->name==NULL) return str_s(str);
	for(tag=tag->sub;tag;tag=tag->next) {
		if (tag->text!=NULL) str_add(str,tag->text,tag->len);
		tag_text(tag,str);
	}
	return str_s(str);
}

Tag *cb(Tag *head) {
	Tag *arg;
	int t;
	Str *str;
	str=str_alloc();
	printf("\nCB Called!! with args:\n");
	for(arg=head,t=0;arg;arg=arg->next,t++) {
		str_clear(str);
		printf("%d:",t);
		if (arg->name!=NULL)
			printf(" %s",*(char **)arg->name);
		printf(" =%s",tag_text(arg,str));
		putchar('\n');
	}
	str_free(str);
	return head;
}
int main()
{
	Tag *code;
	char *end;
	code=new_tag(CODE,NULL,0);
	//parse_file("sample.txt",code);
	//parse_expression(test,&end,code);
	parse_expression(app_1,&end,code);
	//printf("PARSE DONE\n");
	print_xml(code,0);
	eval(code->sub,cb);
    return 0;
}
