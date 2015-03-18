#ifndef CTFDIFF_STRUCT_H
#define CTFDIFF_STRUCT_H

#include <libctf/libctf.h>

int
diff_structs (ctf_file f1, ctf_file f2, unsigned int* diff_count);

#endif

