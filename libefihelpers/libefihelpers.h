/**
 * libefihelpers
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

#ifndef __LIBEFIHELPERS_H
#define __LIBEFIHELPERS_H

#include <efi.h>
#include <efilib.h>

struct uefi_var {
	unsigned int len;
	unsigned char val[];
};

/*
 * debug api
 */

unsigned int dbg_enabled(void);
void dbg_sleep(unsigned int count);

/*
 * print/console api
 */

/* this can be overridden by the caller */
#ifndef PR_PREFIX
#define PR_PREFIX	L""
#endif

#define pr_raw(...) \
	Print(__VA_ARGS__)
#define pr_notice(...) \
	Print(PR_PREFIX __VA_ARGS__)
#define pr_err(...) \
	Print(PR_PREFIX __VA_ARGS__)
#define pr_debug(...) \
	{ if (dbg_enabled()) Print(L"DEBUG: " __VA_ARGS__); }

void print_hexdump(void *buf, unsigned int buf_len);

#define sleep(X) \
	(BS->Stall(X * 1000000))

/*
 * dynamic memory api
 */

void mem_init(EFI_MEMORY_TYPE type);
void *mem_alloc(unsigned int size);
void mem_free(void *buf);

void *mem_copy(void *dest, void *src, unsigned int len);
void *mem_dup(void *buf, unsigned int len);

/*
 * uefi variable api
 */

#define var_value(T,V) \
	(*((T *)V->val))

struct uefi_var *var_get(EFI_GUID scope, CHAR16 *name);

EFI_STATUS var_set_raw_attr(EFI_GUID scope, CHAR16 *name, UINT32 attr,
			    UINTN len, const void *val);
EFI_STATUS var_set_raw(EFI_GUID scope, CHAR16 *name,
		       UINTN len, const void *val);
EFI_STATUS var_set_attr(EFI_GUID scope, CHAR16 *name, UINT32 attr,
			const struct uefi_var *var);
EFI_STATUS var_set(EFI_GUID scope, CHAR16 *name, const struct uefi_var *var);

/*
 * uefi shim sbat api
 */

extern const EFI_GUID SBAT_GUID;

#endif /* __LIBEFIHELPERS_H */
