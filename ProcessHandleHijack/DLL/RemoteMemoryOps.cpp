#define _CRT_SECURE_NO_WARNINGS



#include "RemoteMemoryOps.h"
#include "handles.h"

using namespace std;


int HandleGatewayServer::Init(LPWSTR pName) {
	processName = pName;
	processHandle = get_handle_to_process(processName);
	std::cout << "Handle: " << hex << processHandle << endl;
	
	while (1) {
		if (processHandle) {
			TCHAR buffer[MAX_PATH];
			if (GetModuleBaseName(processHandle, NULL, buffer, MAX_PATH)) {
				WCHAR temp[MAX_PATH];
				mbstowcs(temp, buffer, MAX_PATH);

				wstring handleStr = wstring(temp);
				wstring processStr = wstring(processName);
				//wcout << "HandleStr: " << temp << endl;
				//wcout << "ProcessStr: " << processName << endl;
				if (handleStr != processStr) {
					Sleep(1000);
					processHandle = NULL;
					processHandle = get_handle_to_process(processName);
					continue;
				}
			}
			else if (GetLastError() == EXCEPTION_INVALID_HANDLE){
				Sleep(1000);
				processHandle == NULL;
				processHandle = get_handle_to_process(processName);
				continue;
			}
			else {
				Sleep(1000);
				std::cout << "ERROR ]> Strange process handle. GetLastError: " << dec << GetLastError() << endl;
				processHandle == NULL;
				processHandle = get_handle_to_process(processName);
				continue;
			}
		}
		else {
			Sleep(1000);
			processHandle = get_handle_to_process(processName);
			continue;
		}
		std::cout << "OK    ]> Pipe started. Awaiting clients." << endl;
		//_tprintf(TEXT("\nPipe Server: Main thread awaiting client connection on %s\n"), m_lpszPipename);
		m_pipeHandle = CreateNamedPipe(m_lpszPipename, PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, MAXPIPEFILESIZE, MAXPIPEFILESIZE, 2000, NULL);

		if (m_pipeHandle == INVALID_HANDLE_VALUE) {
			std::cout << "ERROR ]> CreateNamedPipe failed. GetLastError: " << dec << GetLastError() << endl;
			return -1;
		}
		
		// Wait for the client to connect; if it succeeds, the function returns a nonzero value. If the function returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 
		while (!m_clientConnected) {
			if (ConnectNamedPipe(m_pipeHandle, NULL)) {
				m_clientConnected = TRUE;
			}
			else {
				m_clientConnected = (GetLastError() == ERROR_PIPE_CONNECTED);
			}
		}
			//m_clientConnected = ConnectNamedPipe(m_pipeHandle, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		
		if (m_clientConnected) {
			std::cout << "OK    ]> Client connected. Starting gateway." << endl;
			processHandle = get_handle_to_process(processName);
			HandleGatewayServer::Gateway();
		}
		else {
			CloseHandle(m_pipeHandle); // The client could not connect, so close the pipe. 
			m_clientConnected = FALSE;
		}
		CloseHandle(m_pipeHandle); // The client is gone so reset pipe. 
		m_clientConnected = FALSE;
	}

	return 0;
}

int HandleGatewayServer::Gateway() {
	HANDLE hHeap = GetProcessHeap();
	void* request = HeapAlloc(hHeap, 0, MAXPIPEFILESIZE);
	void* reply = HeapAlloc(hHeap, 0, MAXPIPEFILESIZE);

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0;
	BOOL fSuccess = FALSE;

	while (1) { // Loop until done reading
		fSuccess = ReadFile(m_pipeHandle, request, MAXPIPEFILESIZE, &cbBytesRead, NULL);

		if (!fSuccess || cbBytesRead == 0) {
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				std::cout << "INFO  ]> Broken pipe (Client disconnected). GetLastError: " << dec << GetLastError() << endl;
			}
			else {
				std::cout << "ERROR ]> ReadFile failed. GetLastError: " << dec << GetLastError() << endl;
			}
			break;
		}

		int orderID = ((int*)request)[0];

		if (orderID == RMO_ORDER_READPROCESSMEMORY) {
			RMORequestRPM rpmRequest;
			rpmRequest.order = ((int*)request)[0];
			rpmRequest.address = ((int*)request)[1];
			rpmRequest.size = ((int*)request)[2];
			HandleGatewayServer::RemoteReadProcessMemory(rpmRequest);
		}
		else {
			std::cout << "ERROR ]> Unknown order received in the request. orderID: " << dec << orderID << endl;
		}
	}

	// Flush the pipe to allow the client to read the pipe's contents before disconnecting.
	// Then disconnect the pipe, and close the handle to this pipe instance. 

	FlushFileBuffers(m_pipeHandle);
	DisconnectNamedPipe(m_pipeHandle);
	CloseHandle(m_pipeHandle);

	HeapFree(hHeap, 0, request);
	HeapFree(hHeap, 0, reply);

	std::cout << "OK    ]> Gateway closing." << endl;
	return 1;
}

BOOL HandleGatewayServer::RemoteReadProcessMemory(RMORequestRPM request) {
	RMOResponseRPM response;
	if (request.base) {
		LPVOID lpBase = NULL;
		HMODULE hMods[512];
		DWORD cb;
		DWORD flag;
		if (EnumProcessModulesEx(processHandle, hMods, sizeof(hMods), &cb, LIST_MODULES_ALL))
		{
			char szModName[MAX_PATH] = { NULL };
			for (int i = 0; i < cb / sizeof(HMODULE); i++)
			{
				if (GetModuleBaseNameA(processHandle, hMods[i], szModName, MAX_PATH))
				{
					if (!strcmp(szModName, "TslGame.exe"))
					{
						std::cout << hMods[i] << " : " << szModName << std::endl;
						lpBase = hMods[i];
						std::cout << "Base Address: 0x" << (DWORD64)lpBase << std::endl;
						break;
					}
				}
				ZeroMemory(szModName, MAX_PATH);
			}
		}
		response.status = (DWORD_PTR)lpBase;
	}
	else
	{
		//response.status = ReadProcessMemory(pHandle, (LPCVOID)request.address, &response.bytes, request.size, &response.bytesRead);

		// TODO: Maybe check if the handle is an existing/valid one? Or fuck it, RPM return suffice probably
		response.status = ReadProcessMemory((HANDLE)processHandle, (LPCVOID)request.address, &response.bytes, request.size, &response.bytesRead);
		if (response.status == 0) {
			std::cout << "ERROR ]> ReadProcessMemory failed!" << endl;
		}

		BOOL fSuccess = FALSE;
		DWORD bytesWritten = 0;
		fSuccess = WriteFile(m_pipeHandle, &response, sizeof(response), &bytesWritten, NULL);
		if (!fSuccess) {
			std::cout << "ERROR ]> WriteFile failed!" << endl;
		}
		else {
			std::cout << "OK    ]> Response sent (" << dec << bytesWritten << " bytes written)" << endl;
		}
	}
	
		

	return TRUE;
}
