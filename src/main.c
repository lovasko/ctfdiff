#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <libctf/libctf.h>

#include "label.h"

/* exit status */
#define CTF_DIFF_EQUAL          0
#define CTF_DIFF_NOT_EQUAL      1
#define CTF_DIFF_ARG_ERROR      2
#define CTF_DIFF_INTERNAL_ERROR 3

/**
 * Display help message.
 */
static void
usage ()
{
	printf("Usage: ctfdiff [-afFhilLoOqtT] file1 file2\n");
}

/**
 * ctfdiff - compute and display differences in two CTF data sets.
 */
int
main (int argc, char* argv[])
{
	ctf_file f1;
	ctf_file f2;
	int option;
	int rv;
	unsigned int diff_count = 0;
	uint8_t l_flag = 0;

	while ((option = getopt(argc, argv, "hl")) != -1)
	{
		switch(option)
		{
			case 'l': 
				l_flag = 1;
			break;

			case 'h':
				usage();
			return CTF_DIFF_ARG_ERROR;

			case '?':
				fprintf(stderr, "ERROR: invalid option %c\n", optopt);	
				usage();
			return CTF_DIFF_ARG_ERROR;

			default: 
				fprintf(stderr, "ERROR: unknown error during option parsing\n");	
			return CTF_DIFF_ARG_ERROR;
		}
	}

	if (argc - optind < 2)
	{
		fprintf(stderr, "ERROR: expected two files\n");
		usage();
		return CTF_DIFF_ARG_ERROR;
	}

	rv = ctf_file_read(argv[optind], &f1);
	if (ctf_is_error(rv))
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(rv));
		return CTF_DIFF_INTERNAL_ERROR;
	}

	rv = ctf_file_read(argv[optind+1], &f2);
	if (ctf_is_error(rv))
	{
		fprintf(stderr, "ERROR: %s\n", ctf_get_error_string(rv));
		return CTF_DIFF_INTERNAL_ERROR;
	}

	if (l_flag)
	{
		rv = diff_labels(f1, f2, &diff_count);
		if (ctf_is_error(rv))
			return CTF_DIFF_INTERNAL_ERROR;	
	}

	return diff_count > 0 ? CTF_DIFF_NOT_EQUAL : CTF_DIFF_EQUAL;
}

