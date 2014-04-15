#ifndef TYPE_H
#define TYPE_H 1

#include "tag.h"

typedef struct Type {
	char *name;	// ��� ���� :-)
	char *remark;
} Type;

/** ���� - ����, ������������ ��� ������� ���� */
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
extern Type *PRINT;	/** ����� ���������� #?> <?# ��������� � stdio � ������������ ���������� $var � ������ [command] **/
extern Type *NIL;	/** "������" ������� ������������ ����� �������� :) {,,} �� ������ ���������� {} ��� '' "" **/

#endif // TYPE_H
