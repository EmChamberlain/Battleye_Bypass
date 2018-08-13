#pragma once

#include "KReader.hpp"

#include <deque>
#include <vector>
#include "def.h"
//#include "uint128_t.h"
#define READ32(addr) m_kReader->readType32(addr, PROTO_NORMAL_READ)
#define READ64(addr) m_kReader->readType64(addr, PROTO_NORMAL_READ)

#define GET_ADDR(addr) (m_kReader->getPUBase() + addr)

#define BYTEn(x, n) (*((BYTE *)&(x) + n))
#define WORDn(x, n) (*((WORD *)&(x) + n))
#define DWORDn(x, n) (*((DWORD *)&(x) + n))

#define IDA_LOBYTE(x) BYTEn(x, 0)
#define IDA_LOWORD(x) WORDn(x, 0)
#define IDA_LODWORD(x) WORDn(x, 0)
#define IDA_HIBYTE(x) BYTEn(x, 1)
#define IDA_HIWORD(x) WORDn(x, 1)
#define IDA_HIDWORD(x) DWORDn(x, 1)


#define BYTE1(x) BYTEn(x, 1)
#define BYTE2(x) BYTEn(x, 2)
#define WORD1(x) WORDn(x, 1)
#define DWORD1(x) DWORDn(x, 1)

class GameDataParser
{
public:

	GameDataParser()
	{
		m_kReader = new KReader;
		readPUBase();
		oneTimeLocals();
		//pubgdec::decinit(m_kReader);
	}
	~GameDataParser()
	{
		delete m_kReader;
	}


	void readLoop()
	{
		if (getPUBase() == 0)
			readPUBase();
		readLocals();
		readPlayers();
	}

	int64_t getPUBase()
	{
		return m_kReader->getPUBase();
	}

	int64_t readPUBase()
	{
		return m_kReader->readPUBase();
	}

	void writeAng(Vector3 in)
	{
		m_kReader->writeVec(m_playerController + 0x03E0, in);
	}



	/*
	* These vectors house the up to date aactors.
	*/
	bool writingVectors = false;
	std::vector<Player> *players;
	std::vector<Vehicle> *vehicles;
	std::vector<Item> *items;
	std::vector<Player> *playersOld;
	std::vector<Vehicle> *vehiclesOld;
	std::vector<Item> *itemsOld;

	/*
	* Local variables
	* These are updated once every read loop.
	*/
	int64_t m_BaseUWorld;
	int64_t m_UWorld;
	int64_t m_GNames;
	int64_t m_gameInstance;
	int64_t m_ULocalPlayer;
	int64_t m_localPlayer;
	int64_t m_viewportclient;
	int64_t m_localPawn;
	int64_t m_localPlayerState;
	//int64_t m_PWorld;
	int64_t m_ULevel;
	int64_t m_playerCount;
	Vector3 m_localPlayerPosition;
	//int64_t m_localPlayerBasePointer;
	int32_t m_localTeam;
	int64_t m_AActorPtr;
	Vector3 m_originVector;
	int32_t m_XOriginLocation;
	int32_t m_YOriginLocation;
	int32_t m_ZOriginLocation;
	int64_t m_playerController;
	int64_t m_playerCameraManager;
	//Vector3 m_playerCameraLocation;
	Vector3 m_playerCameraRotation;
	Vector3 m_localPlayerPositionCamera;
	int64_t m_encryptionTable;
	int64_t m_localPlayerCharacterMovement;
	int8_t m_localPlayerStance;
	Vector3 m_localPlayerRotation;


	int64_t m_weaponProcessor;
	int32_t m_currentWeaponIndex;
	int64_t m_equippedWeapons;
	int64_t m_currentWeapon;
	Vector3 m_currentWeaponRecoilInfoVert;


	bool needToWriteAng = false;
	Vector3 toWriteAng = Vector3(89.0, 89.0, 0.0);


private:

