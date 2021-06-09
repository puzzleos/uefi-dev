/**
 * uefi sbat write application
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

#include <libefihelpers.h>

#include "sbat_data.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle,
			   EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS rc;
	EFI_GUID guid_image = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	EFI_LOADED_IMAGE *image;
	UINT32 attr;
	char *sbat_str;

	/* init the gnu-efi/uefi services, this *must* be near the top */
	InitializeLib(ImageHandle, SystemTable);

	/* get our EFI_LOADED_IMAGE pointer */
	rc = uefi_call_wrapper(BS->HandleProtocol, 3,
			       ImageHandle, &guid_image, &image);
	if (EFI_ERROR(rc)) {
		pr_err(L"unable to initialize the LOADED_IMAGE protocol\n");
		return rc;
	}

	/* init the memory allocator */
	mem_init(image->ImageDataType);

	/* display the sbat value we are writing */
	sbat_str = mem_alloc(sbat_data_csv_len + 1);
	if (!sbat_str) {
		pr_err(L"memory allocation failed\n");
		goto out;
	}
	mem_copy(sbat_str, sbat_data_csv, sbat_data_csv_len);
	sbat_str[sbat_data_csv_len] = '\0';

	pr_raw(L"STRING:\n");
	pr_raw(L"%a", sbat_str);
	pr_raw(L"\n");
	pr_raw(L"HEX:\n");
	print_hexdump(sbat_data_csv, sbat_data_csv_len);

	/* write the sbat value to a variable */
	/* TODO: need EFI_VARIABLE_RUNTIME_ACCESS for devel ? */
	attr = EFI_VARIABLE_NON_VOLATILE | \
	       EFI_VARIABLE_BOOTSERVICE_ACCESS;
	rc = var_set_raw_attr(SBAT_GUID, L"SbatLevel", attr,
			      sbat_data_csv_len, sbat_data_csv);
	if (EFI_ERROR(rc)) {
		pr_err(L"unable to write the SbatLevel variable\n");
		return rc;
	}

out:
	return EFI_SUCCESS;
}
