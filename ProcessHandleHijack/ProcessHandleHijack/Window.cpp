#include "Window.hpp"
#include <string>
#include <cstdarg>
#include <vector>


//defining data
int s_width = 1000;
int s_height = 1000;
LPDIRECT3D9 d3d;
LPDIRECT3DDEVICE9 d3ddev;
HWND hWnd;
LPD3DXFONT pFont;
ID3DXLine* d3dLine;
LPDIRECT3DTEXTURE9 mapTextureGrey;
LPDIRECT3DTEXTURE9 mapTextureMiramar;
LPDIRECT3DTEXTURE9 mapTextureErangel;
LPDIRECT3DTEXTURE9 mapTextureSavage;
D3DSURFACE_DESC surfaceGrey;
D3DSURFACE_DESC surfaceMiramar;
D3DSURFACE_DESC surfaceErangel;
D3DSURFACE_DESC surfaceSavage;
LPD3DXSPRITE mapSprite;
RECT windowRect;




//ESP stuff
void initD3D(HWND hWnd)
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information

	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hWnd;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;     // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = s_width;    // set the width of the buffer
	d3dpp.BackBufferHeight = s_height;    // set the height of the buffer

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);

	D3DXCreateFontA(d3ddev, 13, 0, FW_HEAVY, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);
	D3DXCreateLine(d3ddev, &d3dLine);

	D3DXCreateTextureFromFile(d3ddev, "D:/pic_mir.png", &mapTextureMiramar);//png for miramar
	mapTextureMiramar->GetLevelDesc(NULL, &surfaceMiramar);


	D3DXCreateTextureFromFile(d3ddev, "D:/pic_er.jpg", &mapTextureErangel);//png for erangel
	mapTextureErangel->GetLevelDesc(NULL, &surfaceErangel);
	D3DXCreateSprite(d3ddev, &mapSprite);

	D3DXCreateTextureFromFile(d3ddev, "D:/pic_sa.png", &mapTextureSavage);//png for savage
	mapTextureSavage->GetLevelDesc(NULL, &surfaceSavage);
	D3DXCreateSprite(d3ddev, &mapSprite);

	D3DXCreateTextureFromFile(d3ddev, "D:/pic_grey.jpg", &mapTextureGrey);//png for nothing
	mapTextureGrey->GetLevelDesc(NULL, &surfaceGrey);
	D3DXCreateSprite(d3ddev, &mapSprite);

	D3DXCreateFont(d3ddev, 20, 0, FW_NORMAL, 0, FALSE,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
		"Tahoma", &pFont);
}

void cleanD3D()
{
	pFont->Release();
	d3ddev->Release();    // close and release the 3D device
	d3d->Release();    // close and release Direct3D
}

void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...)
{
	char buf[1024] = { 0 };
	va_list va_alist;
	RECT FontPos = { x, y, x + 120, y + 16 };
	va_start(va_alist, fmt);
	vsprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	g_pFont->DrawTextA(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color)
{
	D3DXVECTOR2 dLine[2];

	d3dLine->SetWidth(2.f);

	dLine[0].x = x;
	dLine[0].y = y;

	dLine[1].x = xx;
	dLine[1].y = yy;

	d3dLine->Draw(dLine, 2, color);

}

void DrawBox(float x, float y, float width, float height, D3DCOLOR color)
{
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	d3dLine->SetWidth(1);
	d3dLine->Draw(points, 5, color);
}
struct vertex
{
	FLOAT x, y, z, rhw;
	DWORD color;
};

void DrawFilledCircle(float x, float y, float rad, float rotate, int type, int resolution, DWORD color)
{
	std::vector<vertex> circle(resolution + 2);
	float angle = rotate*D3DX_PI / 180;
	float pi;

	if (type == 0) pi = D3DX_PI;        // Full circle
	if (type == 1) pi = D3DX_PI / 2;      // 1/2 circle
	if (type == 2) pi = D3DX_PI / 4;   // 1/4 circle

	circle[0].x = x;
	circle[0].y = y;
	circle[0].z = 0;
	circle[0].rhw = 1;
	circle[0].color = color;

	for (int i = 1; i < resolution + 2; i++)
	{
		circle[i].x = (float)(x - rad*cos(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].y = (float)(y - rad*sin(pi*((i - 1) / (resolution / 2.0f))));
		circle[i].z = 0;
		circle[i].rhw = 1;
		circle[i].color = color;
	}

	// Rotate matrix
	int _res = resolution + 2;
	for (int i = 0; i < _res; i++)
	{
		circle[i].x = x + cos(angle)*(circle[i].x - x) - sin(angle)*(circle[i].y - y);
		circle[i].y = y + sin(angle)*(circle[i].x - x) + cos(angle)*(circle[i].y - y);
	}
	LPDIRECT3DVERTEXBUFFER9 g_pVB;
	d3ddev->CreateVertexBuffer((resolution + 2) * sizeof(vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &g_pVB, NULL);

	VOID* pVertices;
	g_pVB->Lock(0, (resolution + 2) * sizeof(vertex), (void**)&pVertices, 0);
	memcpy(pVertices, &circle[0], (resolution + 2) * sizeof(vertex));
	g_pVB->Unlock();

	d3ddev->SetTexture(0, NULL);
	d3ddev->SetPixelShader(NULL);
	d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	d3ddev->SetStreamSource(0, g_pVB, 0, sizeof(vertex));
	d3ddev->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	d3ddev->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, resolution);
	if (g_pVB != NULL) g_pVB->Release();
}



LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
		render();
		break;
	case WM_CREATE:
		//DwmExtendFrameIntoClientArea(hWnd, &margin);
		break;
	case WM_SIZE:
		ZeroMemory(&windowRect, sizeof(RECT));
		GetWindowRect(hWnd, &windowRect);
		s_width = windowRect.right - windowRect.left;
		s_height = windowRect.bottom - windowRect.top;

		// this is complete shit but its an easy workaround
		if (d3ddev) 
		{
			cleanD3D();
			initD3D(hWnd);
		}
			

		
		break;
	case WM_DESTROY:
		cleanD3D();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}