
#pragma once


#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>
#include <iostream>

#include "Types.hpp"
//#include "def.h"


// ugly to define the wanted items / player gnames this way.
extern std::vector<std::string> playerGNameVec;
extern std::vector<std::string> vehicleGNameVec;
extern std::map<std::string, std::string> dropGNameMap;

#define UWORLD 0x407C4C0 // sig = 48 89 05 ? ? ? ? 0F 28 D6
#define GNAMES 0x3F5E910 // sig = 75 BB 48 8B 35 ? ? ? ? 41 0F B7 C4
//#define ETABLE 0x3F3F500  //GObjsAddress // sig = 48 8d 0d ? ? ? ? c6 05 ? ? ? ? 01 e8 ? ? ? ? c6

#define PIPEWAITTIMOUTIFBUSY 20000

#define BUFSIZE 0x125 // * sizeof(char)
#define MAXPIPEFILESIZE 1024 // 1Kb max file size
// At init() I could calculate the sizeof() all the requests and responses struct and use the largest as maxpipefilesize

#define RMO_ORDER_PING 0 // TODO, for monitoring purposes?
#define RMO_ORDER_READPROCESSMEMORY 1
#define RMO_ORDER_WRITEPROCESSMEMORY 2

typedef struct int128 {
	uint64_t low;
	uint64_t high;
} int128;


typedef struct RMOResponseRPM128 RMOResponseRPM128;
struct RMOResponseRPM128 {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	int128 val;
};

typedef struct RMOResponseRPM64 RMOResponseRPM64;
struct RMOResponseRPM64 {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	LONGLONG val;
};



typedef struct RMOResponseRPM32 RMOResponseRPM32;
struct RMOResponseRPM32 {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	INT32 val;
};

typedef struct RMOResponseRPM16 RMOResponseRPM16;
struct RMOResponseRPM16 {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	INT16 val;
};

typedef struct RMOResponseRPM8 RMOResponseRPM8;
struct RMOResponseRPM8 {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	INT8 val;
};



typedef struct RMOResponseRPMVec RMOResponseRPMVec;
struct RMOResponseRPMVec {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	Vector3 val;
};

typedef struct RMOResponseRPMBytes RMOResponseRPMBytes;
struct RMOResponseRPMBytes {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	byte val[BUFSIZE];
};

typedef struct RMOResponseRPMActor RMOResponseRPMActor;
struct RMOResponseRPMActor {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	EncryptedActor encrActor;
};


typedef struct RMORequestRPM RMORequestRPM;
struct RMORequestRPM {
	int order = 0;
	LONGLONG address = 0;
	int size = BUFSIZE;
	bool base = false;
	Vector3 toWrite;
};

/*typedef struct RMOResponseRPM RMOResponseRPM;
struct RMOResponseRPM {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	byte bytes[BUFSIZE];
};*/

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
	RMOResponseRPM128 HandleGatewayClient::ReceiveReadProcessMemory128();
	RMOResponseRPM128 HandleGatewayClient::RemoteReadProcessMemory128(RMORequestRPM rpmRequest);

	RMOResponseRPM64 HandleGatewayClient::ReceiveReadProcessMemory64();
	RMOResponseRPM64 HandleGatewayClient::RemoteReadProcessMemory64(RMORequestRPM rpmRequest); 

	RMOResponseRPM32 HandleGatewayClient::ReceiveReadProcessMemory32();
	RMOResponseRPM32 HandleGatewayClient::RemoteReadProcessMemory32(RMORequestRPM rpmRequest);

	RMOResponseRPM16 HandleGatewayClient::ReceiveReadProcessMemory16();
	RMOResponseRPM16 HandleGatewayClient::RemoteReadProcessMemory16(RMORequestRPM rpmRequest);

	RMOResponseRPM8 HandleGatewayClient::ReceiveReadProcessMemory8();
	RMOResponseRPM8 HandleGatewayClient::RemoteReadProcessMemory8(RMORequestRPM rpmRequest);

	RMOResponseRPMVec HandleGatewayClient::ReceiveReadProcessMemoryVec();
	RMOResponseRPMVec HandleGatewayClient::RemoteReadProcessMemoryVec(RMORequestRPM rpmRequest);

	RMOResponseRPMBytes HandleGatewayClient::ReceiveReadProcessMemoryBytes();
	RMOResponseRPMBytes HandleGatewayClient::RemoteReadProcessMemoryBytes(RMORequestRPM rpmRequest);

	RMOResponseRPMActor HandleGatewayClient::ReceiveReadProcessMemoryActor();
	RMOResponseRPMActor HandleGatewayClient::RemoteReadProcessMemoryActor(RMORequestRPM rpmRequest);

	void RemoteWriteProcessMemoryVec(RMORequestRPM rpmRequest);

protected:
	HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
	LPTSTR m_lpszPipename = TEXT("\\\\.\\pipe\\serverpipe");
};
