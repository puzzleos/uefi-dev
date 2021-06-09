/**
 * uefi memory functions
 *
 * Copyright (c) 2020 Cisco Systems, Inc. <pmoore2@cisco.com>
 */

/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include <efi.h>
#include <efilib.h>

#include "libefihelpers.h"

static unsigned int pool_type_set = 0;
static EFI_MEMORY_TYPE pool_type;

/**
 * Initialize the memory allocator
 * @type: the EFI memory type
 *
 * Instruct the memory allocator which memory pool to use.
 */
void mem_init(EFI_MEMORY_TYPE type)
{
	pool_type = type;
	pool_type_set = 1;
}

/**
 * Allocate memory
 * @size: size of buffer to allocate
 *
 * Allocate memory, similar to the standard "malloc()" function.
 */
void *mem_alloc(unsigned int size)
{
	EFI_STATUS rc;
	unsigned char *buf;
	unsigned int i;

	if (!pool_type_set)
		return NULL;

	/* allocate and zero out the memory */
	rc = uefi_call_wrapper(BS->AllocatePool, 3, pool_type, size, &buf);
	if (EFI_ERROR(rc)) {
		pr_err(L"unable to allocate memory (%d bytes)\n");
		return NULL;
	}
	/* TODO: look into using BZ->SetMem() to zeroize the buffer */
	for (i = 0; i < size; i++)
		buf[i] = 0x00;

	return buf;
}

/**
 * Free memory
 * @buf: the buffer to free
 *
 * Free memory, similar to the standard "free()" function.  Can free memory
 * allocated via "mem_alloc()" or the standard library functions.
 */
void mem_free(void *buf)
{
	if (!buf)
		return;
	uefi_call_wrapper(BS->FreePool, 1, buf);
}

/**
 * Copy memory from one buffer to another
 * @dest: destination buffer
 * @src: source buffer
 * @len: number of bytes to copy
 */
void *mem_copy(void *dest, void *src, unsigned int len)
{
	unsigned int i;
	unsigned char *d = dest, *s = src;

	/* TODO: look into using BS->CopyMem() */

	if (!dest || !src || len == 0)
		return dest;

	for (i = 0; i < len; i++)
		d[i] = s[i];

	return dest;
}

/**
 * Duplicate a memory buffer
 * @buf: buffer to duplicate
 * @len: length of buffer
 */
void *mem_dup(void *buf, unsigned int len)
{
	unsigned char *new;

	if (!buf || len == 0)
		return NULL;

	new = mem_alloc(len);
	if (!new)
		return NULL;
	return mem_copy(new, buf, len);
}

/**
 * Get a UEFI variable
 * @scope: variable scope guid
 * @name: variable name
 *
 * Retrieve the UEFI variable and return it to the caller, returns NULL on
 * failure.  The caller is responsible for freeing the returned memory.
 */
struct uefi_var *var_get(EFI_GUID scope, CHAR16 *name)
{
	EFI_STATUS rc;
	UINTN len;
	struct uefi_var *var = NULL;

	/* get the variable size */
	len = 0;
	rc = uefi_call_wrapper(RT->GetVariable, 5,
			       name, &scope, NULL, &len, NULL);
	if (rc != EFI_BUFFER_TOO_SMALL)
		goto err;

	/* allocate space for the variable struct + data */
	var = mem_alloc(sizeof(*var) + len);
	if (!var)
		goto err;

	/* do the actual get */
	var->len = len;
	rc = uefi_call_wrapper(RT->GetVariable, 5,
			       name, &scope, NULL, &len, var->val);
	if (EFI_ERROR(rc))
		goto err;

	return var;

err:
	mem_free(var);
	return NULL;
}

/**
 * Set a UEFI variable with attributes
 * @scope: variable scope guid
 * @name: variable name
 * @attr: variable attributes
 * @len: length of variable
 * @val: value
 */
EFI_STATUS var_set_raw_attr(EFI_GUID scope, CHAR16 *name, UINT32 attr,
			    UINTN len, const void *val)
{
	EFI_STATUS rc;
	rc = uefi_call_wrapper(RT->SetVariable, 5,
			       name, &scope, attr, len, val);
	return rc;
}

/**
 * Set a UEFI variable
 * @scope: variable scope guid
 * @name: variable name
 * @len: length of variable
 * @val: value
 */
EFI_STATUS var_set_raw(EFI_GUID scope, CHAR16 *name,
		       UINTN len, const void *val)
{
	UINT32 attr;

	/* NOTE: pick basic attributes, use var_set_raw_attr() for others */
	attr = (EFI_VARIABLE_NON_VOLATILE |
		EFI_VARIABLE_BOOTSERVICE_ACCESS |
		EFI_VARIABLE_RUNTIME_ACCESS);

	return var_set_raw_attr(scope, name, attr, len, val);
}

/**
 * Set a UEFI variable with attributes
 * @scope: variable scope guid
 * @name: variable name
 * @attr: variable attributes
 * @var: variable struct
 */
EFI_STATUS var_set_attr(EFI_GUID scope, CHAR16 *name, UINT32 attr,
			const struct uefi_var *var)
{
	return var_set_raw_attr(scope, name, attr, var->len, var->val);
}

/**
 * Set a UEFI variable
 * @scope: variable scope guid
 * @name: variable name
 * @var: variable struct
 */
EFI_STATUS var_set(EFI_GUID scope, CHAR16 *name, const struct uefi_var *var)
{
	return var_set_raw(scope, name, var->len, var->val);
}
