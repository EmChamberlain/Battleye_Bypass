#pragma once

#include <Windows.h>
#include <vector>
#include <map>

using byte = BYTE;
using uint64_t = ULONGLONG;
using int64_t = LONGLONG;
using uint32_t = UINT32;
using int32_t = INT32;
using uint16_t = USHORT;
using int16_t = SHORT;


enum PROTO_MESSAGE {
	PROTO_NORMAL_READ = 0,
	PROTO_GET_BASEADDR = 1
};


struct Vector3
{
	float X;
	float Y;
	float Z;
};

struct Player
{
	int32_t team;
	Vector3 loc;

	Player(int32_t t, Vector3 l)
	{
		team = t;
		loc = l;
	}
};

struct Vehicle
{
	std::string name;
	Vector3 loc;

	Vehicle(std::string n, Vector3 l)
	{
		name = n;
		loc = l;
	}
};

struct Item
{
	std::string name;
	Vector3 loc;

	Item(std::string n, Vector3 l)
	{
		name = n;
		loc = l;
	}
};

struct EncryptedActor
{
	uint64_t ptr_table[0x2B];
	uint16_t index;
	byte  unk2[0x6];
	uint16_t xor;
	byte  unk3[0x6];
};

