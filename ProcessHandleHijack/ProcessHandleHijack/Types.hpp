#pragma once

#include <Windows.h>
#include <vector>
#include <map>
#include <xlocmon>

/*using byte = BYTE;
using uint64_t = ULONGLONG;
using int64_t = LONGLONG;
using uint32_t = UINT32;
using int32_t = INT32;
using uint16_t = USHORT;
using int16_t = SHORT;*/


enum PROTO_MESSAGE {
	PROTO_NORMAL_READ = 0,
	PROTO_GET_BASEADDR = 1
};

enum StanceMode
{
	none = 0,
	stand = 1,
	crouch = 2,
	prone = 3,
	max = 4
};

struct Vector3
{
	float X;
	float Y;
	float Z;


	Vector3()
	{
		X = 0;
		Y = 0;
		Z = 0;
	}

	Vector3(float x, float y, float z)
	{
		X = x;
		Y = y;
		Z = z;
	}
	Vector3 operator-(Vector3 i) {
		X -= i.X;
		Y -= i.Y;
		Z -= i.Z;
		return *this;
	}
	Vector3 operator+(Vector3 i) {
		X += i.X;
		Y += i.Y;
		Z += i.Z;
		return *this;
	}
	Vector3 operator/(Vector3 i) {
		X /= i.X;
		Y /= i.Y;
		Z /= i.Z;
		return *this;
	}
	friend std::ostream & operator<<(std::ostream & Str, Vector3 const & v) {
		// print something from v to str, e.g: Str << v.getX();
		Str << "(" << v.X << "," << v.Y << "," << v.Z << ")";
		return Str;
	}
	float length()
	{
		return sqrt(X*X + Y*Y + Z*Z);
	}
};

struct Player
{
	int32_t team;
	Vector3 loc;
	StanceMode stance;

	Player(int32_t t, Vector3 l)
	{
		team = t;
		loc = l;
		stance = none;
	}
	Player(int32_t t, Vector3 l, StanceMode s)
	{
		team = t;
		loc = l;
		stance = s;
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



