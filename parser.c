#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>

#include "tag.h"
#include "type.h"

#include "debug.h"

#define SPACE " \t\v\a\b"
#define BLANK SPACE "\r\n"
#define EXPDELIM ";\n"
#define EXPTERM ";\n)}]"
#define BRACES "[]{}()"
#define SPECIAL SPACE EXPDELIM BRACES

/** считывание комментария до конца строки
*/
int parse_comment(char *text,char **endptr,Tag *tag) {
	char *s;
	s=text+strcspn(text,"\n");
	//if (s[0]=='\n') s++;
	if (endptr) (*endptr)=s;
	if (tag) mark(COMMENT,text,s,tag);
	return 0;
}

/** просто последовательность рядовых символов
**/
int
parse_chars(char *text,char **endptr,Tag *tag) {
	char *s;
	TRACE("");
	s=text+strcspn(text,SPECIAL BLANK);
	if (endptr) (*endptr)=s;
	if (tag) mark(CHARS,text,s,tag);
	return 0;
}

/** разбор esc-последовательности
	(маскированные символы)
	\\ \$ \[ \] \{ \} \( \) \' \" - спец.символы языка
	\a \b \e \r \n \t \v	- спец.символы а-ля C
	\x[0-9a-z]{1,2}		- байт в шестнадцатиричном виде
	\0[0-7]{1,4}		- байт в восьмиричном представлении
	\[1-9][0-9]{1,2}	- байт в десятичном представлении
	\&name; - именованный символ unicode или ASCII
*/
int
parse_escape(char *text,char **endptr,Tag *tag) {
	char *s;
	unsigned long byte;
	int len;
	if (text[0]!='\\') return 0;
	s=text+1;
	switch(*s) {
		case '\\' : s++; break;
		case '$'  : s++; break;
		case '['  : s++; break;
		case ']'  : s++; break;
		case '{'  : s++; break;
		case '}'  : s++; break;
		case '('  : s++; break;
		case ')'  : s++; break;
		case '\'' : s++; break;
		case '\"' : s++; break;
		case '0' :
		case 'o' :
		case 'O' : /* octal */
			byte=strtoul(s+1,&s,8);
			if (errno!=0 || byte<0 || byte>0xff) {
			}
			break;
		case 'x':
		case 'X':	/* hex */
			byte=strtoul(s+1,&s,16);
			if (errno!=0 || byte<0 || byte>0xff) {
			}
			break;
		case '1' ... '9': /* decimal */
			byte=strtoul(s+1,&s,10);
			if (errno!=0 || byte<0 || byte>0xff) {
			}
			break;
		case '&': /* named entity */
			len=strcspn(s+1,";\'");
			if (len) s+=len+1;
			if (';'==*s)s++;
			break;
		case 'u':
		case 'U': /* unicode notation */
			break;
		default:
			s++;
	}
	if (endptr) (*endptr)=s;
	if (tag) mark(ESC,text,s,tag);
	return 0;
}

/** числовая константа
*/
int
parse_numeric(char *text,char **endptr,Tag *tag) {
	long long ll;
	long double ld;
	char *tagname,*s;
	char *ll_fin,*ld_fin;
	/* конверсия в целое */
	ll=strtoll(text,&ll_fin,0);
	if (errno==ERANGE) return 1;	// не числовая константа
	/* конверсия в дробное */
	ld=strtold(text,&ld_fin);
	if (errno!=ERANGE && ld_fin>ll_fin) {
		// дробное число
		s=ld_fin;
		tagname=DOUBLE;
	} else {
		// целое число
		s=ll_fin;
		tagname=LONG;
	}
	if (endptr) (*endptr)=s;
	if (tag) mark(tagname,text,s,tag);
	return 0;
}

