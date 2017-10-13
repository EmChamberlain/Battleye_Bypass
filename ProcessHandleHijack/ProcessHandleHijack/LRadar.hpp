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
		float x = (s_width) / 2;
		float y = (s_height) / 2;
		DrawFilledCircle(x, y, 50, 360, 0, 50,  D3DCOLOR_ARGB(255, 255, 0, 255));
		/*for (Player p : GDParser->players)
		{
			
		}*/
	}






private:
	GameDataParser *GDParser = nullptr;
	static const int playerRad = 8;


	int coordToMap(float in) 
	{
		return int(in*0.01);
	}
		
	



};