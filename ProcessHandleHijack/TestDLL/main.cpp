#include <Windows.h>





int ExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) {
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_INVALID_HANDLE)
		return EXCEPTION_CONTINUE_EXECUTION;
	return EXCEPTION_CONTINUE_SEARCH;
}


DWORD WINAPI MainThread(LPVOID lpParam)
{
	AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler);
	HANDLE targetProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, 13968);
	CloseHandle(targetProcessHandle);

	FreeLibraryAndExitThread(NULL, EXIT_SUCCESS);

}

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, &MainThread, NULL, 0, NULL);
	}
}