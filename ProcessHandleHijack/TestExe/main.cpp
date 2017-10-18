#include <windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <tlhelp32.h>
#include <psapi.h>

using namespace std;

HANDLE getHandle(LPWSTR name)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			if (_stricmp(entry.szExeFile, "TslGame.exe") == 0)
			{
				return OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
			}
		}
	}
	return NULL;
}


int main() {
	
	while (true) {
		HANDLE targetProcessHandle = getHandle(L"TslGame.exe");
		Sleep(1000);
		if(!targetProcessHandle)
			CloseHandle(targetProcessHandle);
	}
	return EXIT_SUCCESS;
}