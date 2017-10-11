#pragma once
#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>
#include <iostream>

#define PIPEWAITTIMOUTIFBUSY 20000

#define BUFSIZE 100 // * sizeof(char)
#define MAXPIPEFILESIZE 1024 // 1Kb max file size
// At init() I could calculate the sizeof() all the requests and responses struct and use the largest as maxpipefilesize

#define RMO_ORDER_PING 0 // TODO, for monitoring purposes?
#define RMO_ORDER_READPROCESSMEMORY 1
#define RMO_ORDER_WRITEPROCESSMEMORY 2


template<typename T>
struct ResponseType {
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	T read;
};



typedef struct RMORequestRPM RMORequestRPM;
struct RMORequestRPM {
	int order = 0;
	int address = 0;
	int size = BUFSIZE;
	bool base = false;
};

typedef struct RMOResponseRPM RMOResponseRPM;
struct RMOResponseRPM {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	byte bytes[BUFSIZE];
};

/*class HandleGatewayServer {
public:
	int HandleGatewayServer::Init();
	int HandleGatewayServer::Gateway();
	BOOL HandleGatewayServer::RemoteReadProcessMemory(RMORequestRPM request);

protected:
	BOOL m_clientConnected = FALSE;
	DWORD  m_threadId = 0;
	HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
	LPTSTR m_lpszPipename = TEXT("\\\\.\\pipe\\serverpipe");
};*/

// ----------------------------------------------------

class HandleGatewayClient {
public:
	int HandleGatewayClient::ConnectPipe();
	BOOL HandleGatewayClient::DisconnectPipe();
	int HandleGatewayClient::SetPipeMode(DWORD mode = PIPE_READMODE_BYTE);

	// Functions to ReadProcessMemory remotely
	bool HandleGatewayClient::RequestReadProcessMemory(RMORequestRPM rpmRequest);
	RMOResponseRPM HandleGatewayClient::ReceiveReadProcessMemory();
	RMOResponseRPM HandleGatewayClient::RemoteReadProcessMemory(RMORequestRPM rpmRequest); 
	template<typename T> ResponseType<T> HandleGatewayClient::RemoteReadType(RMORequestRPM request);

protected:
	HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
	LPTSTR m_lpszPipename = TEXT("\\\\.\\pipe\\serverpipe");
};