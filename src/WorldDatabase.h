#pragma once

#include <string> //For names
#include <map> //For storing worlds

#include "World.h" //For storing worlds
#include "GlobalPaths.h" //For getting worlds path
#include "Utils/Singleton.h" //For using global classes

class WorldDatabase
{
public:
	WorldDatabase();
	~WorldDatabase();

	World* loadWorld(std::string worldFileName);

	static std::string NONE;

private:
	World* findWorld(std::string worldFileName);
	World* openWorldFile(std::string worldFileName);
	std::map<std::string, World*> worldMap;
};