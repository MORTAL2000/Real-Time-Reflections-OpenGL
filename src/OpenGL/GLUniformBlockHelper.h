#pragma once

#include <map> //For storing UniformBlocks
#include <vector> //For storage
#include <string> //For names

#include "GLUniformBlock.h" //For storing UniformBlocks
#include "GLState.h" //For getting current world values
#include "GLView.h" //For camera values
#include "../Utils/Singleton.h" //For accessing GLState

class GLUniformBlockHelper
{
public:
	GLUniformBlockHelper();
	~GLUniformBlockHelper();

	void initialize();
	void updateAll();
	void update(std::string name);
	GLUniformBlock* findUniformBlock(std::string name);

	static std::string TYPE_PROJECTION;

private:
	std::map<std::string, GLUniformBlock*> uniformBlockMap;
};

//----------------
// Structs ///////
//----------------
struct ProjectionBlock
{
	glm::mat4 cameraToClipMatrix;
	float zNear;
	float zFar;
	float screenWidth;
	float screenHeight;
};