/** строка с подстановками
*/
int
parse_subst(char *text,char **endptr,Tag *toptag) {
	char *p;
	char *s;
	Tag *tag;
	tag=NULL;
	if (text[0]!='\"') return 1;
	s=text;
	if (toptag) tag=mark(STRING,text,s,toptag);
	mark_len(NULL,1,s,&s,tag);	// открывающая кавычка
	while(*s) {
		p=s;
		s+=strcspn(s,"\\$[{\"");
		if (p!=s) mark(CHARS,p,s,tag);
		switch (*s) {
			case '\\' : parse_escape(s,&s,tag);break;
			case '$'  : parse_variable(s,&s,tag);break;
			case '['  : parse_command(s,&s,tag);break;
		}
		if (s[0]=='\"') {
			mark_len(NULL,1,s,&s,tag);	// закрывающая кавычка
			break;
		}
		if (p==s) break;
	}
	if (endptr) (*endptr)=s;
	return 0;
}
/** строка без подстановок
*/
int
parse_string(char *text,char **endptr,Tag *toptag) {
	char *s,*p;
	Tag *tag;
	TRACE("");
	tag=NULL;
	if (text[0]!='\'') return 1;
	s=text;
	if (toptag) tag=mark(STRING,text,text,toptag);
	mark_len(NULL,1,s,&s,tag);	// открывающая кавычка
	while(*s) {
		p=s;
		mark_cspn(CHARS,"\'\\",s,&s,tag);		// фрагмент до кавычки ' или esc
		if (s[0]=='\\') parse_escape(s,&s,tag);	// esc
		if (s[0]=='\'') {
			mark_len(NULL,1,s,&s,tag);	// закрывающая кавычка
			break;
		}
		if (p==s) break;
	}
	if (endptr) (*endptr)=s;
	return 0;
}
/** текст выводимый на консоль
**/
int
parse_print(char *text,char **endptr,Tag *toptag) {
	char *s,*p;
	Tag *tag;
	TRACE("");
	tag=NULL;
	if (text[0]!='#'||text[1]!='?'||text[2]!='>') return 1;
	s=text;
	if (toptag) tag=mark(PRINT,text,text,toptag);
	mark_len(NULL,3,s,&s,tag);	// открывающая группа #?>
	while(*s) {
		p=s;
		mark_cspn(CHARS,"<$[\\",s,&s,tag);		// фрагмент до кавычки ' или esc
		if (s[0]=='\\') parse_escape(s,&s,tag);	// esc
		if (s[0]=='$') parse_variable(s,&s,tag);// подстановка переменных
		if (s[0]=='[') parse_command(s,&s,tag); // полстановка команд
		if (s[0]=='<') {
			if (s[1]=='?' && s[2]=='#') {
				mark_len(NULL,3,s,&s,tag);	// закрывающая группа <?#
				break;
			} else {
				mark_len(NULL,1,s,&s,tag);
			}
		}
		if (p==s) break;
	}
	if (endptr) (*endptr)=s;
	return 0;
}

/* список :
	слова разделяемые пробелами и запятыми заключенные в {}
*/
int
parse_list(char *text,char **endptr,Tag *toptag) {
	char *s,*p;
	Tag *tag;
	tag=NULL;
	if (text[0]!='{') return 1;
	s=text;
	if (toptag) tag=mark(LIST,s,s,toptag);
	mark_len(NULL,1,s,&s,tag);	// открывающая {
	while(*s) {
		p=s;
		mark_ctype(NULL,isspace,s,&s,tag);	// лидирующие/разделяющие пробелы
		parse_expression(s,&s,tag);		// слово
		if (s[0]=='}') {
			mark_len(NULL,1,s,&s,tag);	// закрывающая }
			break;
		}
		if (strchr(")]",*s)!=NULL) {
			break;
		}
		if (p==s) {
			break;
		}
	}
	if (endptr) (*endptr)=s;
	return 0;
}
/* кортеж - аналогичен списку, но не создаёт собственного синтаксического узла
*/
int
parse_tuple(char *text,char **endptr,Tag *toptag) {
	char *s,*p;
	Tag *tag;
	tag=NULL;
	if (text[0]!='(') return 1;
	s=text;
	if (toptag) tag=toptag;
	mark_len(NULL,1,s,&s,tag);	// открывающая {
	while(*s) {
		p=s;
		mark_ctype(NULL,isblank,s,&s,tag);	// лидирующие/разделяющие пробелы
		if ('('==*s)
			parse_tuple(s,&s,tag);
		else
			parse_word(s,&s,tag);		// слово
		if (s[0]==')') {
			mark_len(NULL,1,s,&s,tag);	// закрывающая }
			break;
		}
		if (strchr(EXPTERM,*s)!=NULL) break;
		if (p==s) break;
	}
	if (endptr) (*endptr)=s;
	return 0;
}

