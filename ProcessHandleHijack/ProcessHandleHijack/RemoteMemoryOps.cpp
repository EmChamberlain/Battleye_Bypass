#include "RemoteMemoryOps.hpp"


bool readLoop = true;

// ugly to define the wanted items / player gnames this way.
std::vector<std::string> playerGNameVec = { "PlayerMale", "PlayerFemale" };
std::vector<std::string> vehicleGNameVec = { "Uaz", "Buggy", "Dacia", "ABP_Motorbike", "BP_Motorbike", "Boat_PG117", "PG117", "AquaRail", "BP_PickupTruck", "BP_Van", "Sedan"};
//std::map<std::string, std::string> dropGNameMap = { { "Item_Head_G_01_Lv3_C", "Helm3" },{ "Item_Head_G_01_Lv3_", "Helm3" },{ "Item_Armor_C_01_Lv3", "Vest3" },{ "Item_Armor_C_01_Lv3_C", "Vest3" },{ "Item_Equip_Armor_Lv3_C", "Vest3" },{ "Item_Equip_Armor_Lv3", "Vest3" },{ "Item_Attach_Weapon_Muzzle_Suppressor_SniperRifle", "Supp(SR)" },{ "Item_Attach_Weapon_Muzzle_Suppressor_Large", "Supp(AR)" },{ "Item_Attach_Weapon_Muzzle_Suppressor_Large_C", "Supp(SR)" },{ "Item_Heal_MedKit", "Meds" },{ "Item_Heal_FirstAid", "Meds" },{ "Item_Weapon_Kar98k", "kar98" },{ "Item_Weapon_Mini14", "mini" },{ "Item_Weapon_M16A4", "M16" },{ "Item_Weapon_HK416", "m416" },{ "Item_Weapon_SCAR-L", "SCAR" },{ "Item_Weapon_SKS", "sks" },{ "Item_Attach_Weapon_Upper_ACOG_01", "4x" },{ "Item_Attach_Weapon_Upper_CQBSS", "8x" },{ "Item_Attach_Weapon_Upper_CQBSS_C", "8x" } };
//previous line is full drop names
std::map<std::string, std::string> dropGNameMap = { { "Item_Head_G_01_Lv3_C", "Helm3" },{ "Item_Head_G_01_Lv3_", "Helm3" },{ "Item_Armor_C_01_Lv3", "Vest3" },{ "Item_Armor_C_01_Lv3_C", "Vest3" },{ "Item_Equip_Armor_Lv3_C", "Vest3" },{ "Item_Equip_Armor_Lv3", "Vest3" },{ "Item_Attach_Weapon_Muzzle_Suppressor_SniperRifle", "Supp(SR)" },{ "Item_Attach_Weapon_Muzzle_Suppressor_Large", "Supp(AR)" },{ "Item_Attach_Weapon_Muzzle_Suppressor_Large_C", "Supp(SR)" },{ "Item_Heal_MedKit", "Meds" },{ "Item_Weapon_Kar98k", "kar98" },{ "Item_Attach_Weapon_Upper_ACOG_01", "4x" },{ "Item_Attach_Weapon_Upper_CQBSS", "8x" },{ "Item_Attach_Weapon_Upper_CQBSS_C", "8x" },{ "Item_Weapon_HK416", "m416" },{ "Item_Weapon_SCAR-L", "SCAR" },{ "Item_Weapon_SKS", "sks" } };


using namespace std;

