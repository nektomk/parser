#ifndef STR_H
#define STR_H 1
typedef struct Str {
	char *s;
	int size;
	int len;
}Str;

Str *str_init(Str *);
Str *str_alloc();

char *str_s(Str *);
void str_add(Str *,char *,int);

void str_clear(Str *);
void str_free(Str *);

#endif // STR_H
