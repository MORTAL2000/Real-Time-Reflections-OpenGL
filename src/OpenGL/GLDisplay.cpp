#include "GLDisplay.h"

GLDisplay::GLDisplay(){}
GLDisplay::~GLDisplay(){}

void GLDisplay::initialize()
{
	//Initializers
	this->initializeGL();
	this->initializeCamera();
	this->initializeFramebuffers();

	//Event handlers
	Singleton<EventHandler>::Instance()->addEnterFrameEventListener(EnterFrameReceiver::from_method<GLDisplay,&GLDisplay::update>(this));
	Singleton<EventHandler>::Instance()->addEnterFrameEventListener(EnterFrameReceiver::from_method<GLDisplay,&GLDisplay::checkKeyPress>(this));
	Singleton<EventHandler>::Instance()->addInputEventListener(sf::Event::Resized,InputReceiver::from_method<GLDisplay,&GLDisplay::resize>(this));
	Singleton<EventHandler>::Instance()->addInputEventListener(sf::Event::MouseButtonPressed,InputReceiver::from_method<GLDisplay,&GLDisplay::mouseButtonPressed>(this));
	Singleton<EventHandler>::Instance()->addInputEventListener(sf::Event::MouseMoved,InputReceiver::from_method<GLDisplay,&GLDisplay::mouseMoved>(this));
	Singleton<EventHandler>::Instance()->addInputEventListener(sf::Event::MouseWheelMoved,InputReceiver::from_method<GLDisplay,&GLDisplay::mouseWheelMoved>(this));
}
void GLDisplay::initializeGL()
{
	gl3wInit();
	Singleton<GLUniformBlockHelper>::Instance()->initialize();

    //Set GL global vars
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);
	glDisable(GL_BLEND);
    //glPointSize(10);
	//glLineWidth(2);
	//glEnable (GL_BLEND); 
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//glPolygonOffset(10.0f,10.0f);
}
void GLDisplay::initializeCamera()
{
	//Camera to Clip
	float fov = 45.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;
	Singleton<GLView>::Instance()->calcCameraToClipMatrix(fov,nearPlane,farPlane);
}
void GLDisplay::initializeFramebuffers()
{
	this->gbufferFBO = new GLFramebuffer_GBuffer();
	this->gbufferFBO->initialize();

	//Instantiate quad mesh that we use to render the whole screen
	GLMeshData* quadMeshData = Singleton<MeshDatabase>::Instance()->loadMesh("quad");
	this->fullScreenQuadMesh = new GLMesh();
	this->fullScreenQuadMesh->initialize(quadMeshData);
}
void GLDisplay::initializePhysics()
{
	//Singleton<PhysicsSceneDefault>::Instance()->makeDefaultScene(this->world);
	//this->physicsWorld = Singleton<PhysicsSceneDefault>::Instance()->getScene();
	//Singleton<PhysicsIO>::Instance()->initialize();
}
void GLDisplay::update()
{ 
	if(this->world != 0)
	{
		GLState* glState = Singleton<GLState>::Instance();
		GLView* glView = Singleton<GLView>::Instance();
		GLUniformBlockHelper* glUniformBlockHelper = Singleton<GLUniformBlockHelper>::Instance();
		
		//Update texture unit positions
		glState->positionTextureUnit = GLFramebuffer_GBuffer::GBUFFER_TEXTURE_TYPE_POSITION;
		glState->normalTextureUnit = GLFramebuffer_GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL;
		glState->diffuseColorTextureUnit = GLFramebuffer_GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE_COLOR;
		glState->specularColorTextureUnit = GLFramebuffer_GBuffer::GBUFFER_TEXTURE_TYPE_SPECULAR_COLOR;
		glState->otherTextureUnit = GLFramebuffer_GBuffer::GBUFFER_TEXTURE_TYPE_OTHER;
		glState->depthTextureUnit = GLFramebuffer_GBuffer::GBUFFER_NUM_TEXTURES;

		//Update everything
		//physicsWorld->update();
		world->update();
		glState->worldToCameraMatrix = this->camera->getWorldToCameraMatrix();
		glUniformBlockHelper->updateAll();

		//Render geometry to textures
		glState->globalProgramName = "DeferredGeometryPass";
		this->gbufferFBO->bindForWriting();
		this->clearGL(); //clear buffers
		world->render(); //render world to textures

		//Read from textures and draw quadrants screen
		glState->globalProgramName = "DeferredLightingPass";
		ShadowLight* light = (ShadowLight*)this->world->getObjectsByType("ShadowLight").at(0);
		glState->lightIntensity = glm::vec3(light->getIntensity());
		glState->lightCameraSpacePosition = glm::vec3(this->camera->getWorldToCameraMatrix()*glm::vec4(light->getTranslation(),1.0f));

		this->gbufferFBO->bindForReading();
		this->clearGL();
		this->fullScreenQuadMesh->Render();

		/*
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		this->clearGL();
		this->gbufferFBO->bindForReading();
		glm::ivec2 windowDimensions = glView->getWindowDimensions();
		GLint windowWidth = (GLint)(windowDimensions.x);
		GLint windowHeight = (GLint)(windowDimensions.y);
        GLint halfWidth = (GLint)(windowDimensions.x/2.0f);
        GLint halfHeight = (GLint)(windowDimensions.y/2.0f);
        
        this->gbufferFBO->setReadBuffer(GLFramebuffer_GBuffer::GBUFFER_TEXTURE_TYPE_POSITION);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, 0, halfWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        this->gbufferFBO->setReadBuffer(GLFramebuffer_GBuffer::GBUFFER_TEXTURE_TYPE_NORMAL);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, 0, halfHeight, halfWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        this->gbufferFBO->setReadBuffer(GLFramebuffer_GBuffer::GBUFFER_TEXTURE_TYPE_DIFFUSE_COLOR);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, halfWidth, halfHeight, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
        this->gbufferFBO->setReadBuffer(GLFramebuffer_GBuffer::GBUFFER_TEXTURE_TYPE_SPECULAR_COLOR);
        glBlitFramebuffer(0, 0, windowWidth, windowHeight, halfWidth, 0, windowWidth, halfHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		*/
	}
}
void GLDisplay::clearGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void GLDisplay::resize(sf::Event sfEvent)
{
	int width = sfEvent.Size.Width;
	int height = sfEvent.Size.Height;
	this->resize(width,height);
}

