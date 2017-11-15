#include <Windows.h>
#include <iostream>
#include <thread>



#include "GameDataParser.hpp"
#include "LRadar.hpp"
#include "KReader.hpp"
#include "Window.hpp"

using namespace std;


GameDataParser* GDParser;
LRadar* Radar;


/*void test() {
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
			cin >> dec >> rpmRequest.size;

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

}*/
/*void testKReader()
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
}*/

void readerLoop(GameDataParser* w_reader, LRadar* radar)
{
	while (true)
	{
		if (readLoop)
		{
			w_reader->readLoop();
			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
}
// render function
void render()
{
	
	
	
	// clear the window alpha
	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	

	d3ddev->BeginScene();    // begins the 3D scene
	
							 
	Radar->render();


	d3ddev->EndScene();    // ends the 3D scene

	d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}

//set up overlay window
void SetupWindow()
{

		
	
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = GetModuleHandle(0);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wc.lpszClassName = "PUMyWindowBG";
	RegisterClassEx(&wc);
	DWORD dwStyle = (WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME);
	hWnd = CreateWindowEx(WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, wc.lpszClassName, "PUMyWindowBG", dwStyle, 0, 0, s_width, s_height, NULL, NULL, wc.hInstance, NULL);

	ShowWindow(hWnd, SW_SHOW);
	GetWindowRect(hWnd, &windowRect);
	initD3D(hWnd);
}


int main()
{
	//test();
	//testKReader();
	
	

	

	
		
	
	SetupWindow();

	// init a new GameDataParser instance
	
	GDParser = new GameDataParser;

	// init a new Radar instance
	
	Radar = new LRadar(GDParser);

	
	
	/*d::string readLine = "";
	while (readLine != "q")
	{
		std::cin >> readLine;
		if (readLine == "start" && readLoop == false && GDParser->getPUBase())
		{
			readLoop = true;
			std::cout << "Starting rloop" << std::endl;
		}

		if (readLine == "stop")
		{
			readLoop = false;
			std::cout << "Stopping rloop" << std::endl;
		}

		if (readLine == "base")
		{
			std::cout << "Base:" << GDParser->readPUBase() << std::endl;
		}

		if (readLine == "q")
		{
			break;
		}
		readLine = "";
	}*/

	std::thread t1(readerLoop, GDParser, Radar);
	
	MSG msg;

	while (TRUE)
	{
		ZeroMemory(&msg, sizeof(MSG));
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT)
		{
			//t1.join();
			exit(0);
		}
		//render shit here
		//GDParser->readLoop();
		render();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
	return msg.wParam;



	

}