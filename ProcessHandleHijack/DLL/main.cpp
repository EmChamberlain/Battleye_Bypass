#define _CRT_SECURE_NO_WARNINGS

#include <iostream>

#include "handles.h"
#include "RemoteMemoryOps.hpp"

using namespace std;



int ExceptionHandler(PEXCEPTION_POINTERS pExceptionInfo) {
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_INVALID_HANDLE)
		return EXCEPTION_CONTINUE_EXECUTION;
	return EXCEPTION_CONTINUE_SEARCH;
}


DWORD WINAPI MainThread(LPVOID lpParam)
{
	//std::cout.setstate(std::ios_base::failbit);//disables debugging couts
	AllocConsole();
	AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)ExceptionHandler);
	if (!AttachConsole(GetCurrentProcessId()))
	{
		ofstream logFile = ofstream("log.log");
		logFile << GetLastError() << endl;
		logFile.close();
	}
	freopen("CON", "w", stdout);
	cout << "Attached to console." << endl;
	
	HandleGatewayServer handleGatewayServer;
	handleGatewayServer.Init(L"obs64.exe");
	//handleGatewayServer.Init(L"TslGame.exe");
	
	FreeLibraryAndExitThread(NULL, EXIT_SUCCESS);
}
/*int main()
{
	
	HANDLE thread = CreateThread(NULL, 0, &MainThread, NULL, 0, NULL);
	WaitForSingleObject(thread, INFINITE);
	return EXIT_SUCCESS;
}*/

BOOL WINAPI DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, &MainThread, NULL, 0, NULL);
	case DLL_PROCESS_DETACH:
		FreeLibraryAndExitThread(NULL, EXIT_SUCCESS);
	}
	return TRUE;
}