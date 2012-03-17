#pragma once

#include <string> //For class hierarchy

//Objects
#include "Objects/Object.h"
#include "Objects/RenderObject.h"
#include "Objects/Jello.h"
#include "Objects/Light.h"

//Programs
#include "OpenGL/Programs/GLProgram_Jello.h"
#include "OpenGL/Programs/GLProgram_Material.h"
#include "OpenGL/Programs/GLProgram_White.h"
#include "OpenGL/Programs/GLProgram_Reflection.h"

#include "Utils/Singleton.h" //For accessing Factory
#include "Utils/Factory.h" //For registration
#include "Utils/tree.h" //For class type hierarchy

/*-----------------------------------------------------------------------------
ClassRegister - The place to enter class data manually. This class could easily 
be autogenerated. Uses: Factory class setup
-----------------------------------------------------------------------------*/

class ClassRegister
{
public:
	ClassRegister()
	{
		/*-------------------------------------------
		//// Factory ////////////////////////////////
		-------------------------------------------*/
		Factory* factory = Singleton<Factory>::Instance();

		//Objects
		factory->register_class<Object>("Object");
		factory->register_class<RenderObject>("RenderObject");
		factory->register_class<Jello>("Jello");
		factory->register_class<Light>("Light");

		//Programs
		factory->register_class<GLProgram_Jello>("GLProgram_Jello");
		factory->register_class<GLProgram_Material>("GLProgram_Material");
		factory->register_class<GLProgram_White>("GLProgram_White");
		factory->register_class<GLProgram_Reflection>("GLProgram_Reflection");

		/*-------------------------------------------
		//// Class Hierarchy ////////////////////////
		-------------------------------------------*/
		tree<std::string>::iterator object, renderObject, jello, light;
		object =       Object::classHierarchy.set_head(Object::className);
		renderObject = Object::classHierarchy.append_child(object,RenderObject::className);
		jello =        Object::classHierarchy.append_child(renderObject,Jello::className);
		light =        Object::classHierarchy.append_child(object,Light::className);
	}
	~ClassRegister(){}
};