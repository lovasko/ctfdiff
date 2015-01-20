#ifndef CTFDIFF_TYPE_H
#define CFFDIFF_TYPE_H

#include <libctf/libctf.h>

#include "string_list.h"

/**
 * Traverse all types of a file and compare them.
 *
 * In case that T_flag is 1, string_list head is used as a name-based filter to
 * compare only matching types from the f1 file. In case of the t_flag, the
 * string_list head can be NULL. At least one of the t/T flags must be set to
 * true.
 *
 * @param[in] f1 file1
 * @param[in] f2 file2
 * @param[in] t_flag all types flag
 * @param[in] T_flag specific types flag
 * @param[in] head string list head
 * @param[out] diff_count number of differences
 * @return CTF_OK on success, CTF_E_* otherwise
 */
int
diff_types (ctf_file f1, ctf_file f2, uint8_t t_flag, uint8_t T_flag, 
    struct string_list_head* head, unsigned int* diff_count);

#endif

