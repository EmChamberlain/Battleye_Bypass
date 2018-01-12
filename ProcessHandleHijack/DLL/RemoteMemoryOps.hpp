#pragma once
#include <windows.h> 
#include <stdio.h> 
#include <tchar.h>
#include <strsafe.h>
#include <iostream>
#include <fstream>

#define BUFSIZE 100 // * sizeof(char)
#define MAXPIPEFILESIZE 1024 // 1Kb max file size
// At init() I could calculate the sizeof() all the requests and responses struct and use the largest as maxpipefilesize

#define RMO_ORDER_PING 0 // TODO, for monitoring purposes?
#define RMO_ORDER_READPROCESSMEMORY 1
#define RMO_ORDER_WRITEPROCESSMEMORY 2

struct Vector3
{
	float X;
	float Y;
	float Z;
};
struct EncryptedActor
{
	uint64_t ptr_table[0x2B];
	uint16_t index;
	byte  unk2[0x6];
	uint16_t xor;
	byte  unk3[0x6];
};


typedef struct int128 {
	LONGLONG low;
	LONGLONG high;
} int128;


typedef struct RMOResponseRPM128 RMOResponseRPM128;
struct RMOResponseRPM128 {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	int128 val;
};


struct RMOResponseRPM64 {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	LONGLONG val;
};



struct RMOResponseRPM32 {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	INT32 val;
};

struct RMOResponseRPM8 {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	INT8 val;
};

struct RMOResponseRPMVec {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	Vector3 val;
};

struct RMOResponseRPMBytes {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	byte val[BUFSIZE];
};

struct RMOResponseRPMActor {
	// RMORequestRPM request;
	BOOL status = FALSE;
	SIZE_T bytesRead = 0;
	EncryptedActor encrActor;
};


struct RMORequestRPM {
	int order = 0;
	LONGLONG address = 0;
	int size = BUFSIZE;
	bool base = false;
};


/*struct RMOResponseRPM {
// RMORequestRPM request;
BOOL status = FALSE;
SIZE_T bytesRead = 0;
byte bytes[BUFSIZE];
};*/

class HandleGatewayServer {
public:
	int HandleGatewayServer::Init(LPWSTR);
	int HandleGatewayServer::Gateway();
	BOOL HandleGatewayServer::RemoteReadProcessMemory(RMORequestRPM request);
	LPWSTR processName = NULL;
	HANDLE processHandle = NULL;
	//std::ofstream logFile = std::ofstream("log.log", std::ios::out | std::ios::trunc);

protected:
	BOOL m_clientConnected = FALSE;
	DWORD  m_threadId = 0;
	HANDLE m_pipeHandle = INVALID_HANDLE_VALUE;
	LPTSTR m_lpszPipename = TEXT("\\\\.\\pipe\\serverpipe");

};