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
/* ������ ������ ������ � ����������� ��������
*/
Tag *
right_pass(
	Tag *head,
	Tag *(*cb)(Tag *)
){
	Tag *args;
	Tag *tag,*result;
	args=NULL;
	/* � �������� ������ ����� ������������
	� �������� �������, ->
	��������� ������ ������� ����������, ������ ��������� ����������,
	������������������ ������� � ����������� �����-�����������
	*/
	for(tag=head;tag;tag=tag->next) {
		/* ���������� ���������� � ����������� */
		if (tag->name==NULL || tag->name==COMMENT) continue;
		if (tag->name==MACRO) {
			/* ��������� ������, �������� args �� ��� ��������� */
			result=left_pass(head->next,cb);
			freetags(args);
			args=result;
			continue;
		}
		/* ��������� � args ����� ���� */
		result=clone_tag(tag);
		result->next=args;
		args=result;
	}
	/* � args ����� ��������� � ���������� �������
	*/
	return left_pass(args,cb);
}
/* ������ ����� ������� - ��������� ����� ������� � ����� callback
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
		/* �������� ������� ����� ������� */
		tag=head;
		head=head->next;
		/* ������������ ���������� */
		if (tag->name==PRINT) {
			// ����� � ������� ��� ������������ �����������
			tag->next=trash;
			trash=tag;
			continue;
		}
		// ToDo - ����������� ���������� � ������

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
