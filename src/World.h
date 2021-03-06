#pragma once

#include <algorithm> //For finding object in array
#include <vector> //For storing objects
#include <map> //For storing object
#include <string> //For object names

#include <tinyxml/tinyxml.h> //For parsing world xml file

#include "Utils/Singleton.h" //For using global classes
#include "Objects/Object.h" //Storage type
#include "OpenGL/GLState.h" //Takes the world's lights
#include "Objects/Lights/Light.h" //For sending lights to GLState
#include "Utils/Factory.h" //For constructing new objects during initialization

class World
{
public:
	World();
	~World();

	void initialize(TiXmlElement* element);

	void update();
	void render();

	void addObject(Object* object);
	void removeObject(Object* object);
	void removeObject(std::string name);

	std::vector<Object*> getObjectsByType(std::string type, bool exclusive = true);
	Object* getObjectByName(std::string name);
	Object* getObjectByNameAndType(std::string type, std::string name);

	//Name
	void setName(std::string name);
	std::string getName();
	
private:

	std::map<std::string,std::vector<Object*>> objectMap;
	std::string name;
};