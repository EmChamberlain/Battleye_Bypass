#pragma once

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 
#include <TlHelp32.h>


#define CENTERX (GetSystemMetrics(SM_CXSCREEN)/2)-(s_width/2)
#define CENTERY (GetSystemMetrics(SM_CYSCREEN)/2)-(s_height/2)

//defining data
extern int s_width;
extern int s_height;
extern LPDIRECT3D9 d3d;
extern LPDIRECT3DDEVICE9 d3ddev;
extern HWND hWnd;
extern LPD3DXFONT pFont;
extern ID3DXLine* d3dLine;
extern LPDIRECT3DTEXTURE9 mapTexture;
extern RECT windowRect;
extern D3DVIEWPORT9 viewport;




LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const char *fmt, ...);
void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color);
void DrawBox(float x, float y, float width, float height, D3DCOLOR color);
void DrawFilledCircle(float x, float y, float rad, float rotate, int type, int resolution, DWORD color);
void initD3D(HWND hWnd);
void render();