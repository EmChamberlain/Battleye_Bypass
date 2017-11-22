#pragma once

#include <windows.h>
#include <psapi.h>
#include <string>
#include <exception>
#include "RemoteMemoryOps.hpp"


class KReader
{
public:
	KReader() : m_PUBase(0)
	{
		gatewayClient.ConnectPipe();
	}
	~KReader()
	{
		gatewayClient.DisconnectPipe();
	}

	/*
	* CLASS METHODS
	*/
	int64_t readPUBase()
	{
		RMORequestRPM request;
		RMOResponseRPM64 response;
		request.order = 1;
		request.address = 0;
		request.size = sizeof(int64_t);
		request.base = true;
		response = gatewayClient.RemoteReadProcessMemory64(request);
		//m_PUBase = 0x7ff712590000;
		//return 0x7ff712590000;
		m_PUBase = response.val;
		return response.val;
	}

	/*template<typename T>
	T readType(const int64_t& w_read, const PROTO_MESSAGE& w_protoMsg)
	{
		//T writeMe;
		RMORequestRPM request;
		RMOResponseRPM response;

		if (w_protoMsg == PROTO_NORMAL_READ)
		{
			//readStruct rStruct{ (uint64_t)&writeMe, (uint64_t)w_read, sizeof(T), (uint32_t)GetCurrentProcessId(), 0, TRUE, 0 };
			// send the struct to IOCTL
			//WriteFile(m_hDriver, (LPCVOID)&rStruct, sizeof(ReadStruct), NULL, NULL);
			request.order = 1;
			request.address = w_read;
			request.size = sizeof(T);
			response = gatewayClient.RemoteReadProcessMemory(request);
		}

		return (T)response.bytes;
	}*/
	int64_t readType64(const int64_t& w_read, const PROTO_MESSAGE& w_protoMsg)
	{
		//T writeMe;
		RMORequestRPM request;
		RMOResponseRPM64 response;

		if (w_protoMsg == PROTO_NORMAL_READ)
		{
			//readStruct rStruct{ (uint64_t)&writeMe, (uint64_t)w_read, sizeof(T), (uint32_t)GetCurrentProcessId(), 0, TRUE, 0 };
			// send the struct to IOCTL
			//WriteFile(m_hDriver, (LPCVOID)&rStruct, sizeof(ReadStruct), NULL, NULL);
			request.order = 1;
			request.address = w_read;
			request.size = sizeof(int64_t);
			response = gatewayClient.RemoteReadProcessMemory64(request);
		}

		return response.val;
	}
	int32_t readType32(const int64_t& w_read, const PROTO_MESSAGE& w_protoMsg)
	{
		//T writeMe;
		RMORequestRPM request;
		RMOResponseRPM32 response;

		if (w_protoMsg == PROTO_NORMAL_READ)
		{
			//readStruct rStruct{ (uint64_t)&writeMe, (uint64_t)w_read, sizeof(T), (uint32_t)GetCurrentProcessId(), 0, TRUE, 0 };
			// send the struct to IOCTL
			//WriteFile(m_hDriver, (LPCVOID)&rStruct, sizeof(ReadStruct), NULL, NULL);
			request.order = 1;
			request.address = w_read;
			request.size = sizeof(int32_t);
			response = gatewayClient.RemoteReadProcessMemory32(request);
		}

		return response.val;
	}
	Vector3 readTypeVec(const int64_t& w_read, const PROTO_MESSAGE& w_protoMsg)
	{
		//T writeMe;
		RMORequestRPM request;
		RMOResponseRPMVec response;

		if (w_protoMsg == PROTO_NORMAL_READ)
		{
			//readStruct rStruct{ (uint64_t)&writeMe, (uint64_t)w_read, sizeof(T), (uint32_t)GetCurrentProcessId(), 0, TRUE, 0 };
			// send the struct to IOCTL
			//WriteFile(m_hDriver, (LPCVOID)&rStruct, sizeof(ReadStruct), NULL, NULL);
			request.order = 1;
			request.address = w_read;
			request.size = sizeof(Vector3);
			response = gatewayClient.RemoteReadProcessMemoryVec(request);
		}

		return response.val;
	}

	// caution: if you use this method, you need to delete the allocated heap from where ever you called this method from
	RMOResponseRPMBytes* readSize(const int64_t& w_read, const int32_t& w_readSize, const PROTO_MESSAGE& w_protoMsg)
	{
		RMORequestRPM request;
		RMOResponseRPMBytes *response = new RMOResponseRPMBytes;
		// memset(paluu, 0, readSize);

		if (w_protoMsg == PROTO_NORMAL_READ)
		{
			//readStruct rStruct{ (uint64_t)writeMe, (uint64_t)w_read, w_readSize - 2, (uint32_t)GetCurrentProcessId(), 0, TRUE, 0 };
			// send the struct to IOCTL
			//WriteFile(m_hDriver, (LPCVOID)&rStruct, sizeof(ReadStruct), NULL, NULL);
			
			request.order = 1;
			request.address = w_read;
			request.size = w_readSize - 2;
			*response = gatewayClient.RemoteReadProcessMemoryBytes(request);

		}

		if (response->status == 0)
		{
			return NULL;
		}

		return response;
	}
	

	// returns a string, if this method fails, returns "FAIL"
	std::string getGNameFromId(const int32_t& w_id)
	{
		int64_t GNames = readType64(m_PUBase + GNAMES, PROTO_NORMAL_READ);//0x36E9790 for live
		int64_t singleNameChunk = readType64(GNames + (w_id / 0x4000) * 8, PROTO_NORMAL_READ);
		int64_t singleNamePtr = readType64(singleNameChunk + 8 * (w_id % 0x4000), PROTO_NORMAL_READ);

		char* name;
		RMOResponseRPMBytes *response = readSize(singleNamePtr + 16, 64, PROTO_NORMAL_READ);
		name = (char*)(response->val);
		if (response != NULL)
		{
			std::string s = std::string(name);
			delete response;
			return s;
		}
		else
		{
			delete response;
			return std::string("FAIL");
		}
		
	}

	/*
	* GETTERS AND SETTERS
	*/
	HANDLE getKernHandle() const
	{
		return m_hDriver;
	}

	int64_t getPUBase() const
	{
		return m_PUBase;
	}

	bool isReading() const
	{
		return m_readActive;
	}

private:
	float bytesToFloat(byte* bytes)
	{
		float output;

		*((unsigned char*)(&output) + 3) = bytes[0];
		*((unsigned char*)(&output) + 2) = bytes[1];
		*((unsigned char*)(&output) + 1) = bytes[2];
		*((unsigned char*)(&output) + 0) = bytes[3];

		return output;
	}

	HANDLE m_hDriver;
	int64_t m_PUBase;
	bool m_readActive;
	HandleGatewayClient gatewayClient;

};
