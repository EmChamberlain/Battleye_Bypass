#pragma once

#include "KReader.hpp"
//#include "pubgdec.hpp"


#include <deque>
#include <vector>




class GameDataParser
{
public:

	GameDataParser()
	{
		m_kReader = new KReader;
		readPUBase();
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
	int64_t m_PWorld;
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


	int64_t m_weaponProcessor ;
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
			int32_t curActorID = m_kReader->readType32(curActor + 0x0018, PROTO_NORMAL_READ);// 0x0018 live server
			if (curActorID == NULL)
				continue;
			std::string actorGName = m_kReader->getGNameFromId(curActorID, m_GNames);
			
			// Here we check if the name is found from the wanted GNames list (PlayerMale etc...)
			if (std::find(playerIDs.begin(), playerIDs.end(), curActorID) != playerIDs.end())
			{
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x0188, PROTO_NORMAL_READ);//USceneComponent //0x180 live server
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x290, PROTO_NORMAL_READ);//FVector    Location //0x1A0 live server
				int64_t playerState = m_kReader->readType64(curActor + 0x3D0, PROTO_NORMAL_READ);//0x3C0 live server
				int32_t actorTeam = m_kReader->readType32(playerState + 0x049C, PROTO_NORMAL_READ);//0x047C live server

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;
				actorLocation.Z += m_ZOriginLocation;

				int64_t movement = m_kReader->readType64(curActor + 0x418, PROTO_NORMAL_READ);
				StanceMode stance = StanceMode(m_kReader->readType8(movement + 0x4B8, PROTO_NORMAL_READ));

