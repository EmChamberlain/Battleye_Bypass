#include <Windows.h>
#include <iostream>
#include <thread>

#include <math.h>



#include "GameDataParser.hpp"
#include "LRadar.hpp"
#include "KReader.hpp"
#include "Window.hpp"

using namespace std;


GameDataParser* GDParser;
LRadar* Radar;

bool aimBool = false;//this is set to true in main after gdparser is initialized
bool smoothBool = true;
float smooth = 1.25f;




//First started writing to angles on 2/14/2018

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

/*void readerLoop(GameDataParser* w_reader, LRadar* radar)
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
}*/
void inputLoop(LRadar* radar)
{
	while(true)
	{
		char letter;
		cin >> letter;
		if (letter == 'M')
			radar->miramarBool = true;
		else if (letter == 'E')
			radar->miramarBool = false;
		else if (letter == 'A')
		{
			aimBool = !aimBool;
			std::cout << aimBool << std::endl;
		}
		else if (letter == 'S')
		{
			float f;
			std::cin >> f;
			smooth = f;
		}
			
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	
}

Vector3 clamp(Vector3 in)
{
	Vector3 result = in;
	//pitch = X
	//yaw = Y
	//roll = Z
	if (result.X > 180)
		result.X -= 360;

	else if (result.X < -180)
		result.X += 360;

	if (result.Y > 180)
		result.Y -= 360;

	else if (result.Y < -180)
		result.Y += 360;

	if (result.X < -89)
		result.X = -89;

	if (result.X > 89)
		result.X = 89;

	while (result.Y < -180.0f)
		result.Y += 360.0f;

	while (result.Y > 180.0f)
		result.Y -= 360.0f;

	result.Z = 0;

	return result;
	
}

Vector3 toRotationVec(Vector3 in)
{
	Vector3 rot;
	float RADPI = (float)(180 / M_PI);
	rot.Y = (float)atan2(in.Y, in.X) * RADPI;
	rot.X = (float)atan2(in.Z, sqrt((in.X * in.X) + (in.Y * in.Y))) * RADPI;
	rot.Z = 0;

	return rot;
}

void aimLoop()
{
	while(true)
	{
		while(aimBool)
		{
			
			
			std::vector<Player> *players;

			if (GDParser->writingVectors)
				players = GDParser->playersOld;
			else
				players = GDParser->players;


			Vector3 localEyeLoc = GDParser->m_localPlayerPositionCamera;
			Vector3 localAng = GDParser->m_localPlayerRotation;
			
			//std::cout << localAng << std::endl;
				
			float fov = 5.0f;
			

			Vector3 bestDelta;
			bool foundTarget = false;

			for (Player p : *players)
			{
				Vector3 chest = p.loc;

				switch(p.stance)
				{
				case stand:
					chest.Z += 30;
					break;
				case crouch:
					chest.Z += 10;
					break;
				case prone:
					chest.Z -= 15;
					break;
				}


				Vector3 delta = chest - localEyeLoc;
				Vector3 angDelta = clamp(toRotationVec(delta) - localAng);
				//
				if (angDelta.length() <= fov)
				{
					fov = angDelta.length();
					bestDelta = delta;
					foundTarget = true;
				}

			}

			//found a target and middle mouse is pressed
			if (foundTarget && (GetKeyState(VK_MBUTTON) & 0x80) != 0)
			{
				
				Vector3 angDelta = clamp(toRotationVec(bestDelta) - localAng);
				if (smoothBool)
				{
					if (fov < 1.0)
						fov = 1.0;
					float smthAmount = fov * smooth;
					angDelta.X = angDelta.X / smthAmount;
					angDelta.Y = angDelta.Y / smthAmount;
					angDelta.Z = angDelta.Z / smthAmount;
				}
				Vector3 toAim = localAng + (angDelta);
				GDParser->toWriteAng = clamp(toAim);
				GDParser->needToWriteAng = true;
			}
			

			std::this_thread::sleep_for(std::chrono::milliseconds(5));
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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

	//std::thread t1(readerLoop, GDParser, Radar);
	std::thread inputThread(inputLoop, Radar);
	std::thread aimThread(aimLoop);

	
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
		GDParser->readLoop();
		render();
		aimBool = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
	return msg.wParam;



	

}