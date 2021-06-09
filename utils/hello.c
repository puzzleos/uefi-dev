/**
 * uefi "hello!" test application
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

#include <libefihelpers.h>

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle,
			   EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS rc;
	EFI_GUID guid_image = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	EFI_LOADED_IMAGE *image;

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

	pr_raw(L"HELLO!\n");

	if (image->LoadOptionsSize > 0) {
		pr_raw(L"\nLoadOptions:\n");
		print_hexdump(image->LoadOptions, image->LoadOptionsSize);
	}

	/* sleep to give us a chance to see the output */
	sleep(5);

	return EFI_SUCCESS;
}