	/*
	* PRIVATE CLASS FUNCTIONS
	*/
	void readPlayers()
	{
		std::vector<Player> *playersTemp = new std::vector<Player>();
		std::vector<Vehicle> *vehiclesTemp = new std::vector<Vehicle>();
		std::vector<Item> *itemsTemp = new std::vector<Item>();
		for (int i = 0; i < m_playerCount; i++)
		{


			// read the position of Player
			int64_t curActor = m_kReader->readType64(m_AActorPtr + (i * 0x8), PROTO_NORMAL_READ);
			//int64_t curActor = pubgdec::decptr(m_kReader, m_AActorPtr + (i * 0x180));//#define O_ENCRYPTEDENTITYSIZE 0x180
			if (curActor == NULL)
				continue;
			int32_t curActorID = tsl_decrypt_ID(READ32(curActor + 0x14));// 0x0018 live server
			if (curActorID == NULL)
				continue;
			std::string actorGName = GetGNameFromId(m_GNames, curActorID);

			// Here we check if the name is found from the wanted GNames list (PlayerMale etc...)
			if (std::find(playerIDs.begin(), playerIDs.end(), curActorID) != playerIDs.end())
			{
				//int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x0198, PROTO_NORMAL_READ);//USceneComponent //0x180 live server
				int64_t rootCmpPtr = tsl_decrypt_rootComponent(curActor + 0x220);//USceneComponent //0x180 live server
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x2D0, PROTO_NORMAL_READ);//FVector    Location //0x1A0 live server
				int64_t playerState = READ64(curActor + 0x0420);//0x3C0 live server
				int32_t actorTeam = READ64(playerState + 0x718);//0x04CC old

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;
				actorLocation.Z += m_ZOriginLocation;

				//int64_t movement = m_kReader->readType64(curActor + 0x418, PROTO_NORMAL_READ);
				//StanceMode stance = StanceMode(m_kReader->readType8(movement + 0x4D0, PROTO_NORMAL_READ));
				StanceMode stance = stand;

				//w_data["players"].emplace_back(json::object({ { "t", actorTeam },{ "x", actorLocation.X },{ "y", actorLocation.Y }/*,{ "z", actorLocation.Z } }));
				playersTemp->push_back(Player(actorTeam, actorLocation, stance));
			}
			else if (actorGName == "DroppedItemGroup" || actorGName == "DroppedItemInteractionComponent")
			{

				//int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x198, PROTO_NORMAL_READ);
				int64_t rootCmpPtr = tsl_decrypt_rootComponent(curActor + 0x220);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x2D0, PROTO_NORMAL_READ);
				int64_t DroppedItemArray = m_kReader->readType64(curActor + 0x278, PROTO_NORMAL_READ);//0x2F8 old //unsigned char    UnknownData14[0xA0]
				int32_t DroppedItemCount = m_kReader->readType32(curActor + 0x280, PROTO_NORMAL_READ);//0x300 old

				for (int j = 0; j < DroppedItemCount; j++)
				{
					int64_t ADroppedItem = m_kReader->readType64(DroppedItemArray + j * 0x10, PROTO_NORMAL_READ);
					Vector3 droppedLocation = m_kReader->readTypeVec(ADroppedItem + 0x2C8, PROTO_NORMAL_READ);
					droppedLocation.X = droppedLocation.X + actorLocation.X + m_XOriginLocation;
					droppedLocation.Y = droppedLocation.Y + actorLocation.Y + m_YOriginLocation;
					int64_t UItem = m_kReader->readType64(ADroppedItem + 0x5E8, PROTO_NORMAL_READ);
					int32_t UItemID = tsl_decrypt_ID(curActor + 0x20);// 0x0018 live server
					std::string itemName = m_kReader->getGNameFromId(UItemID, m_GNames);

					// check if inside the map / array of wanted items
					for (std::map<std::string, std::string>::iterator it = dropGNameMap.begin(); it != dropGNameMap.end(); ++it)
					{
						if (itemName.substr(0, it->first.length()) == it->first)
						{
							//actorLocation.X += m_XOriginLocation;
							//actorLocation.Y += m_YOriginLocation;

							//w_data["items"].emplace_back(json::object({ { "n", it->second },{ "x", droppedLocation.X },{ "y", droppedLocation.Y } }));
							itemsTemp->push_back(Item(it->second, droppedLocation));
						}
					}
				}
			}
			else if (actorGName.substr(0, strlen("CarePackage")) == "CarePackage" || actorGName.substr(0, strlen("AircraftCarePackage")) == "AircraftCarePackage" || actorGName.substr(0, strlen("Carapackage_RedBox")) == "Carapackage_RedBox" || actorGName.substr(0, strlen("Carapackage")) == "Carapackage")
			{
				//int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x0198, PROTO_NORMAL_READ);//USceneComponent //0x180 live server
				int64_t rootCmpPtr = tsl_decrypt_rootComponent(curActor + 0x220);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x2D0, PROTO_NORMAL_READ);//FVector    Location //0x1A0 live server

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;

