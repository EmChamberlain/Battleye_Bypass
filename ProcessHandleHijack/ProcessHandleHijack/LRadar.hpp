#pragma once


#include "GameDataParser.hpp"
#include "Window.hpp"


class LRadar
{
public:
	LRadar(GameDataParser *gdp)
	{
		GDParser = gdp;

	}
	~LRadar()
	{
		delete GDParser;
	}

	
	void render()
	{
		Vector3 mock_loc = { 813000 / 2, 813000 / 2, 0 };
		Vector3 mock_enemy = { ((813000 / 2) + 10000.0), ((813000 / 2) + 20000.0), 0 };
		float scaleRatio = 1.0;

		D3DXVECTOR3 pos;
		pos.x = (s_width / 2)*(1 / scaleRatio);
		pos.y = (s_height / 2)*(1 / scaleRatio);
		pos.z = 0;
		
		Vector3 map_loc = coordsToMap(mock_loc);

		
		
		D3DXMATRIX transMatrix = Translate(-1.0 * map_loc.X, -1.0 * map_loc.Y, 0);
		//D3DXMATRIX transMatrix = Translate(0, 0, 0);
		D3DXMATRIX scaleMatrix;
		D3DXMatrixScaling(&scaleMatrix, scaleRatio, scaleRatio, 1.0);
		D3DXMATRIX resultMatrix;
		D3DXMatrixMultiply(&resultMatrix, &transMatrix, &scaleMatrix);
		
		mapSprite->SetTransform(&resultMatrix);

		mapSprite->Begin(NULL);
		mapSprite->Draw(mapTexture, NULL, NULL, &pos, 0xFFFFFFFF);
		mapSprite->End();

		float x = (s_width) / 2;
		float y = (s_height) / 2;
		

		Vector3 mock_enemy_map_relative = coordsToMap(getRelativeCoords(mock_enemy));
		DrawFilledCircle(x + mock_enemy_map_relative.X, y + mock_enemy_map_relative.Y, playerRad, 360, 0, playerRes, D3DCOLOR_ARGB(255, 0, 0, 255));
		for (Player p : GDParser->players)
		{
			
		}
		DrawFilledCircle(x, y, playerRad, 360, 0, playerRes, D3DCOLOR_ARGB(255, 255, 0, 255));// center dot (local player)
		DrawString(x, y, D3DCOLOR_ARGB(255, 255, 255, 255), pFont, "%d", 0);
	}






private:
	GameDataParser *GDParser = nullptr;
	static const int playerRad = 3;
	static const int playerRes = 5;
	const int gameMapWidth = 813000; 
	const int gameMapHeight = 813000;
	const int gameMapMeterWidth = 8000;
	const int gameMapMeterHeight = 8000;

	
	D3DXMATRIX Translate(const float dx, const float dy, const float dz) {
		D3DXMATRIX ret;

		D3DXMatrixIdentity(&ret);
		ret(3, 0) = dx;
		ret(3, 1) = dy;
		ret(3, 2) = dz;
		return ret;
	}    // End of Translate

	Vector3 coordsToMap(Vector3 coords)
	{
		static float mapGameWidthRatio = float(surface.Width) / float(gameMapWidth);
		static float mapGameHeightRatio = float(surface.Height) / float(gameMapHeight);
		Vector3 toReturn;
		toReturn.X = coords.X * mapGameWidthRatio;
		toReturn.Y = coords.Y * mapGameHeightRatio;
		return toReturn;
	}

	Vector3 getRelativeCoords(Vector3 coords)
	{
		Vector3 toReturn;
		toReturn.X = coords.X - GDParser->m_localPlayerPosition.X;
		toReturn.Y = coords.Y - GDParser->m_localPlayerPosition.Y;
		toReturn.Z = coords.Z - GDParser->m_localPlayerPosition.Z;
		/*toReturn.X = coords.X - 813000 / 2.0;
		toReturn.Y = coords.Y - 813000 / 2.0;
		toReturn.Z = coords.Z - 0;*/
		return toReturn;
	}
	



};