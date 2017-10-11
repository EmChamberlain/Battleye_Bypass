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

bool readLoop = false;

// ugly to define the wanted items / player gnames this way.
std::vector<std::string> playerGNameVec = { "PlayerMale", "PlayerFemale" };
std::vector<std::string> vehicleGNameVec = { "Uaz", "Buggy", "Dacia", "ABP_Motorbike", "BP_Motorbike", "Boat_PG117" };
std::map<std::string, std::string> dropGNameMap = { { "Item_Head_G_01_Lv3_C", "Helm3" },{ "Item_Head_G_01_Lv3_", "Helm3" },{ "Item_Armor_C_01_Lv3", "Vest3" },{ "Item_Armor_C_01_Lv3_C", "Vest3" },{ "Item_Equip_Armor_Lv3_C", "Vest3" },{ "Item_Equip_Armor_Lv3", "Vest3" },{ "Item_Attach_Weapon_Muzzle_Suppressor_SniperRifle", "Supp(SR)" },{ "Item_Attach_Weapon_Muzzle_Suppressor_Large", "Supp(AR)" },{ "Item_Attach_Weapon_Muzzle_Suppressor_Large_C", "Supp(SR)" },{ "Item_Heal_MedKit", "Meds" },{ "Item_Heal_FirstAid", "Meds" },{ "Item_Weapon_Kar98k", "kar98" },{ "Item_Weapon_Mini14", "mini" },{ "Item_Weapon_M16A4", "M16" },{ "Item_Weapon_HK416", "m416" },{ "Item_Weapon_SCAR-L", "SCAR" },{ "Item_Weapon_SKS", "sks" },{ "Item_Attach_Weapon_Upper_ACOG_01", "4x" },{ "Item_Attach_Weapon_Upper_CQBSS", "8x" },{ "Item_Attach_Weapon_Upper_CQBSS_C", "8x" } };


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