				//w_data["vehicles"].emplace_back(json::object({ { "v", "Drop" },{ "x", actorLocation.X },{ "y", actorLocation.Y } }));
				vehiclesTemp->push_back(Vehicle("Drop", actorLocation));

			}
			else if (std::find(vehicleIDs.begin(), vehicleIDs.end(), curActorID) != vehicleIDs.end())
			{
				// tästä alaspäin voi tehdä if-lohkoissa
				//int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x0198, PROTO_NORMAL_READ);//USceneComponent //0x180 live server
				int64_t rootCmpPtr = tsl_decrypt_rootComponent(curActor + 0x220);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x2D0, PROTO_NORMAL_READ);//FVector    Location //0x1A0 live server

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;

				//w_data["vehicles"].emplace_back(json::object({ { "v", carName.substr(0, 3) },{ "x", actorLocation.X },{ "y", actorLocation.Y } }));
				vehiclesTemp->push_back(Vehicle(actorGName.substr(0, 6), actorLocation));
			}
			else if (std::find(allIDs.begin(), allIDs.end(), curActorID) == allIDs.end())
			{
				allIDs.push_back(curActorID);
				if (actorGName == "FAIL")
				{
					continue;
				}
				else
				{
					// iterate thru playerGnameVec
					for (std::vector<std::string>::iterator it = playerGNameVec.begin(); it != playerGNameVec.end(); ++it)
					{
						//check if the name is same, and add it to the playerIDs vector
						if (*it == actorGName.substr(0, (*it).length()))
						{
							playerIDs.push_back(curActorID);
							break;
						}
					}

					// iterate thru vehicleGNameVec
					for (std::vector<std::string>::iterator it = vehicleGNameVec.begin(); it != vehicleGNameVec.end(); ++it)
					{
						//check if the name is same, and add it to the vehicleIDs vector
						if (*it == actorGName.substr(0, (*it).length()))
						{
							vehicleIDs.push_back(curActorID);
							break;
						}
					}
				}
			}
			/*else
			{

				if (actorGName == "FAIL")
				{
					continue;
				}
				else
				{
					// iterate thru playerGnameVec
					for (std::vector<std::string>::iterator it = playerGNameVec.begin(); it != playerGNameVec.end(); ++it)
					{
						//check if the name is same, and add it to the playerIDs vector
						if (*it == actorGName.substr(0, (*it).length()))
						{
							playerIDs.push_back(curActorID);
							break;
						}
					}


				}
			}*/



			if (allIDs.size() > 5000)
			{
				allIDs.clear();
				std::cout << "Clearing aids" << std::endl;
			}
			if (playerIDs.size() > 10)
			{
				playerIDs.clear();
				std::cout << "Clearing pids" << std::endl;
			}
		}
		writingVectors = true;
		delete playersOld;
		delete vehiclesOld;
		delete itemsOld;
		playersOld = players;
		vehiclesOld = vehicles;
		itemsOld = items;
		players = playersTemp; 
		vehicles = vehiclesTemp;
		items = itemsTemp;
		writingVectors = false;

	}

