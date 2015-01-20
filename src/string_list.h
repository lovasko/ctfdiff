#ifndef CTFDIFF_STRING_LIST_H
#define CTFDIFF_STRING_LIST_H

#include <sys/queue.h>

/**
 * Single-linked list of strings.
 */
struct string_list
{
	char* value;
	size_t length;

	SLIST_ENTRY(string_list) elements;
};

SLIST_HEAD(string_list_head, string_list);

#endif

