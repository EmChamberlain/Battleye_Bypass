#include <Windows.h>
#include <iostream>
#include <thread>

#include "GameDataParser.hpp"
#include "LRadar.hpp"
#include "KReader.hpp"

using namespace std;


void test() {
	HandleGatewayClient gatewayClient;
	gatewayClient.ConnectPipe();

	int orderUser = 0;
	do {
		cout << endl;
		cout << "[###] Choose the order to send:" << endl;
		cout << "[ 0 ] Ping (calculate time for a full request) TODO" << endl;
		cout << "[ 1 ] ReadProcessMemory" << endl;
		cout << "[ 2 ] WriteProcessMemory TODO" << endl;
		cout << "[ 9 ] Quit" << endl;
		cout << endl;
		cout << "Your order: ";
		cin >> dec >> orderUser;
		cout << endl;

		if (orderUser == 1) {
			//RMORequestRPM rpmRequest = { RMO_ORDER_READPROCESSMEMORY, 216, 0xBEFC74, 10 };
			RMOResponseRPMVec rpmResponse;
			RMORequestRPM rpmRequest;
			rpmRequest.order = RMO_ORDER_READPROCESSMEMORY;
			rpmRequest.address = 0x00007FF96B141010;
			rpmRequest.size = sizeof(Vector3);
			/*cout << "Memory address to read (in hexadecimal): ";
			cin >> hex >> rpmRequest.address;
			cout << "Size to read (bytes, in decimal): ";
			cin >> dec >> rpmRequest.size;*/

			rpmResponse = gatewayClient.RemoteReadProcessMemoryVec(rpmRequest);

			cout << endl;
			cout << "Status: " << rpmResponse.status << endl;
			cout << "Bytes Read: " << rpmResponse.bytesRead << endl;
			cout << endl;

			cout << "Raw data (as chars): " << endl;
			for (int i(0); i < rpmRequest.size; ++i) {
				cout << ((char *)(&rpmResponse.val))[i];
			}
			cout << endl;

			//cout << "Hex data: " << hex << rpmResponse.val << endl;
			
			cout << endl;

			//cout << "Val: " << dec << rpmResponse.val << endl;

			cout << "X: " << rpmResponse.val.X << " Y: " << rpmResponse.val.Y << " Z: " << rpmResponse.val.Z << endl;
			cout << endl << endl;

			system("pause");
		}

		system("cls");
	} while (orderUser != 9);

	gatewayClient.DisconnectPipe();

	system("pause");

}
void testKReader()
{
	KReader *reader = new KReader;
	cout << sizeof(int64_t) << endl;
	do {
		
		int64_t address = 0x00007FF96B141100;


		int64_t integer64 = reader->readType64(address, PROTO_NORMAL_READ);
		cout << "int64_t: " << integer64 << endl << endl;

		int32_t integer32 = reader->readType32(address, PROTO_NORMAL_READ);
		cout << "int32_t: " << integer32 << endl << endl;

		Vector3 vec = reader->readTypeVec(address, PROTO_NORMAL_READ);
		cout << "X: " << vec.X << " Y: " << vec.Y << " Z: " << vec.Z << endl;

		char* name;
		RMOResponseRPMBytes *response = reader->readSize(address, 64, PROTO_NORMAL_READ);
		if ((name = (char*)(response->val)) != NULL)
		cout << name << endl;
		std::system("pause");
		delete name;
		delete response;
		std::system("cls");
	} while (true);

	delete reader;

	std::system("pause");
}

void readerLoop(GameDataParser* w_reader, LRadar* radar)
{
	while (1)
	{
		if (readLoop)
		{
			w_reader->readLoop();
			radar->render();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
}


int main()
{
	//test();
	testKReader();
	/*
	// init a new GameDataParser instance
	GameDataParser* GDParser;
	GDParser = new GameDataParser;

	LRadar* Radar;
	Radar = new LRadar(GDParser);


	std::thread t1(readerLoop, GDParser, Radar);

	std::string readLine = "";

	// l33b user interface
	while (readLine != "quit")
	{
		std::cin >> readLine;
		if (readLine == "start" && readLoop == false && GDParser->getPUBase())
		{
			readLoop = true;
			std::cout << "Starting read loop" << std::endl;
		}

		if (readLine == "stop")
		{
			readLoop = false;
			std::cout << "Stopping read loop" << std::endl;
		}

		if (readLine == "base")
		{
			std::cout << "Base set to: " << GDParser->readPUBase() << std::endl;
		}

		if (readLine == "exit" || readLine == "quit")
		{
			break;
		}
		readLine = "";
	}
	*/

	return 0;
}