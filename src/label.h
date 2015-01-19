#ifndef CTFDIFF_LABEL_H
#define CTFDIFF_LABEL_H

#include <libctf/libctf.h>

/**
 * Traverse all labels of a file and compare them.
 *
 * @param[in] f1 file1
 * @param[in] f2 file2
 * @param[out] diff_count number of differences
 * @return CTF_OK on success, CTF_E_* otherwise
 */
int
diff_labels (ctf_file f1, ctf_file f2, unsigned int* diff_count);

#endif

