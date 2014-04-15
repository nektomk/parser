#include <stdlib.h>
#include "tag.h"
#include "type.h"

Tag *left_pass(Tag *head,Tag *(*cb)(Tag *));
Tag *right_pass(Tag *head,Tag *(*cb)(Tag *));

static void freetags(Tag *head) {
	Tag *tag;
	while(head) {
		tag=head;
		head=head->next;
		if (!tag->name && tag->text && tag->size) {
			free(tag->text);
		}
		free(tag);
	}
}
/* проход справа налево с исполнением макросов
*/
Tag *
right_pass(
	Tag *head,
	Tag *(*cb)(Tag *)
){
	Tag *args;
	Tag *tag,*result;
	args=NULL;
	/* в исходном списке слова представлены
	в обратном порядке, ->
	формируем прямой порядок аргументов, заодно выкидывая коментарии,
	неинтерпретируемые символы и осуществляя макро-подстановки
	*/
	for(tag=head;tag;tag=tag->next) {
		/* пропускаем оформление и комментарии */
		if (tag->name==NULL || tag->name==COMMENT) continue;
		if (tag->name==MACRO) {
			/* исполнить макрос, заменить args на его результат */
			result=left_pass(head->next,cb);
			freetags(args);
			args=result;
			continue;
		}
		/* поместить в args копию тега */
		result=clone_tag(tag);
		result->next=args;
		args=result;
	}
	/* в args лежат аргументы в правильном порядке
	*/
	return left_pass(args,cb);
}
/* проход слева направо - раскрытие имени команды и вызов callback
*/
Tag *
left_pass(
	Tag *head,
	Tag *(*cb)(Tag *)
){
	Tag *args;
	Tag *trash;
	Tag *tag;

	args=NULL;
	trash=NULL;
	while(head) {
		/* получить элемент имени команды */
		tag=head;
		head=head->next;
		/* формирование аргументов */
		if (tag->name==PRINT) {
			// вывод в консоль без формирования результатов
			tag->next=trash;
			trash=tag;
			continue;
		}
		// ToDo - подстановка переменных и команд

		tag->next=args;
		args=tag;
	}
	freetags(trash);
	tag=cb(args);
	freetags(args);
	return args;
}
Tag *
eval(
	Tag *tag,
	Tag *(*cb)(Tag *)
) {
	Tag *result;
	return right_pass(tag->sub,cb);
}
