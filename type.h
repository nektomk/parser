#ifndef TYPE_H
#define TYPE_H 1

#include "tag.h"

typedef struct Type {
	char *name;	// имя типа :-)
	char *remark;
} Type;

/** типы - теги, образующиеся при разборе кода */
extern Type *WORD;
extern Type *CHARS;
extern Type *COMMENT;
extern Type *ESC;
extern Type *NUMERIC;
extern Type *DOUBLE;
extern Type *LONG;
extern Type *STRING;
extern Type *LIST;
extern Type *COMMAND;
extern Type *VAR;
extern Type *MACRO;
extern Type *SMACRO;
extern Type *EXPR;
extern Type *CODE;
extern Type *PRINT;	/** текст обрамлённый #?> <?# выводится в stdio с подстановкой переменных $var и команд [command] **/
extern Type *NIL;	/** "пустой" элемент образующийся между запятыми :) {,,} по смыслу аналогичен {} или '' "" **/

#endif // TYPE_H
