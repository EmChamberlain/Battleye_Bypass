#pragma once


#include "GameDataParser.hpp"



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

	}






private:
	GameDataParser *GDParser = nullptr;




};