				//w_data["players"].emplace_back(json::object({ { "t", actorTeam },{ "x", actorLocation.X },{ "y", actorLocation.Y }/*,{ "z", actorLocation.Z } }));
				playersTemp->push_back(Player(actorTeam, actorLocation, stance));
			}
			else if (actorGName == "DroppedItemGroup" || actorGName == "DroppedItemInteractionComponent")
			{
				
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x188, PROTO_NORMAL_READ);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x290, PROTO_NORMAL_READ);
				int64_t DroppedItemArray = m_kReader->readType64(curActor + 0x2E8, PROTO_NORMAL_READ);//unsigned char    UnknownData14[0xA0] //0x2D8 live server
				int32_t DroppedItemCount = m_kReader->readType32(curActor + 0x2F0, PROTO_NORMAL_READ);//0x2E0 live server

				for (int j = 0; j < DroppedItemCount; j++)
				{
					int64_t ADroppedItem = m_kReader->readType64(DroppedItemArray + j * 0x10, PROTO_NORMAL_READ);
					Vector3 droppedLocation = m_kReader->readTypeVec(ADroppedItem + 0x2D0, PROTO_NORMAL_READ);
					droppedLocation.X = droppedLocation.X + actorLocation.X + m_XOriginLocation;
					droppedLocation.Y = droppedLocation.Y + actorLocation.Y + m_YOriginLocation;
					int64_t UItem = m_kReader->readType64(ADroppedItem + 0x538, PROTO_NORMAL_READ);
					int32_t UItemID = m_kReader->readType32(UItem + 0x18, PROTO_NORMAL_READ);
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
			else if (actorGName.substr(0, strlen("CarePackage")) == "CarePackage" || actorGName.substr(0, strlen("AircraftCarePackage")) == "AircraftCarePackage" || actorGName.substr(0, strlen("Carapackage_RedBox")) == "Carapackage_RedBox")
			{
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x0188, PROTO_NORMAL_READ);//USceneComponent //0x180 live server
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x290, PROTO_NORMAL_READ);//FVector    Location //0x1A0 live server

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;

				//w_data["vehicles"].emplace_back(json::object({ { "v", "Drop" },{ "x", actorLocation.X },{ "y", actorLocation.Y } }));
				vehiclesTemp->push_back(Vehicle("Drop", actorLocation));

			}
			else if (std::find(vehicleIDs.begin(), vehicleIDs.end(), curActorID) != vehicleIDs.end())
			{
				// tästä alaspäin voi tehdä if-lohkoissa
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x0188, PROTO_NORMAL_READ);//USceneComponent //0x180 live server
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x290, PROTO_NORMAL_READ);//FVector    Location //0x1A0 live server

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;

				//w_data["vehicles"].emplace_back(json::object({ { "v", carName.substr(0, 3) },{ "x", actorLocation.X },{ "y", actorLocation.Y } }));
				vehiclesTemp->push_back(Vehicle(actorGName.substr(0, 6), actorLocation));
			}
			else if(std::find(allIDs.begin(), allIDs.end(), curActorID) == allIDs.end())
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
			
			
			
			if(allIDs.size() > 5000)
			{
				allIDs.clear();
				std::cout << "Clearing aids" << std::endl;
			}
			if(playerIDs.size() > 10)
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

	void readLocals()
	{
		if (needToWriteAng)
		{
			writeAng(toWriteAng);
			needToWriteAng = false;
			toWriteAng = Vector3(89.0, 89.0, 0.0);
		}

		

		m_BaseUWorld = m_kReader->readType64(m_kReader->getPUBase() + UWORLD, PROTO_NORMAL_READ);
		m_UWorld = m_kReader->readType64(m_BaseUWorld, PROTO_NORMAL_READ);

		m_GNames = m_kReader->readType64(m_kReader->getPUBase() + GNAMES, PROTO_NORMAL_READ);
		m_GNames = m_kReader->readType64(m_GNames + 0xF8, PROTO_NORMAL_READ);
		m_GNames = m_GNames + +0x530;
		//std::cout << "UWorld: " << std::hex << m_UWorld << std::endl;
		//std::cout << "GNames: " << std::hex << m_GNames << std::endl;

		m_gameInstance = m_kReader->readType64(m_UWorld + 0x0140, PROTO_NORMAL_READ);//UGameInstance    OwningGameInstance    //0x140 test?
		m_ULocalPlayer = m_kReader->readType64(m_gameInstance + 0x38, PROTO_NORMAL_READ);//TArray<class ULocalPlayer*>
		//m_ULocalPlayer = pubgdec::decptr(m_kReader, m_gameInstance + 0x38);
		m_localPlayer = m_kReader->readType64(m_ULocalPlayer, PROTO_NORMAL_READ);//UPlayer
		m_viewportclient = m_kReader->readType64(m_localPlayer + 0x58, PROTO_NORMAL_READ);//UGameViewportClient
		m_playerController = m_kReader->readType64(m_localPlayer + 0x30, PROTO_NORMAL_READ);//APlayerController
		m_localPawn = m_kReader->readType64(m_playerController + 0x0428, PROTO_NORMAL_READ);//APawn //0x3A8 live server //0x3B8 test?
		m_localPlayerState = m_kReader->readType64(m_localPawn + 0x3D0, PROTO_NORMAL_READ);//APlayerState //0x3C0 live server //0x3D0 test?
		m_PWorld = m_kReader->readType64(m_viewportclient + 0x80, PROTO_NORMAL_READ);//UWorld
		m_ULevel = m_kReader->readType64(m_PWorld + 0x30, PROTO_NORMAL_READ);//ULevel

		m_localPlayerPosition = m_kReader->readTypeVec(m_localPlayer + 0x70, PROTO_NORMAL_READ);
		//m_localPlayerBasePointer = m_kReader->readType64(m_localPlayer, PROTO_NORMAL_READ);

		m_localTeam = m_kReader->readType32(m_localPlayerState + 0x049C, PROTO_NORMAL_READ);//0x049C seems like new one//old one 0x0484

		//int32_t myID = m_kReader->readType32(m_localPlayer + 0x0018, PROTO_NORMAL_READ);
		//std::string myGName = m_kReader->getGNameFromId(myID, m_GNames);


		int64_t decryptedAActorPtr = DecryptData(m_ULevel + 0xA0, m_kReader->getPUBase() + 0x3DAA540);
		m_AActorPtr = m_kReader->readType64(decryptedAActorPtr, PROTO_NORMAL_READ);//TArray<class AActor*>    AActors //0xA0 near actors //0xB0 all actors
		m_playerCount = m_kReader->readType32(decryptedAActorPtr + 0x08, PROTO_NORMAL_READ);//TArray<class AActor*> + 0x8 //0xA8 near actors //0xB8 all actors
		//m_AActorPtr = m_kReader->readType64(m_ULevel + 0xA0, PROTO_NORMAL_READ);//TArray<class AActor*>    AActors //0xA0 near actors //0xB0 all actors
		//m_playerCount = m_kReader->readType32(m_ULevel + 0xA8, PROTO_NORMAL_READ);//TArray<class AActor*> + 0x8 //0xA8 near actors //0xB8 all actors

		m_XOriginLocation = m_kReader->readType32(m_PWorld + 0x928, PROTO_NORMAL_READ);//these tend to be before class UWorldComposition*                           WorldComposition;
		m_YOriginLocation = m_kReader->readType32(m_PWorld + 0x92C, PROTO_NORMAL_READ);//
		m_ZOriginLocation = m_kReader->readType32(m_PWorld + 0x930, PROTO_NORMAL_READ);//
		
		m_playerCameraManager = m_kReader->readType64(m_playerController + 0x448, PROTO_NORMAL_READ);//APlayerCameraManager //0x438 live server
		//m_playerCameraLocation = m_kReader->readTypeVec(m_playerCameraManager + 0x430, PROTO_NORMAL_READ);//within CameraCache //0x420 FCameraCacheEntry, 0x10 FMinimalViewInfo, 0x0 Location
		m_playerCameraRotation = m_kReader->readTypeVec(m_playerCameraManager + 0x43C, PROTO_NORMAL_READ);//within CameraCache //0x420 FCameraCacheEntry, 0x10 FMinimalViewInfo, 0xC Rotation

		
		m_localPlayerPositionCamera = m_kReader->readTypeVec(m_playerCameraManager + 0x430, PROTO_NORMAL_READ);


		//m_localPlayerCharacterMovement = m_kReader->readType64(m_localPawn + 0x418, PROTO_NORMAL_READ);
		//m_localPlayerStance = m_kReader->readType8(m_localPlayerCharacterMovement + 0x4B8, PROTO_NORMAL_READ);

		m_localPlayerRotation = m_kReader->readTypeVec(m_playerController + 0x03E0, PROTO_NORMAL_READ);//FRotator

		//StanceMode at 0x4B8 1 byte

		

		//Weapon processor can be found in ATslCharacter in pubg_tslgame_classes.hpp
		m_weaponProcessor = m_kReader->readType64(m_localPawn + 0xA48, PROTO_NORMAL_READ);
		m_currentWeaponIndex = m_kReader->readType32(m_weaponProcessor + 0x4C8, PROTO_NORMAL_READ);
		m_equippedWeapons = m_kReader->readType64(m_weaponProcessor + 0x4B8, PROTO_NORMAL_READ);
		m_currentWeapon = m_kReader->readType64(m_equippedWeapons + (m_currentWeaponIndex * 8), PROTO_NORMAL_READ);//ATslWeapon
		m_currentWeaponRecoilInfoVert = m_kReader->readTypeVec(m_currentWeapon + 0xB90 + 0x48, PROTO_NORMAL_READ);//FRecoilInfo
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






	//external c++ uworld decrypt
	typedef DWORD_PTR(__stdcall *_qDeCrypt)(DWORD_PTR esi);
	PVOID codeMem = VirtualAlloc(0, 1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	_qDeCrypt qDecrypt3 = (_qDeCrypt)codeMem;

	template<typename T> T rol(T val, size_t count)
	{
		size_t bitcount = sizeof(T) * 8;
		count %= bitcount;
		return (val << count) | (val >> (bitcount - count));

	}
	DWORD_PTR DecryptData(DWORD_PTR cryptedOffset, DWORD_PTR decryptFuncBase)
	{

		DWORD_PTR _esi = m_kReader->readType64(cryptedOffset + 8, PROTO_NORMAL_READ);
		DWORD_PTR raxrbx = m_kReader->readType64(cryptedOffset, PROTO_NORMAL_READ);

		DWORD key1 = (DWORD(raxrbx) >> 8); //shr edx,08
		BYTE key2 = BYTE(raxrbx) ^ BYTE(key1); //xor dl,dil
		DWORD key3 = rol<BYTE>(key2, BYTE(raxrbx));  //rol dl,cl
		DWORD index = key3 & 0x1F; //and edx,1F


		DWORD_PTR targetFunc = m_kReader->readType64(decryptFuncBase + (index * 8), PROTO_NORMAL_READ);


		/*if (!ReadVirtualMemory(HLProcID, (ULONGLONG)(targetFunc), (ULONGLONG)codeMem, 60)) {
			return 0;
		}*/
		RMOResponseRPMBytes *response = m_kReader->readSize(targetFunc, 60, PROTO_NORMAL_READ);
		memcpy((PVOID)codeMem, (PVOID)(response->val), 60);
		
		delete response;
		if (!codeMem)
			return 0;
			


		if (*(WORD*)(codeMem) != 0xC148) //ror ecx,xx
			return 0;

		return qDecrypt3(_esi);
	}







};