//IO Events
void GLDisplay::resize(int width, int height)
{
	Singleton<GLView>::Instance()->setWindowDimensions(width,height);
}
void GLDisplay::mouseButtonPressed(sf::Event sfEvent)
{
	EventHandler* eventHandler = Singleton<EventHandler>::Instance();
	glm::ivec2 mousePos = eventHandler->getMousePos();
	if(sfEvent.MouseButton.Button == sf::Mouse::Left)
	{
		if(!eventHandler->isAltDown())
		{
			RenderObject* oldSelectedObject = this->selectedObject;
			this->selectedObject = 0;
			float closestDistance = FLT_MAX;

			Ray clickRay = Singleton<GLView>::Instance()->getPickingRay(mousePos.x,mousePos.y);
			std::vector<Object*> renderObjects = this->world->getObjectsByType("RenderObject");
			for(unsigned int i = 0; i < renderObjects.size(); i++)
			{
				RenderObject* renderObject = (RenderObject*)renderObjects.at(i);
				glm::mat4 transformationMatrix = renderObject->getTransformationMatrix();
				BoundingBox* boundingBox = renderObject->getMesh()->getGLMeshData()->boundingBox;
				IntersectionData intersectionData = IntersectionAlgorithms::RayBoxIntersect(clickRay,transformationMatrix, boundingBox);
				if(intersectionData.valid && intersectionData.distanceAlongRay < closestDistance)
				{
					closestDistance = intersectionData.distanceAlongRay;
					this->selectedObject = renderObject;	
				}
			}
			if(selectedObject != 0)
				this->selectedObject->getMaterial()->diffuseColor += 1.0f;
			if(oldSelectedObject != 0)
				oldSelectedObject->getMaterial()->diffuseColor -= 1.0f;
		}
	}
}
void GLDisplay::mouseMoved(sf::Event sfEvent)
{
	EventHandler* eventHandler = Singleton<EventHandler>::Instance();
	glm::ivec2 mousePos = eventHandler->getMousePos();
	glm::ivec2 prevMousePos = eventHandler->getPrevMousePos();
	int mouseXDiff = (mousePos.x - prevMousePos.x);
	int mouseYDiff = (mousePos.y - prevMousePos.y);

	bool altIsDown = eventHandler->isAltDown();
	if(eventHandler->isLeftMouseDown() && altIsDown)
	{	
		float scaleFactor = .008f;
		float mouseXDifference = -(float)mouseXDiff * scaleFactor;
		float mouseYDifference = (float)mouseYDiff * scaleFactor;
		this->camera->rotateRad(mouseXDifference,mouseYDifference);
	}
	else if(eventHandler->isMiddleMouseDown() && altIsDown)
	{
		float scaleFactor = .01f;
		float mouseXDifference = -(float)mouseXDiff * scaleFactor;
		float mouseYDifference = (float)mouseYDiff * scaleFactor;
		this->camera->pan(mouseXDifference,mouseYDifference);
	}
	else if(eventHandler->isRightMouseDown() && altIsDown)
	{
		float scaleFactor = .05f;
		float mouseYDifference = -(float)mouseYDiff * scaleFactor;
		this->camera->zoom(mouseYDifference);
	}
}
void GLDisplay::mouseWheelMoved(sf::Event sfEvent)
{
	int delta = sfEvent.MouseWheel.Delta;
	float scaleFactor = 1.0f;
	this->camera->zoom(delta*scaleFactor);
}
void GLDisplay::checkKeyPress()
{
	EventHandler* eventHandler = Singleton<EventHandler>::Instance();
	if(this->selectedObject != 0)
	{
		float translationAmount = .03f;
		float rotationAmount = 1.0f;
		float colorAmount = 0.05f;
		float scaleAmount = 1.01f;
		glm::vec3 camUp = glm::normalize(this->camera->getUpDir());
		glm::vec3 camLook = glm::normalize(this->camera->getLookDir());
		glm::vec3 camRight = glm::normalize(glm::cross(camLook,camUp));
		glm::vec3 moveX = /*glm::vec3(translationAmount,0,0);*/translationAmount*camRight;
		glm::vec3 moveY = /*glm::vec3(0,translationAmount,0);*/translationAmount*camUp;
		glm::vec3 moveZ = /*glm::vec3(0,0,translationAmount);*/translationAmount*camLook;

		if(eventHandler->isKeyDown(sf::Keyboard::A))
			selectedObject->translate(-moveX);
		if(eventHandler->isKeyDown(sf::Keyboard::D))
			selectedObject->translate(moveX);
		if(eventHandler->isKeyDown(sf::Keyboard::W))
			selectedObject->translate(moveY);
		if(eventHandler->isKeyDown(sf::Keyboard::S))
			selectedObject->translate(-moveY);
		if(eventHandler->isKeyDown(sf::Keyboard::Q))
			selectedObject->translate(moveZ);
		if(eventHandler->isKeyDown(sf::Keyboard::E))
			selectedObject->translate(-moveZ);
		if(eventHandler->isKeyDown(sf::Keyboard::R))
			selectedObject->rotate(glm::vec3(0.0f,1.0f,0.0f),rotationAmount);
		if(eventHandler->isKeyDown(sf::Keyboard::V))
			selectedObject->getMaterial()->diffuseColor += colorAmount;
		if(eventHandler->isKeyDown(sf::Keyboard::C))
			selectedObject->getMaterial()->diffuseColor -= colorAmount;
		if(eventHandler->isKeyDown(sf::Keyboard::Z))
			selectedObject->scale(scaleAmount);
		if(eventHandler->isKeyDown(sf::Keyboard::X))
			selectedObject->scale(2.0f-scaleAmount);
	}
}

//World
void GLDisplay::setWorld(World* world)
{
	this->world = world;
	this->camera = (Camera3rdPerson*)this->world->getObjectsByType("Camera3rdPerson",true).at(0);
	this->selectedObject = 0;
}
World* GLDisplay::getWorld()
{
	return this->world;
}

//Camera
void GLDisplay::setCamera(Camera* camera)
{
	this->camera = camera;
}
Camera* GLDisplay::getCamera()
{
	return this->camera;
}