int HandleGatewayClient::ConnectPipe() {
	while (1) { // Try to open a named pipe; wait for it, if necessary. 
		m_pipeHandle = CreateFile(m_lpszPipename, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

		if (m_pipeHandle != INVALID_HANDLE_VALUE)
			break; // Break if the pipe handle is valid. 

		if (GetLastError() != ERROR_PIPE_BUSY) { // Exit if an error other than ERROR_PIPE_BUSY occurs. 
			//cout << "Could not open pipe. GetLastError: " << dec << GetLastError() << endl;
			return -1;
		}

		// All pipe instances are busy, so wait. 
		if (!WaitNamedPipe(m_lpszPipename, PIPEWAITTIMOUTIFBUSY)) {
			//cout << "Could not open pipe: 20 second wait timed out." << endl;
			return -1;
		}
	}

	cout << "P conn." << endl;

	HandleGatewayClient::SetPipeMode(PIPE_READMODE_BYTE);

	return 0;
}

int HandleGatewayClient::SetPipeMode(DWORD mode) {
	// Should be PIPE_READMODE_BYTE or PIPE_READMODE_MESSAGE
	// See SetNamedPipeHandleState on MSDN for more info: https://msdn.microsoft.com/en-us/library/windows/desktop/aa365787(v=vs.85).aspx
	BOOL fSuccess = FALSE;
	fSuccess = SetNamedPipeHandleState(m_pipeHandle, &mode, NULL, NULL);
	if (!fSuccess) {
		//cout << "SetNamedPipeHandleState failed. GetLastError: " << GetLastError() << endl;
		return -1;
	}

	cout << "P mode set." << endl;
	return 0;
}

bool HandleGatewayClient::RequestReadProcessMemory(RMORequestRPM rpmRequest) {
	//cout << "Sending RPM request (" << sizeof(rpmRequest) << " bytes) message." << endl;

	BOOL fSuccess = FALSE;
	DWORD bytesWritten = 0;
	fSuccess = WriteFile(m_pipeHandle, &rpmRequest, sizeof(rpmRequest), &bytesWritten, NULL);
	//cout << "Address: " << hex << rpmRequest.address << " Size: " << dec << rpmRequest.size << endl;
	if (!fSuccess) {
		cout << "WFile failed, Error: " << dec << GetLastError() << endl;
		return false;
	}

	//cout << "Request sent (" << bytesWritten << " bytes written)." << endl;

	return true;
}

RMOResponseRPM128 HandleGatewayClient::ReceiveReadProcessMemory128() {
	RMOResponseRPM128 response;
	BOOL fSuccess = FALSE;
	DWORD bytesRead = 0;

	do { // Read from the pipe.
		fSuccess = ReadFile(m_pipeHandle, &response, sizeof(RMOResponseRPM128), &bytesRead, NULL);

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
		cout << "RFile failed, Error: " << dec << GetLastError() << endl;

	return response;
}

RMOResponseRPM128 HandleGatewayClient::RemoteReadProcessMemory128(RMORequestRPM rpmRequest) {
	RMOResponseRPM128 response;
	rpmRequest.order = 6;
	if (HandleGatewayClient::RequestReadProcessMemory(rpmRequest)) {
		response = HandleGatewayClient::ReceiveReadProcessMemory128();
	}

	return response;
}

RMOResponseRPM64 HandleGatewayClient::ReceiveReadProcessMemory64() {
	RMOResponseRPM64 response;
	BOOL fSuccess = FALSE;
	DWORD bytesRead = 0;

	do { // Read from the pipe.
		fSuccess = ReadFile(m_pipeHandle, &response, sizeof(RMOResponseRPM64), &bytesRead, NULL);

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
		cout << "RFile failed, Error: " << dec << GetLastError() << endl;

	return response;
}

RMOResponseRPM64 HandleGatewayClient::RemoteReadProcessMemory64(RMORequestRPM rpmRequest) {
	RMOResponseRPM64 response;
	rpmRequest.order = 0;
	if (HandleGatewayClient::RequestReadProcessMemory(rpmRequest)) {
		response = HandleGatewayClient::ReceiveReadProcessMemory64();
	}

	return response;
}

RMOResponseRPM32 HandleGatewayClient::ReceiveReadProcessMemory32() {
	RMOResponseRPM32 response;
	BOOL fSuccess = FALSE;
	DWORD bytesRead = 0;

	do { // Read from the pipe.
		fSuccess = ReadFile(m_pipeHandle, &response, sizeof(RMOResponseRPM32), &bytesRead, NULL);

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
		cout << "RFile failed, Error: " << dec << GetLastError() << endl;

	return response;
}

RMOResponseRPM32 HandleGatewayClient::RemoteReadProcessMemory32(RMORequestRPM rpmRequest) {
	RMOResponseRPM32 response;
	rpmRequest.order = 1;
	if (HandleGatewayClient::RequestReadProcessMemory(rpmRequest)) {
		response = HandleGatewayClient::ReceiveReadProcessMemory32();
	}

	return response;
}

RMOResponseRPM8 HandleGatewayClient::ReceiveReadProcessMemory8() {
	RMOResponseRPM8 response;
	BOOL fSuccess = FALSE;
	DWORD bytesRead = 0;

	do { // Read from the pipe.
		fSuccess = ReadFile(m_pipeHandle, &response, sizeof(RMOResponseRPM8), &bytesRead, NULL);

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
		cout << "RFile failed, Error: " << dec << GetLastError() << endl;

	return response;
}

RMOResponseRPM8 HandleGatewayClient::RemoteReadProcessMemory8(RMORequestRPM rpmRequest) {
	RMOResponseRPM8 response;
	rpmRequest.order = 5;
	if (HandleGatewayClient::RequestReadProcessMemory(rpmRequest)) {
		response = HandleGatewayClient::ReceiveReadProcessMemory8();
	}

	return response;
}

RMOResponseRPMVec HandleGatewayClient::ReceiveReadProcessMemoryVec() {
	RMOResponseRPMVec response;
	BOOL fSuccess = FALSE;
	DWORD bytesRead = 0;

	do { // Read from the pipe.
		fSuccess = ReadFile(m_pipeHandle, &response, sizeof(RMOResponseRPMVec), &bytesRead, NULL);

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
		cout << "RFile failed, Error: " << dec << GetLastError() << endl;

	return response;
}

RMOResponseRPMVec HandleGatewayClient::RemoteReadProcessMemoryVec(RMORequestRPM rpmRequest) {
	RMOResponseRPMVec response;
	rpmRequest.order = 2;
	if (HandleGatewayClient::RequestReadProcessMemory(rpmRequest)) {
		response = HandleGatewayClient::ReceiveReadProcessMemoryVec();
	}

	return response;
}

RMOResponseRPMBytes HandleGatewayClient::ReceiveReadProcessMemoryBytes() {
	RMOResponseRPMBytes response;
	BOOL fSuccess = FALSE;
	DWORD bytesRead = 0;

	do { // Read from the pipe.
		fSuccess = ReadFile(m_pipeHandle, &response, sizeof(RMOResponseRPMBytes), &bytesRead, NULL);

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
		cout << "RFile failed, Error: " << dec << GetLastError() << endl;

	return response;
}

RMOResponseRPMBytes HandleGatewayClient::RemoteReadProcessMemoryBytes(RMORequestRPM rpmRequest) {
	RMOResponseRPMBytes response;
	rpmRequest.order = 3;
	if (HandleGatewayClient::RequestReadProcessMemory(rpmRequest)) {
		response = HandleGatewayClient::ReceiveReadProcessMemoryBytes();
	}

	return response;
}
RMOResponseRPMActor HandleGatewayClient::ReceiveReadProcessMemoryActor() {
	RMOResponseRPMActor response;
	BOOL fSuccess = FALSE;
	DWORD bytesRead = 0;

	do { // Read from the pipe.
		fSuccess = ReadFile(m_pipeHandle, &response, sizeof(RMOResponseRPMActor), &bytesRead, NULL);

		if (!fSuccess && GetLastError() != ERROR_MORE_DATA)
			break;

	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess)
		cout << "RFile failed, Error: " << dec << GetLastError() << endl;

	return response;
}

RMOResponseRPMActor HandleGatewayClient::RemoteReadProcessMemoryActor(RMORequestRPM rpmRequest) {
	RMOResponseRPMActor response;
	rpmRequest.order = 4;
	if (HandleGatewayClient::RequestReadProcessMemory(rpmRequest)) {
		response = HandleGatewayClient::ReceiveReadProcessMemoryActor();
	}

	return response;
}


BOOL HandleGatewayClient::DisconnectPipe() {
	return CloseHandle(m_pipeHandle);
}