#pragma once

#include <glm/glm.hpp> //For math

#include "../Object.h" //Superclass

class Light: public Object
{
public:
	//Class
	static std::string className;

	//Construct/Destruct
	Light();
	virtual ~Light();

	//Initialize
	virtual void initialize(TiXmlElement* element);
	void initialize(std::string name, glm::vec4 color, float intensity);
	
	//Type
	virtual std::string getClassname();

	//Color
	void setColor(glm::vec4 color);
	glm::vec4 getColor();

	//Intensity
	void setIntensity(float intensity);
	float getIntensity();

	//Static
	static void setAmbientIntensity(glm::vec4 ambientIntensity);
	static void setLightAttenuation(float lightAttenuation);
	static void setMaxIntensity(float maxIntensity);
	static void setGamma(float gamma);

	static glm::vec4 getAmbientIntensity();
	static float getLightAttenuation();
	static float getMaxIntensity();
	static float getGamma();

private:
	void initialize(glm::vec4 color, float intensity);

	glm::vec4 color;
	float intensity;

	//Static
	static glm::vec4 ambientIntensity;
	static float lightAttenuation;
	static float maxIntensity;
	static float gamma;
};