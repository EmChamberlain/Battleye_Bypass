#pragma once

#include "KReader.hpp"
#include <deque>
#include <vector>




class GameDataParser
{
public:

	GameDataParser()
	{
		m_kReader = new KReader;
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
	int64_t m_UWorld;
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
	Vector3 m_playerCameraRotation;
	Vector3 m_localPlayerPositionCamera;
	int64_t m_encryptionTable;



private:

	/*
	* PRIVATE CLASS FUNCTIONS
	*/
	int64_t shittyDecrypt(int i)
	{
		
	}
	void readPlayers()
	{
		std::vector<Player> *playersTemp = new std::vector<Player>();
		std::vector<Vehicle> *vehiclesTemp = new std::vector<Vehicle>();
		std::vector<Item> *itemsTemp = new std::vector<Item>();
		for (int i = 0; i < m_playerCount; i++)
		{

			/*EncryptedActor encrActor = m_kReader->readTypeActor(m_AActorPtr + (i * sizeof(EncryptedActor)), PROTO_NORMAL_READ);
			
			DWORD decoded_xor = encrActor.xor ^ 0xCBAC;
			DWORD decoded_Index = encrActor.index ^ 0xD7AF5ABC;

			auto Xor1 = m_kReader->readType32(m_encryptionTable + 4*((byte)(decoded_Index)+0x300), PROTO_NORMAL_READ);
			auto Xor2 = m_kReader->readType32(m_encryptionTable + 4*((byte)((DWORD_PTR)decoded_Index >> 0x8) + 0x200), PROTO_NORMAL_READ);
			auto Xor3 = m_kReader->readType32(m_encryptionTable + 4*((byte)((DWORD_PTR)decoded_Index >> 0x10) + 0x100), PROTO_NORMAL_READ);
			auto Xor4 = m_kReader->readType32(m_encryptionTable + 4*((DWORD_PTR)(decoded_Index >> 0x18)), PROTO_NORMAL_READ);
			auto Real_Index = (Xor1^Xor2^Xor3^~Xor4) % 0x2B;
			if (decoded_xor == 0 || decoded_Index == 0 || Xor1 == 0 || Xor2 == 0 || Xor3 == 0 || Xor4 == 0 || Real_Index == 0)
			{
				std::cout << decoded_xor << ":" << decoded_Index << ":" << Xor1 << ":" << Xor2 << ":" << Xor3 << ":" << Xor4 << ":" << Real_Index << std::endl;
				char trash;
				std::cin >> trash;
			}
			int64_t curActor = encrActor.ptr_table[Real_Index] ^ decoded_xor;
			*/


			//this is complete shit, need to find a way to do it with the encryptionTable
			/*int64_t curActor;
			int32_t curActorID;
			std::string actorGName;

			EncryptedActor encrActor = m_kReader->readTypeActor(m_AActorPtr + (i * sizeof(EncryptedActor)), PROTO_NORMAL_READ);
			uint16_t decoded_xor = encrActor.xor ^ 0xCBAC;
			
			for (int j = 0; j < 0x2B; j++)
			{
				if (0xFFFFF < encrActor.ptr_table[j] < 0xFFFFFFFFFFF)
				{
					curActor = encrActor.ptr_table[j] ^ (int64_t)(decoded_xor);
					curActorID = m_kReader->readType32(curActor + 0x0018, PROTO_NORMAL_READ);
					actorGName = m_kReader->getGNameFromId(curActorID);
					if (actorGName != "None" && actorGName != "FAIL")
					{
						std::cout << actorGName << std::endl;
						for (std::vector<std::string>::iterator it = playerGNameVec.begin(); it != playerGNameVec.end(); ++it)
						{
							//check if the name is same, and add it to the playerIDs vector
							if (*it == actorGName.substr(0, (*it).length()))
							{
								std::cout << actorGName << std::endl;
								char trash;
								std::cin >> trash;
								break;
							}
						}
						
					}
						
				}
			}*/

			// read the position of Player
			int64_t curActor = m_kReader->readType64(m_AActorPtr + (i * 0x8), PROTO_NORMAL_READ);
			if (curActor == NULL)
				continue;
			int32_t curActorID = m_kReader->readType32(curActor + 0x0018, PROTO_NORMAL_READ);// 0x0018 live server
			if (curActorID == NULL)
				continue;
			std::string actorGName = m_kReader->getGNameFromId(curActorID);
			
			// Here we check if the name is found from the wanted GNames list (PlayerMale etc...)
			if (std::find(playerIDs.begin(), playerIDs.end(), curActorID) != playerIDs.end())
			{
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x0188, PROTO_NORMAL_READ);//USceneComponent //0x180 live server
				
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x290, PROTO_NORMAL_READ);//FVector    Location //0x1A0 live server
				int64_t playerState = m_kReader->readType64(curActor + 0x3D0, PROTO_NORMAL_READ);//0x3C0 live server
				int32_t actorTeam = m_kReader->readType32(playerState + 0x0484, PROTO_NORMAL_READ);//0x047C live server

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;
				actorLocation.Z += m_ZOriginLocation;

				//w_data["players"].emplace_back(json::object({ { "t", actorTeam },{ "x", actorLocation.X },{ "y", actorLocation.Y }/*,{ "z", actorLocation.Z } }));
				playersTemp->push_back(Player(actorTeam, actorLocation));
			}
			/*else if (actorGName == "DroppedItemGroup" || actorGName == "DroppedItemInteractionComponent")
			{
				
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x188, PROTO_NORMAL_READ);
				int64_t playerState = m_kReader->readType64(curActor + 0x3D0, PROTO_NORMAL_READ);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x280, PROTO_NORMAL_READ);
				int64_t DroppedItemArray = m_kReader->readType64(curActor + 0x2E0, PROTO_NORMAL_READ);//unsigned char    UnknownData14[0xA0] //0x2D8 live server
				int32_t DroppedItemCount = m_kReader->readType32(curActor + 0x2E8, PROTO_NORMAL_READ);//0x2E0 live server

				for (int j = 0; j < DroppedItemCount; j++)
				{
					int64_t ADroppedItem = m_kReader->readType64(DroppedItemArray + j * 0x10, PROTO_NORMAL_READ);
					Vector3 droppedLocation = m_kReader->readTypeVec(ADroppedItem + 0x1E0, PROTO_NORMAL_READ);
					droppedLocation.X = droppedLocation.X + actorLocation.X + m_XOriginLocation;
					droppedLocation.Y = droppedLocation.Y + actorLocation.Y + m_YOriginLocation;
					int64_t UItem = m_kReader->readType64(ADroppedItem + 0x448, PROTO_NORMAL_READ);
					int32_t UItemID = m_kReader->readType32(UItem + 0x18, PROTO_NORMAL_READ);
					std::string itemName = m_kReader->getGNameFromId(UItemID);

					// check if inside the map / array of wanted items
					for (std::map<std::string, std::string>::iterator it = dropGNameMap.begin(); it != dropGNameMap.end(); ++it)
					{
						if (itemName.substr(0, it->first.length()) == it->first)
						{
							actorLocation.X += m_XOriginLocation;
							actorLocation.Y += m_YOriginLocation;

							//w_data["items"].emplace_back(json::object({ { "n", it->second },{ "x", droppedLocation.X },{ "y", droppedLocation.Y } }));
							itemsTemp->push_back(Item(it->second, droppedLocation));
						}
					}
				}
			}
			else if (actorGName.substr(0, strlen("CarePackage")) == "CarePackage" || actorGName.substr(0, strlen("AircraftCarePackage")) == "AircraftCarePackage" || actorGName.substr(0, strlen("Carapackage_RedBox")) == "Carapackage_RedBox")
			{
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x188, PROTO_NORMAL_READ);
				int64_t playerState = m_kReader->readType64(curActor + 0x3D0, PROTO_NORMAL_READ);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x280, PROTO_NORMAL_READ);

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;

				//w_data["vehicles"].emplace_back(json::object({ { "v", "Drop" },{ "x", actorLocation.X },{ "y", actorLocation.Y } }));
				vehiclesTemp->push_back(Vehicle("Drop", actorLocation));

			}
			else if (std::find(vehicleIDs.begin(), vehicleIDs.end(), curActorID) != vehicleIDs.end())
			{
				// tästä alaspäin voi tehdä if-lohkoissa
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x188, PROTO_NORMAL_READ);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x280, PROTO_NORMAL_READ);

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;

				std::string carName = m_kReader->getGNameFromId(curActorID);

				//w_data["vehicles"].emplace_back(json::object({ { "v", carName.substr(0, 3) },{ "x", actorLocation.X },{ "y", actorLocation.Y } }));
				vehiclesTemp->push_back(Vehicle(carName.substr(0, 3), actorLocation));
			}
			else if(std::find(allIDs.begin(), allIDs.end(), curActorID) == allIDs.end())
			{
				allIDs.push_back(curActorID);
				std::string actorGName = m_kReader->getGNameFromId(curActorID);
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
			}*/
			else
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
			}
			
			
			
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
		m_UWorld = m_kReader->readType64(m_kReader->getPUBase() + UWORLD, PROTO_NORMAL_READ);
		m_gameInstance = m_kReader->readType64(m_UWorld + 0x0140, PROTO_NORMAL_READ);//UGameInstance    OwningGameInstance    //0x140 test?
		m_ULocalPlayer = m_kReader->readType64(m_gameInstance + 0x38, PROTO_NORMAL_READ);//TArray<class ULocalPlayer*>
		m_localPlayer = m_kReader->readType64(m_ULocalPlayer, PROTO_NORMAL_READ);//UPlayer
		m_viewportclient = m_kReader->readType64(m_localPlayer + 0x58, PROTO_NORMAL_READ);//UGameViewportClient
		m_playerController = m_kReader->readType64(m_localPlayer + 0x30, PROTO_NORMAL_READ);//APlayerController
		m_localPawn = m_kReader->readType64(m_playerController + 0x0428, PROTO_NORMAL_READ);//APawn //0x3A8 live server //0x3B8 test?
		m_localPlayerState = m_kReader->readType64(m_localPawn + 0x3D0, PROTO_NORMAL_READ);//APlayerState //0x3C0 live server //0x3D0 test?
		m_PWorld = m_kReader->readType64(m_viewportclient + 0x80, PROTO_NORMAL_READ);//UWorld
		m_ULevel = m_kReader->readType64(m_PWorld + 0x30, PROTO_NORMAL_READ);//ULevel
		m_playerCount = m_kReader->readType32(m_ULevel + 0xB8, PROTO_NORMAL_READ);//TArray<class AActor*> + 0x8 //0xA8 live server //0xB8 test?    changed from 64 to 32

