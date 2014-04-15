#ifndef PARSER_H
#define PARSER_H 1

#include "tag.h"

int parse_comment(char *s,char **endptr,Tag *toptag);
int parse_chars(char *s,char **endptr,Tag *toptag);
int parse_escape(char *s,char **endptr,Tag *toptag);
int parse_numeric(char *s,char **endptr,Tag *toptag);
int parse_subst(char *s,char **endptr,Tag *toptag);
int parse_string(char *s,char **endptr,Tag *toptag);
int parse_list(char *s,char **endptr,Tag *toptag);
int parse_command(char *s,char **endptr,Tag *toptag);
int parse_code(char *s,char **endptr,Tag *toptag);
int parse_variable(char *s,char **endptr,Tag *toptag);
int parse_expression(char *s,char **endptr,Tag *toptag);
int parse_macro(char *s,char **endptr,Tag *toptag);

#endif // PARSER_H