/* вызов процедуры
	список выражений (expression) разделённых \n ;
*/
int parse_command(char *text,char **endptr,Tag *toptag) {
	char *s,*p;
	Tag *tag;
	tag=NULL;
	s=text;
	if (text[0]!='[') return 1;
	if (toptag) tag=mark(COMMAND,s,s,toptag);
	mark_len(NULL,1,s,&s,tag);
	while(*s) {
		p=s;
		mark_ctype(NULL,isblank,s,&s,tag);
		parse_expression(s,&s,tag);
		if (s[0]=='\n' || s[0]==';') {
			mark_len(NULL,1,s,&s,tag);
			continue;
		}
		if (s[0]==']') {
			mark_len(NULL,1,s,&s,tag);
			break;
		}
		if (p==s) break;
	}
	if (endptr) (*endptr)=s;
	return 0;
}
/* разобрать имя переменной
*/
int
parse_variable(char *text,char **endptr,Tag *toptag) {
	char *s;
	Tag *tag;
	if (text[0]!='$') return 0;	// имя переменной с $
	tag=NULL;
	s=text;
	if (toptag)	tag=mark(VAR,text,text,toptag);
	mark_len(NULL,1,s,&s,tag);	// символ $
	if (s[0]!='(') {
		Tag *exp;
		parse_word(s,&s,tag);	// первая часть
	}
	while(s[0]=='(') {
		parse_tuple(s,&s,tag);	// индекс
		if (s[0]==')') {
			mark_len(NULL,1,s,&s,tag);	// закрывающая скобка
		} else if (strchr(EXPTERM,*s)) {
			break;
		}
	}
	if (endptr) (*endptr)=s;
	return 0;
}
/** макрокоманда #word
*/
int parse_macro(char *text,char **endptr,Tag *toptag) {
	Tag *tag;
	char *s;
	if (text[0]!='#') return 0;
	if (text[1]=='!' || text[1]=='#') return 0;
	tag=NULL;
	s=text;
	if (toptag) tag=mark(MACRO,s,s,toptag);
	mark_len(NULL,1,s,&s,tag);
	parse_word(s,&s,tag);
	if (endptr) (*endptr)=s;
	return 0;
}
/**
	слово:
		список в скобках {}
		подстановка выражения в скобках []
*/
int
parse_word(char *text,char **endptr,Tag *toptag) {
	char *p,*s;
	Tag *tag;
	tag=NULL;
	s=text;

	if (s==NULL || isblank(*s)||strchr(EXPTERM,*s)) return 1;
	if (toptag) tag=mark(WORD,text,text,toptag);

	while(*s) {
		p=s;
		switch(*s) {
			case '{' : parse_list(s,&s,tag);break;
			case '[' : parse_command(s,&s,tag);break;
			case '\'': parse_string(s,&s,tag);break;
			case '\"': parse_subst(s,&s,tag);break;
			case '$' : parse_variable(s,&s,tag);break;
			default:
				if (s[0]=='#' && s[1]!='#' && s[1]!='!') {
					parse_macro(s,&s,tag);
				} else if (s[0]=='#' && s[1]=='?' && s[2]=='>') {
					parse_print(s,&s,tag);
				} else if ((s[0]=='-' && isdigit(s[1])) || isdigit(*s))
					parse_numeric(s,&s,tag);
				else
					mark_cspn(CHARS,BLANK EXPTERM "(",s,&s,tag);
		}
		if (strchr(BLANK EXPTERM,*s)) break;
		if (p==s) break;
	}
	if (endptr) *endptr=s;
	return 0;
}
/** разобрать "выражение"
	выражение: последовательность слов разделённых пробелами
**/
int
parse_expression(char *text,char **endptr,Tag *toptag) {
	char *s,*p;
	Tag *tag;
	int comma;
	TRACE("");
	if (!text) return 0;
	s=text;
	tag=NULL;
	comma=1;
	if (toptag) tag=mark(EXPR,text,text,toptag);
	while(*s) {
		p=s;
		mark_ctype(NULL,isblank,s,&s,tag);
		if (s[0]==',') {
			/* слова в выражении разделены , */
			if (comma) {
				mark(NIL,s,s,tag);
			}
			mark_len(NULL,1,s,&s,tag);	// отметить ,
			mark_ctype(NULL,isspace,s,&s,tag); // после , разрешён \n
			comma=1;
			continue;
		} else {
			comma=0;
		}
		if (s[0]=='#' && s[1]=='!') parse_comment(s,&s,tag);
		if (s[0]=='#' && s[1]=='?' && s[2]=='>') parse_print(s,&s,tag);
		if (strchr(EXPTERM,*s)) break;
		if ('('==*s) {
			parse_tuple(s,&s,tag);
		} else
			parse_word(s,&s,tag);
		if (p==s) break;
	}
	if (comma) {
		mark(NIL,s,s,tag);
		s++;
	}
	if (endptr) *endptr=s;
	return 0;
}
/** разобрать код
		последовательность выражений разделённых ';' '\n'

**/
int
parse_code(char *text,char **endptr,Tag *toptag) {
	char *s,*p;
	Tag *tag;
	tag=NULL;
	if (toptag) tag=mark(CODE,text,text,toptag);
	s=text;
	while(*s) {
		p=s;
		mark_ctype(NULL,isspace,s,&s,tag); // лидирующие пробелы
		if (s[0]=='#' && s[1]=='!' ) {		// комментарии пропускаются
			parse_comment(s,&s,tag);
			continue;
		}
		parse_expression(s,&s,tag); // выражения
		if (s[0]==';' || s[0]=='\n') {	// разделяемые ; или \n
			mark_len(NULL,1,s,&s,tag);
			continue;
		}
		if (p==s) break;	// зацикливание
	}
	if (endptr) *endptr=s;
	return 0;
}
