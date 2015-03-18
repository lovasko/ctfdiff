#ifndef CTFDIFF_TRIE_H
#define CTFDIFF_TRIE_H

#include <sys/types.h>
#include <sys/stdint.h>

#include <stdlib.h>
#include <stdio.h>

#define TRIE_SUCCESS 0
#define TRIE_NOT_FOUND 1
#define TRIE_KEY_INVALID 2

#define TRIE_KEY_SET_SIZE 55

struct trie
{
	char key;
	void* data;
	size_t size;
	struct trie* children[TRIE_KEY_SET_SIZE];
};

/**
 * Initialize a trie node.
 *
 * @return initialized trie node
 */
struct trie*
trie_create ();

/**
 * Insert a trie record.
 *
 * @param[in] key NULL-terminated key of the record 
 * @param[in] value value of the record
 * @param[in] size size of the value
 * @return TRIE_SUCCESS on successful insertion, TRIE_KEY_INVALID in case of an
 *         invalid key
 */
int
trie_set (struct trie* root, char* key, void* value, size_t size);

int
trie_remove (struct trie* root, char* key);

int
trie_dump (struct trie* root);

/**
 * Retrieve a trie record.
 *
 * @param[in] key NULL-terminated key of the record
 * @param[out] value value of the record
 * @param[out] size size of the value
 * @return TRIE_FOUND if the record was found, TRIE_NOT_FOUND otherwise.
 *         TRIE_KEY_INVALID in case of an invalid key
 */
int
trie_get (struct trie* root, char* key, void** value, size_t* size);

int
trie_contains (struct trie* root, char* key);


#endif

