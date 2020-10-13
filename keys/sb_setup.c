/**
 * uefi secure boot setup application
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

/*
 * this program is based on ideas from James Bottomley's efitools project
 * -> https://git.kernel.org/pub/scm/linux/kernel/git/jejb/efitools.git
 */

#include <efi.h>
#include <efilib.h>

#define PR_PREFIX L"SB_SETUP: "
#include <libefihelpers.h>

/* TODO: we should support the dbx variable, also why? */
#include "PK.h"
#include "KEK.h"
#include "DB.h"

/* TODO: efilib must define this somewhere ... */
#define __EFI_DB_GUID \
	{ 0xd719b2cb, 0x3d3a, 0x4596, \
		{ 0xa3, 0xbc, 0xda, 0xd0,  0xe, 0x67, 0x65, 0x6f }};

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle,
			   EFI_SYSTEM_TABLE *SystemTable)
{
	EFI_STATUS rc;
	EFI_GUID guid_image = EFI_LOADED_IMAGE_PROTOCOL_GUID;
	EFI_GUID guid_db = __EFI_DB_GUID;
	EFI_LOADED_IMAGE *image;
	UINT32 attr;
	struct uefi_var *var;

	/*
	 * startup boilerplate
	 */

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

	/*
	 * the guts of the app
	 */

	attr = EFI_VARIABLE_NON_VOLATILE |
	       EFI_VARIABLE_RUNTIME_ACCESS |
	       EFI_VARIABLE_BOOTSERVICE_ACCESS |
	       EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;

	pr_notice(L"attempting to configure UEFI Secure Boot\n");

	var = var_get(gEfiGlobalVariableGuid, L"SetupMode");
	if (!var) {
		pr_err(L"unable to find the SetupMode variable\n");
		return EFI_SUCCESS;
	}
	pr_notice(L"system is in Setup Mode\n");

	/* KEK */
	rc = var_set_raw_attr(gEfiGlobalVariableGuid, L"KEK", attr,
			      KEK_auth_len, KEK_auth);
	if (EFI_ERROR(rc)) {
		pr_err(L"unable to set the KEK variable (%d)\n", rc);
		return rc;
	}
	pr_notice("KEK installed\n");

	/* db */
	rc = var_set_raw_attr(guid_db, L"db", attr, DB_auth_len, DB_auth);
	if (EFI_ERROR(rc)) {
		pr_err(L"unable to set the db (%d)\n", rc);
		return rc;
	}
	pr_notice("db installed\n");

	/* PK */
	rc = var_set_raw_attr(gEfiGlobalVariableGuid, L"PK", attr,
			      PK_auth_len, PK_auth);
	if (EFI_ERROR(rc)) {
		pr_err(L"unable to set the PK variable (%d)\n", rc);
		return rc;
	}
	pr_notice("PK installed\n");

	/* PK should have set SecureBoot, verify */
	var = var_get(gEfiGlobalVariableGuid, L"SecureBoot");
	if (!var) {
		pr_err(L"unable to find the SecureBoot variable\n");
		return EFI_UNSUPPORTED;
	}
	if (var_value(UINT8, var) == 0) {
		pr_err(L"failure enabling UEFI Secure Boot\n");
		return EFI_UNSUPPORTED;
	}

	pr_notice("system is configured for UEFI Secure Boot\n");

	return EFI_SUCCESS;
}
