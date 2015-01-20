#include "type.h"

/**
 * Compare two integers.
 *
 * @param[in] i1 first integer
 * @param[in] i2 second integer
 * @param[out] diff_count number of differences
 */
static int
diff_int (ctf_int i1, ctf_int i2, unsigned int* diff_count)
{
	int rv;
	ctf_int_content c1;
	ctf_int_content c2;
	ctf_int_offset o1;
	ctf_int_offset o2;
	ctf_int_size s1;
	ctf_int_size s2;
	uint8_t is1;
	uint8_t is2;

	/* content */
	rv = ctf_int_get_content(i1, &c1);
	if (ctf_is_error(rv))
		return rv;

	rv = ctf_int_get_content(i2, &c2);
	if (ctf_is_error(rv))
		return rv;

	if (i1 != i2)
	{
		/* TODO print */
		(*diff_count)++;
	}

	/* offset */
	rv = ctf_int_get_offset(i1, &o1);
	if (ctf_is_error(rv))
		return rv;

	rv = ctf_int_get_offset(i2, &o2);
	if (ctf_is_error(rv))
		return rv;

	if (o1 != o2)
	{
		/* TODO print */
		(*diff_count)++;
	}

	/* size */
	rv = ctf_int_get_size(i1, &s1);
	if (ctf_is_error(rv))
		return rv;

	rv = ctf_int_get_size(i2, &s2);
	if (ctf_is_error(rv))
		return rv;

	if (s1 != s2)
	{
		/* TODO print */
		(*diff_count)++;
	}

	/* is_signed */
	rv = ctf_int_get_is_signed(i1, &is1);
	if (ctf_is_error(rv))
		return rv;

	rv = ctf_int_get_is_signed(i2, &is2);
	if (ctf_is_error(rv))
		return rv;

	if (is1 != is2)
	{
		/* TODO print */
		(*diff_count)++;
	}

	return CTF_OK;
}

/**
 * Get name of the type.
 *
 * Note: some kinds do not contain the name property.
 *
 * @param[in] type type
 * @param[out] name name
 * @return CTF_OK on success, CTF_E_* otherwise
 */
static int
get_type_name (ctf_type type, char** name)
{
	int rv;
	ctf_kind kind;
	ctf_int _int;
	ctf_float _float;
	ctf_function function;
	ctf_enum _enum;
	ctf_typedef _typedef;
	ctf_fwd_decl fwd_decl;
	ctf_struct_union struct_union;

	rv = ctf_type_get_kind(type, &kind);
	if (ctf_is_error(rv))
		return rv;

	switch (kind)
	{
		/* FALL THROUGH */
		case CTF_KIND_NONE:
		case CTF_KIND_POINTER:
		case CTF_KIND_ARRAY:
		case CTF_KIND_VOLATILE:
		case CTF_KIND_CONST:
		case CTF_KIND_RESTRICT:
			*name = NULL;
		break;

		case CTF_KIND_INT:
		{
			rv = ctf_int_init(type, &_int);
			if (ctf_is_error(rv))
				return rv;

			rv = ctf_int_get_name(_int, name);
			if (ctf_is_error(rv))
				return rv;

			break;
		}

		case CTF_KIND_FLOAT:
		{
			rv = ctf_float_init(type, &_float);
			if (ctf_is_error(rv))
				return rv;

			rv = ctf_float_get_name(_float, name);
			if (ctf_is_error(rv))
				return rv;

			break;
		}

		case CTF_KIND_FUNC:
		{
			rv = ctf_function_init(type, &function);
			if (ctf_is_error(rv))
				return rv;

			rv = ctf_function_get_name(function, name);
			if (ctf_is_error(rv))
				return rv;

			break;
		}

		/* FALL THROUGH */
		case CTF_KIND_UNION:
		case CTF_KIND_STRUCT:
		{
			rv = ctf_struct_union_init(type, &struct_union);
			if (ctf_is_error(rv))
				return rv;

			rv = ctf_struct_union_get_name(struct_union, name);
			if (ctf_is_error(rv))
				return rv;

			break;
		}

		case CTF_KIND_ENUM:
		{
			rv = ctf_enum_init(type, &_enum);
			if (ctf_is_error(rv))
				return rv;

			rv = ctf_enum_get_name(_enum, name);
			if (ctf_is_error(rv))
				return rv;

			break;
		}

		case CTF_KIND_FWD_DECL:
		{
			rv = ctf_fwd_decl_init(type, &fwd_decl);
			if (ctf_is_error(rv))
				return rv;

			rv = ctf_fwd_decl_get_name(fwd_decl, name);
			if (ctf_is_error(rv))
				return rv;

			break;
		}

		case CTF_KIND_TYPEDEF:
		{
			rv = ctf_typedef_init(type, &_typedef);
			if (ctf_is_error(rv))
				return rv;

			rv = ctf_typedef_get_name(_typedef, name);
			if (ctf_is_error(rv))
				return rv;

			break;
		}

		default:
			return CTF_E_KIND_INVALID;
	}

	return CTF_OK;
}

int
diff_types (ctf_file f1, ctf_file f2, uint8_t t_flag, uint8_t T_flag, 
    struct string_list_head* head, unsigned int* diff_count)
{
	ctf_type t1 = NULL;
	ctf_type t2 = NULL;
	struct string_list* runner;
	char* type_name;
	uint8_t found;
	int rv1;
	int rv2;
	int rv;
	ctf_int i1;
	ctf_int i2;


	*diff_count = 0;

	while (1)
	{
		rv1 = ctf_file_get_next_type(f1, t1, &t1);
		if (ctf_is_error(rv1))
			return rv1;

		rv2 = ctf_file_get_next_type(f2, t2, &t2);
		if (ctf_is_error(rv2))
			return rv2;

		if (rv1 == CTF_EMPTY && rv2 != CTF_EMPTY)
		{
			printf("First file has no labels and the second one does.\n");
			(*diff_count)++;
			return CTF_OK;
		}

		if (rv1 != CTF_EMPTY && rv2 == CTF_EMPTY)
		{
			printf("Second file has no labels and the first one does.\n");
			(*diff_count)++;
			return CTF_OK;
		}

		rv = ctf_type_get_kind(t1, &k1);
		if (ctf_is_error(rv))
			return rv;

		rv = ctf_type_get_kind(t2, &k2);
		if (ctf_is_error(rv))
			return rv;

		if (k1 == k2)
		{
			if (T_flag)
			{
				rv = get_type_name(t1, &type_name);
				if (ctf_is_error(rv))
					return rv;

				found = 0;
				SLIST_FOREACH (runner, head, elements)
				{
					if (strncmp(runner->value, type_name, runner->length) == 0)
						found = 1;
				}

				if (!found)
					continue;
			}

			switch (k1)
			{
				case CTF_KIND_INT:
				{
					rv = ctf_int_init(t1, &i1);
					if (ctf_is_error(rv))
						return rv;

					rv = ctf_int_init(t2, &i2);
					if (ctf_is_error(rv))
						return rv;

					rv = diff_int(i1, i2, &diff_count);
					if (ctf_is_error(rv))
						return rv;

					break;
				}
			}
		}
		else
		{
			/* TODO print information */
			*(diff_count)++;
			break;
		}
	}

	return CTF_OK;
}

