#include "struct.h"
#include "trie.h"
#include "list.h"

struct key_data
{
	char* key;
	void* data;
};

static int 
get_structs (ctf_file file, struct trie** root, struct list** head)
{
	ctf_type type = NULL;
	ctf_kind kind;
	ctf_struct_union su;
	int rv;
	char* su_name;

	*root = trie_create();

	while (1)
	{
		rv = ctf_file_get_next_type(file, type, &type);
		if (ctf_is_error(rv))
			return rv;

		if (rv == CTF_END || rv == CTF_EMPTY)
			break;

		rv = ctf_type_get_kind(type, &kind);
		if (ctf_is_error(rv))
			return rv;

		if (kind != CTF_KIND_STRUCT && kind != CTF_KIND_UNION)
			continue;

		ctf_struct_union_init(type, &su);	
		ctf_struct_union_get_name(su, &su_name);

		if (su_name[0] == '\0' || su_name == NULL)
			continue;
	
		/* insert record to trie */
		trie_set(*root, su_name, su, sizeof(ctf_struct_union));

		/* insert record to list */
		struct key_data* kd = malloc(sizeof(struct key_data));
		kd->key = su_name;
		kd->data = su;
		struct list* to_add = list_create(kd);

		*head = list_add(*head, to_add);
	}

	return CTF_OK;
}

static int
compare_su (ctf_struct_union su1, ctf_struct_union su2, 
            unsigned int* diff_count)
{
	char* n1;
	char* n2;
	ctf_count c1;
	ctf_count c2;
	ctf_member m1 = NULL;
	ctf_member m2 = NULL;
	ctf_member_offset o1;
	ctf_member_offset o2;
	unsigned int i;

	ctf_struct_union_get_member_count(su1, &c1);
	ctf_struct_union_get_member_count(su2, &c2);
	ctf_struct_union_get_name(su1, &n1);
	ctf_struct_union_get_name(su2, &n2);

	if (c1 != c2)
	{
		if (c1 < c2)
		{
			printf("'%s' has more members.\n", n2);
			(*diff_count)++;
		}

		if (c1 > c2)
		{
			printf("'%s' has more members.\n", n1);
			(*diff_count)++;
		}
	}

	printf("Comparing %s\n", n1);
	for (i = 0; i < c1; i++)
	{
		printf("  member #%d: ", i);
		ctf_struct_union_get_next_member(su1, m1, &m1);
		ctf_struct_union_get_next_member(su2, m2, &m2);

		ctf_member_get_name(m1, &n1);
		ctf_member_get_name(m2, &n2);
		if (strcmp(n1, n2) != 0)
		{
			printf("names do not match: '%s' != '%s'\n", n1, n2);
			(*diff_count)++;
			break;
		}
		else
			printf("%s ", n1);

		ctf_member_get_offset(m1, &o1);
		ctf_member_get_offset(m2, &o2);
		if (o1 != o2)
		{
			printf("offsets do not match: %llu != %llu\n", o1, o2);
			(*diff_count)++;
			break;
		}
		else
			printf("%llu\n", o1);
	}

	return CTF_OK;
}

int
diff_structs (ctf_file f1, ctf_file f2, unsigned int* diff_count)
{
	struct trie* f1_root;
	struct trie* f2_root;
	struct list* f1_head = NULL;
	struct list* f2_head = NULL;

	get_structs(f1, &f1_root, &f1_head);
	get_structs(f2, &f2_root, &f2_head);

	/* 
	 * Iterate all f1 elements and find appropriate struct in f2. In case its not
	 * found, inform about it. In case it is found, compare the members and
	 * delete the entry from f2. List f2 elements that left as not found in f1.
	 */

	struct list* runner = f1_head;
	while (runner != NULL)
	{
		struct key_data* kd = list_data(runner);

		void* data;
		size_t size;
		if (trie_get(f2_root, kd->key, &data, &size) == TRIE_SUCCESS)
		{
			compare_su(data, kd->data, diff_count);
		}
		else
		{
			printf("'%s' only in file 1.\n", kd->key);
			(*diff_count)++;
		}

		runner = list_next(runner);
	}

	/*
	std::map<std::string, ctf_struct_union>::iterator iter;
	for (iter = f1_map.begin(); iter != f1_map.end(); iter++)
	{
		std::map<std::string, ctf_struct_union>::iterator iter2;
		if ((iter2 = f2_map.find()) != std::map::end)
		{
			compare_su(iter, iter2);
			f2_map.erase(iter2);
		}
		else
		{
			printf("'%s' only in file 1.\n", iter->first);
			(*diff_count)++;
		}
	}

	struct kv_list* runner;
	LIST_FOREACH(runner, f2_head, kvs)
	{
		printf("'%s' only in file 2.\n", runner->key);
		(*diff_count)++;
	}
	*/

	/*
	for (iter = f2_map.begin(); iter != f2_map.end(); iter++)
	{
		printf("'%s' only in file 2.\n", iter->first);
		(*diff_count)++;
	}
	*/

	return CTF_OK;
}

