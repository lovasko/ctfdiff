#include <string.h>
#include <stdio.h>
#include <libctf/libctf.h>

#include "label.h"

/**
 * Compare two labels.
 *
 * Note: no NULL check is performed on the diff_count variable, since this
 * function can be called only from the diff_labels function which already
 * performs the check.
 * 
 * @param[in] l1 first label
 * @param[in] l2 second label
 * @param[out] diff_count number of differences
 * @return CTF_OK on success, CTF_E_* otherwise
 */
static int
diff_label (ctf_label l1, ctf_label l2, unsigned int* diff_count)
{
	char* n1;
	char* n2;
	int rv;

	*diff_count = 0;

	rv = ctf_label_get_name(l1, &n1);
	if (ctf_is_error(rv))
		return rv;

	rv = ctf_label_get_name(l2, &n2);
	if (ctf_is_error(rv))
		return rv;

	if (strcmp(n1, n2) != 0)
	{
		printf("Different labels: '%s' =/= '%s'\n", n1, n2);
		*diff_count = 1;
	}

	return CTF_OK;	
}

int
diff_labels (ctf_file f1, ctf_file f2, unsigned int* diff_count)
{
	ctf_label l1;
	ctf_label l2;
	int rv;
	int rv1;
	int rv2;

	if (diff_count == NULL)
		return CTF_E_NULL;

	*diff_count = 0;

	rv1 = ctf_file_get_next_label(f1, NULL, &l1);
	if (ctf_is_error(rv1))
		return rv1;

	rv2 = ctf_file_get_next_label(f2, NULL, &l2);
	if (ctf_is_error(rv2))
		return rv2;

	if (rv1 == CTF_EMPTY && rv2 != CTF_EMPTY)
	{
		printf("First file has no labels and the second one does.\n");
		*diff_count = 1;
		return CTF_OK;
	}

	if (rv1 != CTF_EMPTY && rv2 == CTF_EMPTY)
	{
		printf("Second file has no labels and the first one does.\n");
		*diff_count = 1;
		return CTF_OK;
	}

	l1 = NULL;
	l2 = NULL;

	while (1)
	{
		rv1 = ctf_file_get_next_label(f1, l1, &l1);
		if (ctf_is_error(rv1))
			return rv1;

		rv2 = ctf_file_get_next_label(f2, l2, &l2);
		if (ctf_is_error(rv2))
			return rv2;

		if (rv1 != rv2)
		{
			if (rv1 == CTF_END && rv2 == CTF_OK)
				printf("Second file has more labels.\n");

			if (rv2 == CTF_END && rv1 == CTF_OK)
				printf("First file has more labels.\n");

			(*diff_count)++;
			break;
		}

		/*
		 * Non-trivial thought: at this point we can assume that rv1 == rv2 so it
		 * is sufficient to work with rv1 only.
		 */
		if (rv1 == CTF_EMPTY || rv1 == CTF_END)
			return CTF_OK;

		if (rv1 == CTF_OK)
		{
			rv = diff_label(l1, l2, diff_count);
			if (ctf_is_error(rv))
				return rv;
		}
	}

	return CTF_OK;
}

