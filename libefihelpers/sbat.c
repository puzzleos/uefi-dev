/**
 * uefi sbat functions
 *
 * Copyright (c) 2021 Cisco Systems, Inc. <pmoore2@cisco.com>
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

const EFI_GUID SBAT_GUID = { 0x605dab50, 0xe046, 0x4300,
	{ 0xab, 0xb6, 0x3d, 0xd8, 0x10, 0xdd, 0x8b, 0x23 }
};