		m_localPlayerPosition = m_kReader->readTypeVec(m_localPlayer + 0x70, PROTO_NORMAL_READ);
		//m_localPlayerBasePointer = m_kReader->readType64(m_localPlayer, PROTO_NORMAL_READ);

		m_localTeam = m_kReader->readType32(m_localPlayerState + 0x0484, PROTO_NORMAL_READ);//0x047C live server //0x047C test?

		m_AActorPtr = m_kReader->readType64(m_ULevel + 0xB0, PROTO_NORMAL_READ);//TArray<class AActor*>    AActors //0xA0 live server //0xB0 test?

		m_XOriginLocation = m_kReader->readType32(m_PWorld + 0x928, PROTO_NORMAL_READ);//0x918 live server
		m_YOriginLocation = m_kReader->readType32(m_PWorld + 0x92C, PROTO_NORMAL_READ);//0x91C live server
		m_ZOriginLocation = m_kReader->readType32(m_PWorld + 0x930, PROTO_NORMAL_READ);//0x920 live server
		
		m_playerCameraManager = m_kReader->readType64(m_playerController + 0x448, PROTO_NORMAL_READ);//APlayerCameraManager //0x438 live server
		m_playerCameraRotation = m_kReader->readTypeVec(m_playerCameraManager + 0x43C, PROTO_NORMAL_READ);//within CameraCache //0x420 FCameraCacheEntry, 0x10 FMinimalViewInfo
		
		m_localPlayerPositionCamera = m_kReader->readTypeVec(m_playerCameraManager + 0x430, PROTO_NORMAL_READ);

		m_encryptionTable = m_kReader->readType64(m_kReader->getPUBase() + ETABLE, PROTO_NORMAL_READ);

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
};

