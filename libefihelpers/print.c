/**
 * uefi console print and related functions
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

/**
 * Check to see if we are operating in "debug" mode
 */
unsigned int dbg_enabled(void)
{
	static int enabled = -1;
	struct uefi_var *var;

	/* return our cached copy if we have it */
	if (enabled != -1)
		return enabled;

	/* TODO: create our own variable scope */

	/* check for our "debug" uefi variable */
	var = var_get(gEfiGlobalVariableGuid, L"debug");
	if (var && var->len == sizeof(int))
		enabled = var_value(UINT8, var);
	else
		enabled = 0;

	return enabled;
}

/**
 * Sleep for a period of time
 * @count: a relative period of time to sleep, 0 for default
 *
 * Unfortunately there doesn't appear to be an easy way to sleep for a
 * specified amount of time so we go into a busy loop to stall execution.
 * Needless to say this if a pretty ugly thing to do, but it works for now and
 * it isn't worth trying to come up with something more elegant at the moment.
 *
 * If debugging is not enabled, this becomes a no-op.
 *
 */
#define _SLEEP_DEFAULT    1
void dbg_sleep(unsigned int count)
{
	unsigned int i, j;

	if (!dbg_enabled())
		return;

	if (count == 0)
		count = _SLEEP_DEFAULT;

	for (i = 0; i < count; i++)
		for (j = 0; j < 0xffffffff; j++);
}

/**
 * Output a buffer using a "hexdump" style of output
 * @buf: the buffer to dump
 * @buf_len: the buffer size
 */
#define _HEXDUMP_COLUMNS		16
void print_hexdump(void *buf, unsigned int buf_len)
{
	unsigned int i, j, k;
	unsigned char val;
	unsigned col_width;
	unsigned char *b = buf;

	/* NOTE: set the number of columns */
	col_width = _HEXDUMP_COLUMNS;

	for (i = 0, j = 0; i < buf_len;) {
		/* NOTE: we need to do out own zero-padding to work around
		 *       problems with the library */
		if (i == j) {
			if (i <= 9999) {
				if (i < 1000)
					pr_raw(L"0");
				if (i < 100)
					pr_raw(L"0");
				if (i < 10)
					pr_raw(L"0");
				pr_raw(L"%d: ", i);
			} else
				pr_raw(L"XXXX");
		}
		if (b[i] < 0x10)
			pr_raw(L"0");
		pr_raw(L"%x ", b[i++]);
		if (!(i % col_width) || i == buf_len) {
			for (k = i; k % col_width; k++)
				pr_raw(L"   ");
			while (j < i) {
				val = b[j++];
				if (val == ' ' || val < '!' || val > '~')
					val = '.';
				pr_raw(L"%c", val);
			}
			pr_raw(L"\n");
		}
	}
}
