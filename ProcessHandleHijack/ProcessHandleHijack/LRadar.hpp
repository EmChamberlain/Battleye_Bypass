#pragma once


#include "GameDataParser.hpp"
#include "Window.hpp"

# define M_PI           3.14159265358979323846  /* pi */

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
		float scaleRatio = 1.0;

		D3DXVECTOR3 pos;
		pos.x = (s_width / 2)*(1 / scaleRatio);
		pos.y = (s_height / 2)*(1 / scaleRatio);
		pos.z = 0;
		
		
		Vector3 currentPos = GDParser->m_localPlayerPosition;
		currentPos.X += float(GDParser->m_XOriginLocation);
		currentPos.Y += float(GDParser->m_YOriginLocation);
		
		


		Vector3 map_loc = coordsToMap(currentPos);
		D3DXMATRIX transMatrix = Translate((-1.0 * (map_loc.X)), -1.0 * (map_loc.Y), 0);
		//D3DXMATRIX transMatrix = Translate(0, 0, 0);
		D3DXMATRIX scaleMatrix;
		D3DXMatrixScaling(&scaleMatrix, scaleRatio, scaleRatio, 1.0);
		D3DXMATRIX resultMatrix;
		D3DXMatrixMultiply(&resultMatrix, &transMatrix, &scaleMatrix);
		
		mapSprite->SetTransform(&resultMatrix);

		mapSprite->Begin(NULL);
		switch(map)
		{
		case erangel:
			mapSprite->Draw(mapTextureErangel, NULL, NULL, &pos, 0xFFFFFFFF);
			break;
		case miramar:
			mapSprite->Draw(mapTextureMiramar, NULL, NULL, &pos, 0xFFFFFFFF);
			break;
		case savage:
			mapSprite->Draw(mapTextureSavage, NULL, NULL, &pos, 0xFFFFFFFF);
			break;
		default:
			mapSprite->Draw(mapTextureGrey, NULL, NULL, &pos, 0xFFFFFFFF);
			break;

		}			
		mapSprite->End();

		float x = (s_width) / 2;
		float y = (s_height) / 2;

		std::vector<Player> *players;
		std::vector<Vehicle> *vehicles;
		std::vector<Item> *items;

		if(GDParser->writingVectors)
		{
			players = GDParser->playersOld;
			vehicles = GDParser->vehiclesOld;
			items = GDParser->itemsOld;
		}
		else
		{
			players = GDParser->players;
			vehicles = GDParser->vehicles;
			items = GDParser->items;
		}
		
		for (Vehicle v : *vehicles)
		{
			Vector3 relative = coordsToMap(getRelativeCoords(currentPos, v.loc));
			DrawString(x + relative.X, y + relative.Y, D3DCOLOR_ARGB(255, 255, 255, 255), pFont, "%s", v.name);
			DrawFilledCircle(x + relative.X, y + relative.Y, 2, 360, 0, playerRes, D3DCOLOR_ARGB(255, 0, 0, 255));
		}
		
		for (Item i : *items)
		{
			Vector3 relative = coordsToMap(getRelativeCoords(currentPos, i.loc));
			DrawString(x + relative.X, y + relative.Y, D3DCOLOR_ARGB(255, 255, 255, 255), pFont, "%s", i.name);
			DrawFilledCircle(x + relative.X, y + relative.Y, 2, 360, 0, playerRes, D3DCOLOR_ARGB(255, 128, 0, 200));
		}
		
		for (Player p : *players)
		{
			Vector3 relative = coordsToMap(getRelativeCoords(currentPos, p.loc));
			DrawString(x + relative.X, y + relative.Y, D3DCOLOR_ARGB(255, 0, 0, 0), pFont, "%d", p.team);
		}
		
		for (Player p : *players)
		{
			Vector3 relative = coordsToMap(getRelativeCoords(currentPos, p.loc));
			D3DCOLOR color = NULL;
			if (p.team == GDParser->m_localTeam)
				color = D3DCOLOR_ARGB(255, 0, 255, 0);
			else
				color = D3DCOLOR_ARGB(255, 255, 0, 0);
			DrawFilledCircle(x + relative.X, y + relative.Y, playerRad, 360, 0, playerRes, color);
		}
		DrawFilledCircle(x, y, playerRad, 360, 0, playerRes, D3DCOLOR_ARGB(255, 255, 0, 255));// center dot (local player)
		Vector3 dir = getDirection();
		int dirScale = max(s_width, s_height);
		DrawLine(x, y, x + (dir.X * dirScale), y + (dir.Y * dirScale), D3DCOLOR_ARGB(255, 255, 0, 255));// direction aiming
	}


	Map map = savage;


private:
	GameDataParser *GDParser = nullptr;
	static const int playerRad = 3;
	static const int playerRes = 5;
	const int gameMapWidth = 819200;
	const int gameMapHeight = 819200;
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
		static float mapGameWidthRatio = float(surfaceMiramar.Width) / float(gameMapWidth);
		static float mapGameHeightRatio = float(surfaceMiramar.Height) / float(gameMapHeight);
		Vector3 toReturn;
		toReturn.X = coords.X * mapGameWidthRatio;
		toReturn.Y = coords.Y * mapGameHeightRatio;
		return toReturn;
	}

	Vector3 getRelativeCoords(Vector3 origin, Vector3 coords)
	{
		Vector3 toReturn;
		toReturn.X = coords.X - origin.X;
		toReturn.Y = coords.Y - origin.Y;
		toReturn.Z = coords.Z - origin.Z;
		/*toReturn.X = coords.X - 813000 / 2.0;
		toReturn.Y = coords.Y - 813000 / 2.0;
		toReturn.Z = coords.Z - 0;*/
		return toReturn;
	}
	
	float distance(Vector3 a, Vector3 b)
	{
		return (a.X - b.X)*(a.X - b.X) + (a.Y - b.Y)*(a.Y - b.Y) + (a.Z - b.Z)*(a.Z - b.Z);
	}
	Vector3 getDirection()
	{
		Vector3 toReturn;
		Vector3 rot = GDParser->m_playerCameraRotation;
		toReturn.X = cosf(rot.Y * M_PI / 180.0);
		toReturn.Y = sinf(rot.Y * M_PI / 180.0);
		toReturn.Z = 0.0;
		return toReturn;
	}

};