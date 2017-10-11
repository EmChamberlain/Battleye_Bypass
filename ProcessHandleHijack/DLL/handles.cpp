#include <Windows.h>



#include "handles.h"


NTSTATUS enumerate_handles(ENUM_HANDLE_CALLBACK callback)
{
	NTSTATUS                    status = STATUS_UNSUCCESSFUL;
	PVOID                       buffer = NULL;
	ULONG                       bufferSize = 0;

	do {
		status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)16/*SystemHandleInformation*/, buffer, bufferSize, &bufferSize);
		if (!NT_SUCCESS(status)) {
			if (status == STATUS_INFO_LENGTH_MISMATCH) {
				if (buffer != NULL)
					VirtualFree(buffer, 0, MEM_RELEASE);
				buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
				continue;
			}
			break;
		}
		else {
			PSYSTEM_HANDLE_INFORMATION handleInfo = (PSYSTEM_HANDLE_INFORMATION)buffer;
			for (ULONG i = 0; i < handleInfo->NumberOfHandles; i++) {
				auto handle = &handleInfo->Handles[i];
				status = callback(handle);
				if (NT_SUCCESS(status))
					break;
			}
			break;
		}
	} while (true);
	if (buffer != NULL)
		VirtualFree(buffer, 0, MEM_RELEASE);
	return status;
}


HANDLE get_handle_to_process(LPWSTR process)
{
	HANDLE hProcess = NULL;

	enumerate_handles([&](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) {

		if (GetCurrentProcessId() != handle->UniqueProcessId) return STATUS_UNSUCCESSFUL;

		BOOL        found = FALSE;
		PVOID       buffer = NULL;
		ULONG       bufferSize = 0x100;
		NTSTATUS    status;

		//
		// 7 is the process type index. It's possible that this is different value on your system. 
		// Check the output print_handle_information for more info. You can compare the TypeName to
		// "Process" instead of doing it like this
		//
		if (handle->ObjectTypeIndex == 7) {
			WCHAR processPath[MAX_PATH] = {};
			//
			// The handle needs to have PROCESS_QUERY_INFORMATION and PROCESS_VM_READ
			// access, otherwise this call fails
			// 
			if (GetModuleFileNameExW((HANDLE)handle->HandleValue, NULL, processPath, MAX_PATH)) {
				LPWSTR filename = PathFindFileNameW(processPath);
				if (!wcscmp(filename, process)) {
					found = TRUE;
					hProcess = (HANDLE)handle->HandleValue;
				}
			}
		}

		//
		// STATUS_SUCCESS stops the enumeration
		//
		if (found)
			return STATUS_SUCCESS;

		return STATUS_UNSUCCESSFUL;
	});

	return hProcess;
}