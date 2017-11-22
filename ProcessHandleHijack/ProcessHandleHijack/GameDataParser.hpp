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
	int64_t m_localPlayerBasePointer;
	int32_t m_localTeam;
	int64_t m_AActorPtr;
	int32_t m_XOriginLocation;
	int32_t m_YOriginLocation;
	int32_t m_ZOriginLocation;
	int64_t m_playerController;
	int64_t m_playerCameraManager;
	Vector3 m_playerCameraRotation;



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
			int32_t curActorID = m_kReader->readType32(curActor + 0x0018, PROTO_NORMAL_READ);
			std::string actorGName = m_kReader->getGNameFromId(curActorID);

			// Here we check if the name is found from the wanted GNames list (PlayerMale etc...)
			if (std::find(playerIDs.begin(), playerIDs.end(), curActorID) != playerIDs.end())
			{
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x180, PROTO_NORMAL_READ);
				int64_t playerState = m_kReader->readType64(curActor + 0x3C0, PROTO_NORMAL_READ);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x1A0, PROTO_NORMAL_READ);

				int32_t actorTeam = m_kReader->readType32(playerState + 0x0444, PROTO_NORMAL_READ);

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;
				actorLocation.Z += m_ZOriginLocation;

				//w_data["players"].emplace_back(json::object({ { "t", actorTeam },{ "x", actorLocation.X },{ "y", actorLocation.Y }/*,{ "z", actorLocation.Z }*/ }));
				playersTemp->push_back(Player(actorTeam, actorLocation));
			}
			else if (actorGName == "DroppedItemGroup" || actorGName == "DroppedItemInteractionComponent")
			{
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x180, PROTO_NORMAL_READ);
				int64_t playerState = m_kReader->readType64(curActor + 0x3C0, PROTO_NORMAL_READ);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x1A0, PROTO_NORMAL_READ);
				int64_t DroppedItemArray = m_kReader->readType64(curActor + 0x2D8, PROTO_NORMAL_READ);
				int32_t DroppedItemCount = m_kReader->readType32(curActor + 0x2E0, PROTO_NORMAL_READ);

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
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x180, PROTO_NORMAL_READ);
				int64_t playerState = m_kReader->readType64(curActor + 0x3C0, PROTO_NORMAL_READ);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x1A0, PROTO_NORMAL_READ);

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;

				//w_data["vehicles"].emplace_back(json::object({ { "v", "Drop" },{ "x", actorLocation.X },{ "y", actorLocation.Y } }));
				vehiclesTemp->push_back(Vehicle("Drop", actorLocation));

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
			else if (std::find(vehicleIDs.begin(), vehicleIDs.end(), curActorID) != vehicleIDs.end())
			{
				// tästä alaspäin voi tehdä if-lohkoissa
				int64_t rootCmpPtr = m_kReader->readType64(curActor + 0x180, PROTO_NORMAL_READ);
				Vector3 actorLocation = m_kReader->readTypeVec(rootCmpPtr + 0x1A0, PROTO_NORMAL_READ);

				actorLocation.X += m_XOriginLocation;
				actorLocation.Y += m_YOriginLocation;

				std::string carName = m_kReader->getGNameFromId(curActorID);

				//w_data["vehicles"].emplace_back(json::object({ { "v", carName.substr(0, 3) },{ "x", actorLocation.X },{ "y", actorLocation.Y } }));
				vehiclesTemp->push_back(Vehicle(carName.substr(0, 3), actorLocation));
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
		m_UWorld = m_kReader->readType64(m_kReader->getPUBase() + UWORLD, PROTO_NORMAL_READ);//0x37E6988 for live
		m_gameInstance = m_kReader->readType64(m_UWorld + 0x140, PROTO_NORMAL_READ);
		m_ULocalPlayer = m_kReader->readType64(m_gameInstance + 0x38, PROTO_NORMAL_READ);
		m_localPlayer = m_kReader->readType64(m_ULocalPlayer, PROTO_NORMAL_READ);
		m_viewportclient = m_kReader->readType64(m_localPlayer + 0x58, PROTO_NORMAL_READ);
		m_playerController = m_kReader->readType64(m_localPlayer + 0x30, PROTO_NORMAL_READ);//added this
		m_localPawn = m_kReader->readType64(m_playerController + 0x3A8, PROTO_NORMAL_READ);//changed this from m_localPlayer
		m_localPlayerState = m_kReader->readType64(m_localPawn + 0x3C0, PROTO_NORMAL_READ);
		m_PWorld = m_kReader->readType64(m_viewportclient + 0x80, PROTO_NORMAL_READ);
		m_ULevel = m_kReader->readType64(m_PWorld + 0x30, PROTO_NORMAL_READ);
		m_playerCount = m_kReader->readType32(m_ULevel + 0xA8, PROTO_NORMAL_READ);//changed from 64 to 32

		m_localPlayerPosition = m_kReader->readTypeVec(m_localPlayer + 0x70, PROTO_NORMAL_READ);
		m_localPlayerBasePointer = m_kReader->readType64(m_localPlayer, PROTO_NORMAL_READ);

		m_localTeam = m_kReader->readType32(m_localPlayerState + 0x0444, PROTO_NORMAL_READ);

		m_AActorPtr = m_kReader->readType64(m_ULevel + 0xA0, PROTO_NORMAL_READ);

		m_XOriginLocation = m_kReader->readType32(m_PWorld + 0x918, PROTO_NORMAL_READ);
		m_YOriginLocation = m_kReader->readType32(m_PWorld + 0x91C, PROTO_NORMAL_READ);
		m_ZOriginLocation = m_kReader->readType32(m_PWorld + 0x920, PROTO_NORMAL_READ);

		m_playerCameraManager = m_kReader->readType64(m_playerController + 0x438, PROTO_NORMAL_READ);
		m_playerCameraRotation = m_kReader->readTypeVec(m_playerCameraManager + 0x42C, PROTO_NORMAL_READ);
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

