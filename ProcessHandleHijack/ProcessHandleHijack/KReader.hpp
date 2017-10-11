#pragma once

#include <windows.h>
#include <psapi.h>
#include <string>
#include <exception>
#include "Types.hpp"
#include "RemoteMemoryOps.h"


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
		int64_t base = 0;
		// get the base address
		/*
		RMORequestRPM request;
		ResponseType<int64_t> response;
		request.order = 1;
		request.address = NULL;
		request.size = sizeof(int64_t);
		request.base = true;
		response = gatewayClient.RemoteReadProcessMemory(request);

		base = (uint64_t)response.bytes;
		if (base)
		{
			m_PUBase = base;
		}*/

		return base;
	}

	template<typename T>
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
			response = gatewayClient.RemoteReadType(request);
		}

		return (T)response.bytes;
	}

	// caution: if you use this method, you need to delete the allocated heap from where ever you called this method from
	byte* readSize(const int64_t& w_read, const int32_t& w_readSize, const PROTO_MESSAGE& w_protoMsg)
	{
		RMORequestRPM request;
		RMOResponseRPM response;
		// memset(paluu, 0, readSize);

		if (w_protoMsg == PROTO_NORMAL_READ)
		{
			//readStruct rStruct{ (uint64_t)writeMe, (uint64_t)w_read, w_readSize - 2, (uint32_t)GetCurrentProcessId(), 0, TRUE, 0 };
			// send the struct to IOCTL
			//WriteFile(m_hDriver, (LPCVOID)&rStruct, sizeof(ReadStruct), NULL, NULL);
			
			request.order = 1;
			request.address = w_read;
			request.size = w_readSize - 2;
			response = gatewayClient.RemoteReadProcessMemory(request);

		}

		if (response.status == 0)
		{
			return NULL;
		}

		return response.bytes;
	}
	

	// this needs to be fixed, its currently a workaround of just reading 3 floats.
	Vector3 readVec(const int64_t& w_read, const PROTO_MESSAGE& w_protoMsg)
	{
		/*
		RMORequestRPM request;
		RMOResponseRPM response;

		if (w_protoMsg == PROTO_NORMAL_READ)
		{
			//readStruct luku{ (uint64_t)&writeMe, (uint64_t)w_read, sizeof(Vector3), (uint32_t)GetCurrentProcessId(), 0, TRUE, 0 };
			// send the struct to IOCTL
			//WriteFile(m_hDriver, (LPCVOID)&luku, sizeof(ReadStruct), NULL, NULL);
			request.order = 1;
			request.address = w_read;
			request.size = sizeof(Vector3);
			response = gatewayClient.RemoteReadProcessMemory(request);
		}

		return (Vector3)(response.bytes);*/
		
		Vector3 vec;
		vec.X = bytesToFloat(readSize(w_read, sizeof(float), w_protoMsg));
		vec.Y = bytesToFloat(readSize(w_read + sizeof(float), sizeof(float), w_protoMsg));
		vec.Z = bytesToFloat(readSize(w_read + (2*sizeof(float)), sizeof(float), w_protoMsg));
		return vec;

	}

	// returns a string, if this method fails, returns "FAIL"
	std::string getGNameFromId(const int32_t& w_id)
	{
		int64_t GNames = readType<int64_t>(m_PUBase + 0x36DA610, PROTO_NORMAL_READ);
		int64_t singleNameChunk = readType<int64_t>(GNames + (w_id / 0x4000) * 8, PROTO_NORMAL_READ);
		int64_t singleNamePtr = readType<int64_t>(singleNameChunk + 8 * (w_id % 0x4000), PROTO_NORMAL_READ);

		char* name;
		if ((name = (char*)readSize(singleNamePtr + 16, 64, PROTO_NORMAL_READ)) != NULL)
		{
			std::string s = std::string(name);
			delete name;
			return s;
		}
		else
		{
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