#define STATIC_GNAMES 0x45450F0  // (48 8D 3D ? ? ? ? F3 48 AB 48 8D 3D ? ? ? ? B9 ? ? ? ? F3 48 AB + 3) + 1FA0

#define GNAMES_CHUNK_SIZE 0x416C  // C1 ? ? 8B ? C1 ? 1F 03 ? 69 ? ? ? ? ? 48 63 ? + 12



	std::string GetGNameFromId(uintptr_t GN, int ID)
	{
		try
		{
			const uintptr_t fNamePtr = READ64(GN + int(ID / GNAMES_CHUNK_SIZE) * 8);
			const uintptr_t fName = READ64(fNamePtr + 8 * int(ID % GNAMES_CHUNK_SIZE));

			byte resultname[256]{ NULL };
			RMOResponseRPMBytes *response = m_kReader->readBytes(fName + 0x10, 0x100, PROTO_NORMAL_READ);
			if (!response)
				return "";
			memcpy(resultname, response->val, 0x100);
			delete response;

			return std::string(reinterpret_cast<const char*>(resultname));
		}
		catch (...)
		{
			return "";
		}
	}

	uintptr_t get_gnames()
	{
		uintptr_t gnames = 0;

		uintptr_t tmp = READ64(getPUBase() + STATIC_GNAMES);

		for (int i = 0; i < 0xFFFF; i++)
		{
			gnames = tmp - (i * 0x80) + 0x10;
			//gnames = tmp - (i * 0x80);
			if (
				GetGNameFromId(gnames, 1) == "ByteProperty"
				&& GetGNameFromId(gnames, 2) == "IntProperty"
				&& GetGNameFromId(gnames, 3) == "BoolProperty"
				)
			{
				return gnames;
			}
		}
		return 0;
	}

	void oneTimeLocals()
	{
		if (getPUBase() == 0)
			readPUBase();

		/*if (!tsl_init(&tsl)) {
			// what?
		}*/

		int64_t base = m_kReader->getPUBase();

		//m_UWorld = m_kReader->readType64(m_kReader->readType64(base + 0x3EB9580, PROTO_NORMAL_READ), PROTO_NORMAL_READ);
		//m_UWorld = DecryptData(base + 0x407D8C0, base + 0x5108F10);//0x407C8C0 //base 0x3B2A120		
		//m_UWorld = DecryptUWorld(base + 0x418F520, base + 0x3C34120);//0x3C34120



		//m_GNames = m_kReader->readType64(base + 0x3F28888, PROTO_NORMAL_READ);

		//m_GNames = DecryptData(DecryptData(DecryptData((base + 0x3F5FD10), base + 0x5108F10), base + 0x5108F10), base + 0x5108F10);//0x3F5ED10 //base 0x3B2A120
		//m_GNames = DecryptGName(base + 0xABBD70, base + 0x3C34120);//0x406F6A0
		//m_GNames = DecryptGName2(m_GNames, base + 0x3C34120);

		//m_BaseUWorld = m_kReader->readType64(m_kReader->getPUBase() + UWORLD, PROTO_NORMAL_READ);
		//m_UWorld = m_kReader->readType64(m_BaseUWorld, PROTO_NORMAL_READ);


		//m_GNames = m_kReader->readType64(m_kReader->getPUBase() + 0x3F5E8D8, PROTO_NORMAL_READ) + 0x30;
		//m_GNames = DecryptData(m_GNames, m_kReader->getPUBase() + 0x3B2A5F0);
		//m_GNames = m_kReader->readType64(m_kReader->getPUBase() + GNAMES, PROTO_NORMAL_READ);
		//m_GNames = m_kReader->readType64(m_GNames + 0xF8, PROTO_NORMAL_READ);
		//m_GNames = m_GNames + +0x530;
		//std::cout << "UWorld: " << std::hex << m_UWorld << std::endl;
		//std::cout << "GNames: " << std::hex << m_GNames << std::endl;

		/*m_gameInstance = m_kReader->readType64(m_UWorld + 0x0140, PROTO_NORMAL_READ);//UGameInstance    OwningGameInstance    //0x140 test?
		m_ULocalPlayer = m_kReader->readType64(m_gameInstance + 0x38, PROTO_NORMAL_READ);//TArray<class ULocalPlayer*>
																						 //m_ULocalPlayer = pubgdec::decptr(m_kReader, m_gameInstance + 0x38);
		m_localPlayer = m_kReader->readType64(m_ULocalPlayer, PROTO_NORMAL_READ);//UPlayer
		m_viewportclient = m_kReader->readType64(m_localPlayer + 0x58, PROTO_NORMAL_READ);//UGameViewportClient
		m_playerController = m_kReader->readType64(m_localPlayer + 0x30, PROTO_NORMAL_READ);//APlayerController
		m_localPawn = m_kReader->readType64(m_playerController + 0x0428, PROTO_NORMAL_READ);//APawn //0x3A8 live server //0x3B8 test?
		m_localPlayerState = m_kReader->readType64(m_localPawn + 0x3D0, PROTO_NORMAL_READ);//APlayerState //0x3C0 live server //0x3D0 test?
		m_PWorld = m_kReader->readType64(m_viewportclient + 0x80, PROTO_NORMAL_READ);//UWorld
		m_ULevel = m_kReader->readType64(m_PWorld + 0x30, PROTO_NORMAL_READ);//ULevel*/



		m_UWorld = m_kReader->readType64(base + 0x4116160, PROTO_NORMAL_READ);
		m_UWorld = m_kReader->readType64(m_UWorld, PROTO_NORMAL_READ);
		m_GNames = get_gnames();//READ64(get_gnames());

		m_gameInstance = tsl_decrypt_game_inst(m_UWorld + 0xa8);
		m_ULocalPlayer = READ64(m_gameInstance + 0xc0);
		m_localPlayer = tsl_decrypt_Local(m_ULocalPlayer);
		//m_viewportclient = READ64(m_localPlayer + 0xa0);

		m_playerController = tsl_decrypt_controller(m_localPlayer + 0x38);//currently wrong
		m_localPawn = tsl_decrypt_pawn(m_playerController + 0x430);//local_player_actor
		m_localPlayerState = READ64(m_localPawn + 0x0420);//old 0x3E0

		m_ULevel = tsl_decrypt_level(m_UWorld + 0xA08);

	}
	void readLocals()
	{
		/*if (needToWriteAng)
		{
			writeAng(toWriteAng);
			needToWriteAng = false;
			toWriteAng = Vector3(89.0, 89.0, 0.0);
		}*/


		//int64_t decryptedAActorPtr = DecryptData(m_ULevel + 0xA0, m_kReader->getPUBase() + 0x3D90770);// + 0xA0
		//uint64_t decryptedAActorPtr = DecryptActors(m_ULevel + 0xA0, m_kReader->getPUBase() + 0x3C34120);
		//uint64_t decryptedAActorPtr = DecryptActors(m_ULevel + 0xA0);
		uint64_t decryptedAActorPtr = tsl_decrypt_actor(m_ULevel + 0x1D0);
		m_AActorPtr = m_kReader->readType64(decryptedAActorPtr, PROTO_NORMAL_READ);//TArray<class AActor*>    AActors //0xA0 near actors //0xB0 all actors
		m_playerCount = m_kReader->readType32(decryptedAActorPtr + 0x08, PROTO_NORMAL_READ);//TArray<class AActor*> + 0x8 //0xA8 near actors //0xB8 all actors

		//m_AActorPtr = m_kReader->readType64(m_ULevel + 0xA0, PROTO_NORMAL_READ);//TArray<class AActor*>    AActors //0xA0 near actors //0xB0 all actors
		//m_playerCount = m_kReader->readType32(m_ULevel + 0xA8, PROTO_NORMAL_READ);//TArray<class AActor*> + 0x8 //0xA8 near actors //0xB8 all actors


		m_localPlayerPosition = m_kReader->readTypeVec(m_localPlayer + 0xB8, PROTO_NORMAL_READ);//old 0x78
		//m_localPlayerBasePointer = m_kReader->readType64(m_localPlayer, PROTO_NORMAL_READ);

		m_localTeam = m_kReader->readType32(m_localPlayerState + 0x718, PROTO_NORMAL_READ);//0x04CC old

		//int32_t myID = m_kReader->readType32(m_localPlayer + 0x0018, PROTO_NORMAL_READ);
		//std::string myGName = m_kReader->getGNameFromId(myID, m_GNames);





		m_XOriginLocation = m_kReader->readType32(m_UWorld + 0x958, PROTO_NORMAL_READ);//0x938 old //these tend to be before class UWorldComposition*                           WorldComposition;
		m_YOriginLocation = m_kReader->readType32(m_UWorld + 0x95C, PROTO_NORMAL_READ);//0x93C old
		m_ZOriginLocation = m_kReader->readType32(m_UWorld + 0x960, PROTO_NORMAL_READ);//0x940 old

		m_playerCameraManager = m_kReader->readType64(m_playerController + 0x04C8, PROTO_NORMAL_READ);//APlayerCameraManager //0x498 live server
		//m_playerCameraLocation = m_kReader->readTypeVec(m_playerCameraManager + 0x430, PROTO_NORMAL_READ);//within CameraCache //0x420 FCameraCacheEntry, 0x10 FMinimalViewInfo, 0x0 Location
		m_playerCameraRotation = m_kReader->readTypeVec(m_playerCameraManager + (0x1520), PROTO_NORMAL_READ);//within CameraCache //0x44C FCameraCacheEntry, 0x10 FMinimalViewInfo, 0xC Rotation
		
		//m_localPlayerPositionCamera = m_kReader->readTypeVec(m_playerCameraManager + 0x47C, PROTO_NORMAL_READ);//old 0x440


		//m_localPlayerCharacterMovement = m_kReader->readType64(m_localPawn + 0x418, PROTO_NORMAL_READ);
		//m_localPlayerStance = m_kReader->readType8(m_localPlayerCharacterMovement + 0x4D0, PROTO_NORMAL_READ);
		//std::cout << (int)m_localPlayerStance << std::endl;

		//m_localPlayerRotation = m_kReader->readTypeVec(m_playerController + 0x03E0, PROTO_NORMAL_READ);//FRotator

		//StanceMode at 0x4B8 1 byte



		//AWeapon processor can be found in ATslCharacter in pubg_tslgame_classes.hpp
		/*m_weaponProcessor = m_kReader->readType64(m_localPawn + 0xA68, PROTO_NORMAL_READ);//0x0xA48 old
		m_currentWeaponIndex = m_kReader->readType32(m_weaponProcessor + 0x4C8, PROTO_NORMAL_READ);//0x4C8 old
		m_equippedWeapons = m_kReader->readType64(m_weaponProcessor + 0x4B8, PROTO_NORMAL_READ);//0x4B8 old
		m_currentWeapon = m_kReader->readType64(m_equippedWeapons + (m_currentWeaponIndex * 8), PROTO_NORMAL_READ);//ATslWeapon
		m_currentWeaponRecoilInfoVert = m_kReader->readTypeVec(m_currentWeapon + 0xC58 + 0x40, PROTO_NORMAL_READ);//FRecoilInfo in ATslWeapon_Trajectory //0xB90 + 0x40 prev
		*/
		/*static int tempcounter = 0;
		if(tempcounter++ == 200)
		{
			std::cout << "*****" << std::endl;
			std::cout << m_currentWeaponRecoilInfoVert << std::endl;
			std::cout << m_kReader->readTypeVec(m_currentWeapon + 0xB90 + 0x34, PROTO_NORMAL_READ) << std::endl;
			std::cout << m_kReader->readTypeVec(m_currentWeapon + 0xB90 + 0x40, PROTO_NORMAL_READ) << std::endl;
			std::cout << m_kReader->readTypeVec(m_currentWeapon + 0xB90 + 0x4C, PROTO_NORMAL_READ) << std::endl;
			tempcounter = 0;
		}*/

		//FProjectileWeaponData



		//m_encryptionTable = m_kReader->readType64(m_kReader->getPUBase() + ETABLE, PROTO_NORMAL_READ);

	}

	/*
	* CLASS VARIABLES
	*/
	KReader* m_kReader;

	/*
	* Global IDs that are found from the game
	* These containers are used to help the
	* maintaining of systematic ID handling and
	* storing.
	*/
	std::deque<int32_t> allIDs;

	std::vector<int32_t> playerIDs;
	std::vector<int32_t> vehicleIDs;

	uint64_t tsl_decrypt_game_inst(uint64_t game_inst)
	{
		uint64_t a1 = READ64(game_inst);
		unsigned __int64 v1; // r10
		int v2; // er8
		__int64 v4; // [rsp+10h] [rbp+10h]

		v1 = a1 >> 32;
		v2 = ((a1 & 0xFFFF0000 | (unsigned __int16)a1 ^ ((unsigned int)a1 >> 16)) - 1360120078) >> 16;
		LODWORD(v4) = (v2 ^ (unsigned __int16)((a1 ^ WORD1(a1)) + 14066) | ((unsigned __int16)__ROR2__(v2, 8) << 16)) ^ 0xAEEE36F2;
		HIDWORD(v4) = ((((unsigned __int16)__ROR2__(WORD2(a1) ^ WORD1(v1), 8) | ((unsigned __int16)__ROR2__(WORD1(v1), 8) << 16))
			- 1380864718) & 0xFFFF0000 | (unsigned __int16)__ROR2__(
			(__ROR2__(WORD2(a1) ^ WORD1(v1), 8) - 21198) ^ ((((unsigned __int16)__ROR2__(WORD2(a1) ^ WORD1(v1), 8) | ((unsigned int)(unsigned __int16)__ROR2__(WORD1(v1), 8) << 16)) - 1380864718) >> 16),
				8)) ^ 0x524E52CE;
		return v4;
	}

	uint64_t tsl_decrypt_Local(uint64_t Local)
	{
		uint64_t a1 = READ64(Local);

		__int64 v1; // rcx
		return __PAIR__(
			(uint32)((HIDWORD(a1) ^ 0x2E4703E8) + 1675322332) ^ 0x55639FCCu,
			__ROR4__((__ROR4__(a1 ^ 0xC91840BD, 8) + 1692685404) ^ 0x641840BD, 8) ^ 0x64E4545Cu);
	}

	uint64_t tsl_decrypt_controller(uint64_t controller)
	{
		uint64_t a1 = READ64(controller);
		unsigned __int64 v1; // r10
		unsigned int v2; // er8
		unsigned int v3; // edx
		__int64 v5; // [rsp+10h] [rbp+10h]

		v1 = a1 >> 32;
		v2 = ((unsigned __int16)a1 ^ ((unsigned int)a1 >> 16) | ((unsigned __int16)__ROR2__(WORD1(a1), 8) << 16)) + 943216712;
		v3 = (HIDWORD(a1) & 0xFFFF0000 | (unsigned __int16)__ROL2__(WORD2(a1) ^ WORD1(v1), 8)) + 927414088;
		LODWORD(v5) = ((v2 >> 16) ^ (unsigned __int16)((a1 ^ WORD1(a1)) + 22600) | ((unsigned __int16)__ROR2__(HIWORD(v2), 8) << 16)) ^ 0x38385848;
		HIDWORD(v5) = (v3 & 0xFFFF0000 | (unsigned __int16)__ROL2__(v3 ^ HIWORD(v3), 8)) ^ 0xC8B8C8B8;
		return v5;
	}
	uint64_t tsl_decrypt_pawn(int64_t entity)
	{
		uint64_t a1 = READ64(entity);

		unsigned __int64 v1; // r8
		unsigned int v2; // edx
		__int64 v4; // [rsp+10h] [rbp+10h]

		v1 = a1 >> 32;
		v2 = (a1 & 0xFFFF0000 | (unsigned __int16)a1 ^ ((unsigned int)a1 >> 16)) + 404273256;
		IDA_LODWORD(v4) = (v2 & 0xFFFF0000 | (unsigned __int16)((a1 ^ WORD1(a1)) - 18328) ^ (v2 >> 16)) ^ 0x1818B868;
		IDA_LODWORD(v1) = (((unsigned __int16)__ROR2__(WORD1(v1), 8) << 16) | (unsigned __int16)__ROL2__(WORD2(a1) ^ WORD1(v1), 8))
			+ 392632168;
		IDA_HIDWORD(v4) = ((unsigned __int16)__ROL2__(WORD1(v1) ^ v1, 8) | ((unsigned __int16)__ROR2__(WORD1(v1), 8) << 16)) ^ 0xE898E898;
		return v4;
	}
	uint64_t tsl_decrypt_level(uint64_t level)
	{
		uint64_t a1 = READ64(level);
		int v1; // eax
		int v2; // eax
		unsigned int v3; // eax
		unsigned __int64 v4; // rcx
		int v5; // ecx
		int v6; // eax
		unsigned int v7; // eax
		__int64 v9; // [rsp+10h] [rbp+10h]

		if (a1 & 1)
			v1 = a1 ^ 0x31C87570 | 1;
		else
			v1 = (a1 ^ 0xDB4803BD) & 0xFFFFFFFE;
		v2 = v1 - 164209110;
		if (v2 & 1)
			v3 = v2 ^ 0xA2C87571 | 1;
		else
			v3 = (v2 ^ 0x764803BC) & 0xFFFFFFFE;
		v4 = a1 >> 32;
		LODWORD(v9) = v3 ^ 0xF6365E2A;
		if (v4 & 1)
			v5 = v4 ^ 0xC0272A89 | 1;
		else
			v5 = (v4 ^ 0x4077C6E8) & 0xFFFFFFFE;
		v6 = v5 - 169216662;
		if (((_BYTE)v5 + 106) & 1)
			v7 = (v6 ^ 0x31272A88 | 1) ^ 0xA160A96;
		else
			v7 = (v6 ^ 0xDB77C6E9) & 0xFFFFFFFE ^ 0xA160A96;
		HIDWORD(v9) = v7;
		return v9;
	}

	uint64_t tsl_decrypt_actor(uint64_t actor) {
		uint64_t a1 = READ64(actor);
		__int64 v4; // [rsp+30h] [rbp+8h]
		v4 = __ROL4__(__ROL4__(a1 - 1374092355, 16) + 1392691556, 16) ^ 0x9E9C5A7u;
		IDA_HIDWORD(v4) = __ROR4__(__ROR4__((a1 >> 32) + 323536616, 8) + 915361519, 8) ^ 0x77B977F9;

		return v4;
	}

	uint64_t tsl_decrypt_rootComponent(uint64_t prop)
	{
		uint64_t a1 = READ64(prop);
		__int64 v112; // [rsp+198h] [rbp+98h]
		LODWORD(v112) = __ROL4__(__ROL4__(a1 - 297270339, 16) - 764344028, 16) ^ 0x49290567;
		HIDWORD(v112) = __ROR4__(__ROR4__(HIDWORD(a1) + 1400358632, 8) - 1233219665, 8) ^ 0x37F93739;
		return v112;
	}
	int32_t tsl_decrypt_ID(int32_t id)
	{
		return   __ROR4__(id ^ 0x2B0248D0, 15) ^ (__ROR4__(id ^ 0x2B0248D0, 15) << 16) ^ 0x89927FF9;
	}
};