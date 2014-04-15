#include "str.h"
#include "type.h"

Type *WORD=&(Type){"word","Отдельное слово языка"};
Type *CHARS=&(Type){"chars","литерал"};
Type *COMMENT=&(Type){"comment","комментарий"};
Type *ESC=&(Type){"esc","маскированный символ"};
Type *NUMERIC=&(Type){"numeric","цифровая константа"};
Type *DOUBLE=&(Type){"double","целая константа"};
Type *LONG=&(Type){"long","дробная константа"};
Type *STRING=&(Type){"string","строчная константа"};
Type *LIST=&(Type){"list","список обрамлённый {}"};
Type *COMMAND=&(Type){"command","подстановка команды []"};
Type *VAR=&(Type){"var","подстановка переменной с $"};
Type *MACRO=&(Type){"macro","макро вызов #"};
Type *SMACRO=&(Type){"smacro","супер-макро вызов ##"};
Type *EXPR=&(Type){"expr","отдельное выражение"};
Type *CODE=&(Type){"code","код как последовательность выражений"};
Type *PRINT=&(Type){"print","текст выводимый на консоль"};
Type *NIL=&(Type){"nil","пустой элемент"};

