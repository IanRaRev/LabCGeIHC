//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>

//glfw include
#include <GLFW/glfw3.h>

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"
// Geometry primitives
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
//Texture includes
#include "Headers/Texture.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

Sphere sphere(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Box box, box1;

int cambioIluminacion = 1;

Shader shaderColor;
Shader shaderTexture;
Shader shaderCubeTexture;
Shader shaderMateriales;
Shader shaderDirectionLight;
Shader shaderPointLight;
Shader shaderSpotLight;

GLuint fachada, pared, pasto, suelo, paredFondo, TV, vestidor, textureCubeTexture;
GLuint MB, buro, madera, mesa, colchon;
GLuint cubeTextureID;

GLenum types[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

std::string fileNames[6] = {
	/*
	"../../Textures/mp_bloodvalley/blood-valley_ft.tga",
	"../../Textures/mp_bloodvalley/blood-valley_bk.tga",
	"../../Textures/mp_bloodvalley/blood-valley_up.tga",
	"../../Textures/mp_bloodvalley/blood-valley_dn.tga",
	"../../Textures/mp_bloodvalley/blood-valley_rt.tga",
	"../../Textures/mp_bloodvalley/blood-valley_lf.tga" */
	"../../Textures/ame_nebula/purplenebula_ft.tga",
	"../../Textures/ame_nebula/purplenebula_bk.tga",
	"../../Textures/ame_nebula/purplenebula_up.tga",
	"../../Textures/ame_nebula/purplenebula_dn.tga",
	"../../Textures/ame_nebula/purplenebula_rt.tga",
	"../../Textures/ame_nebula/purplenebula_lf.tga"
};

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX, offsetX;
int lastMousePosY, offsetY;

double deltaTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroyWindow();
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
			glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
			nullptr);

	if (window == nullptr) {
		std::cerr
			<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
			<< std::endl;
		destroyWindow();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);

	shaderColor.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");
	shaderTexture.initialize("../../Shaders/texturizado_res.vs", "../../Shaders/texturizado_res.fs");
	shaderCubeTexture.initialize("../../Shaders/cubeTexture.vs", "../../Shaders/cubeTexture.fs");
	shaderMateriales.initialize("../../Shaders/iluminacion_materiales_res.vs", "../../Shaders/iluminacion_materiales_res.fs");
	shaderDirectionLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/directionalLight.fs");
	shaderPointLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/pointLight.fs");
	shaderSpotLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/spotLight.fs");

	sphere.init();
	cylinder.init();
	/* Cubo habitación*/
	box.init();
	/* Cubos para muebles */
	box1.init();

	camera->setPosition(glm::vec3(0.0f, 0.0f, 15.0f));

	/* Textura fachada */
	int imageWidth, imageHeight;
	Texture texture("../../Textures/fachada.jpg");
	FIBITMAP* bitmap = texture.loadImage(false);
	unsigned char * data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &fachada);
	glBindTexture(GL_TEXTURE_2D, fachada);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	/* Textura pared */
	texture = Texture("../../Textures/pared.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &pared);
	glBindTexture(GL_TEXTURE_2D, pared);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	/* Textura pasto*/
	texture = Texture("../../Textures/pasto.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &pasto);
	glBindTexture(GL_TEXTURE_2D, pasto);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);


	/* Textura suelo*/
	texture = Texture("../../Textures/suelo.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &suelo);
	glBindTexture(GL_TEXTURE_2D, suelo);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	/* Textura pared con ventana*/
	texture = Texture("../../Textures/pared.gif");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &paredFondo);
	glBindTexture(GL_TEXTURE_2D, paredFondo);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);


	/* Textura TV*/
	texture = Texture("../../Textures/TV.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &TV);
	glBindTexture(GL_TEXTURE_2D, TV);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);


	/* Textura Vestidor */
	texture = Texture("../../Textures/vestidor.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &vestidor);
	glBindTexture(GL_TEXTURE_2D, vestidor);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	/* MB */
	texture = Texture("../../Textures/MB.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &MB);
	glBindTexture(GL_TEXTURE_2D, MB);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);


	/* Buró */
	texture = Texture("../../Textures/buro.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &buro);
	glBindTexture(GL_TEXTURE_2D, buro);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	/* Madera */
	texture = Texture("../../Textures/madera.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &madera);
	glBindTexture(GL_TEXTURE_2D, madera);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);


	/* Mesa */
	texture = Texture("../../Textures/mesa.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &mesa);
	glBindTexture(GL_TEXTURE_2D, mesa);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	/* Colchon */
	texture = Texture("../../Textures/cobertor.jpg");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &colchon);
	glBindTexture(GL_TEXTURE_2D, colchon);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);


	glGenTextures(1, &cubeTextureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		texture = Texture(fileNames[i]);
		FIBITMAP* bitmap = texture.loadImage(true);
		data = texture.convertToData(bitmap, imageWidth, imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		texture.freeImage(bitmap);
	}
}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shaderColor.destroy();
	shaderTexture.destroy();
	shaderCubeTexture.destroy();
	shaderMateriales.destroy();
	shaderDirectionLight.destroy();
	shaderPointLight.destroy();
	shaderSpotLight.destroy();
	sphere.destroy();
	cylinder.destroy();
	box.destroy();
	box1.destroy();
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) 
	{
		switch (key) 
		{
			case GLFW_KEY_ESCAPE:
				exitApp = true;
				break;
			case GLFW_KEY_P:
				cambioIluminacion = 1;
				break;
			case GLFW_KEY_L:
				cambioIluminacion = 2;
				break;
			case GLFW_KEY_M:
				cambioIluminacion = 3;
				break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	glm::vec3 objPosition = glm::vec3(0.0f, 0.0f, -3.0f);
	float angle = 0.0;
	float ratio = 10.0;

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		glm::mat4 view = camera->getViewMatrix();

		glm::mat4 cubeModelMatrix = glm::translate(glm::mat4(1.0f), objPosition);

		// Iluminación
		glm::mat4 lightModelmatrix = glm::rotate(cubeModelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix, glm::vec3(0.0f, 0.0f, -ratio));

		if (cambioIluminacion == 1)
		{
			shaderDirectionLight.turnOn();
			glUniform3fv(shaderDirectionLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 0.0f)));
			glUniform3fv(shaderDirectionLight.getUniformLocation("light.direction"), 1, glm::value_ptr(glm::vec3(-0.3f, -1.0f, -0.2f)));
			//glUniform3fv(shaderDirectionLight.getUniformLocation("light.position"), 1, glm::value_ptr(camera->getPosition()));
			//glUniform3fv(shaderDirectionLight.getUniformLocation("light.direction"), 1, glm::value_ptr(camera->getFront()));
			glUniform3fv(shaderDirectionLight.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
			glUniform3f(shaderDirectionLight.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
			glUniform3f(shaderDirectionLight.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
			glUniform3f(shaderDirectionLight.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
			glUniform3f(shaderDirectionLight.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
			glUniform3f(shaderDirectionLight.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
			glUniform3f(shaderDirectionLight.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
			//glUniform1f(shaderDirectionLight.getUniformLocation("light.cutOff"), glm::cos(glm::radians(12.5f)));
			//glUniform1f(shaderDirectionLight.getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(15.0f)));
			glUniform1f(shaderDirectionLight.getUniformLocation("light.constant"), 1.0f);
			glUniform1f(shaderDirectionLight.getUniformLocation("light.linear"), 0.14f);
			glUniform1f(shaderDirectionLight.getUniformLocation("light.quadratics"), 0.07f);
			glUniform1f(shaderDirectionLight.getUniformLocation("material.shininess"), 32.0);

			glBindTexture(GL_TEXTURE_2D, madera);
			cylinder.setProjectionMatrix(projection);
			cylinder.setViewMatrix(view);
			cylinder.setShader(&shaderDirectionLight);
			cylinder.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cubeModelMatrix);
			glBindTexture(GL_TEXTURE_2D, fachada);
			cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cylinder.getSlices() * 3, cubeModelMatrix);
			glBindTexture(GL_TEXTURE_2D, pasto);
			cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3 + cylinder.getSlices() * 3, cylinder.getSlices() * 3, cubeModelMatrix);
			glBindTexture(GL_TEXTURE_2D, 0);


			//cylinder.setShader(&shaderTexture);
			cylinder.setProjectionMatrix(projection);
			cylinder.setViewMatrix(view);
			cylinder.setPosition(glm::vec3(2.0, -3.6, -3.0));
			cylinder.setScale(glm::vec3(0.1, 0.7, 0.1));
			glBindTexture(GL_TEXTURE_2D, madera);
			cylinder.render();

			cylinder.setProjectionMatrix(projection);
			cylinder.setViewMatrix(view);
			cylinder.setPosition(glm::vec3(-4.0, -3.6, 0.0));
			cylinder.setScale(glm::vec3(0.1, 0.7, 0.1));
			glBindTexture(GL_TEXTURE_2D, madera);
			cylinder.render();

			/* Cubo casa */
			box.setProjectionMatrix(projection);
			box.setViewMatrix(view);
			box.setShader(&shaderDirectionLight);;
			box.setScale(glm::vec3(10.0f, 8.0f, 20.0f));
			box.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));


			/* Fachada*/
			glBindTexture(GL_TEXTURE_2D, fachada);
			box.render(0.0, 6.0);
			/* Pared derecha*/
			glBindTexture(GL_TEXTURE_2D, pared);
			box.render(6.0, 6.0);
			/* Pared izquierda */
			glBindTexture(GL_TEXTURE_2D, pared);
			box.render(18.0, 6.0);
			/*Suelo*/
			glBindTexture(GL_TEXTURE_2D, suelo);
			box.render(30.0, 6.0);
			/*Pared fondo*/
			glBindTexture(GL_TEXTURE_2D, paredFondo);
			box.render(12.0, 6.0);

			/* Jardin */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setShader(&shaderDirectionLight);
			box1.setScale(glm::vec3(20.0f, 20.0f, 40.0f));
			box1.setPosition(glm::vec3(0.0f, 5.99f, 0.0f));
			/* Jardin xD*/
			glBindTexture(GL_TEXTURE_2D, pasto);
			box1.render(30.0, 6.0);

			/* Televisión */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(0.2f, 2.0f, 3.0f));
			box1.setPosition(glm::vec3(4.79f, 1.0f, -5.0f));
			glBindTexture(GL_TEXTURE_2D, TV);
			box1.render();

			/* Ropero */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(5.0f, 4.0f, 1.0f));
			box1.setPosition(glm::vec3(2.0f, -2.0f, -9.5f));
			glBindTexture(GL_TEXTURE_2D, vestidor);
			box1.render(0.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, MB);
			box1.render(18.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, MB);
			box1.render(24.0, 6.0);


			/* Buró */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
			box1.setPosition(glm::vec3(-4.0f, -3.0f, 0.0f));
			glBindTexture(GL_TEXTURE_2D, buro);
			box1.render(6.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render(12.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render(0.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render(24.0, 6.0);

			/* Mesa */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(1.0f, 5.0f, 5.0f));
			box1.setPosition(glm::vec3(-4.0f, -3.0f, 6.0f));
			glBindTexture(GL_TEXTURE_2D, mesa);
			box1.render(6.0, 6.0);


			/* Cama */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(6.0f, 0.51f, 4.0f));
			box1.setPosition(glm::vec3(-1.0f, -3.0f, -5.0f));
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render();

			/* Cama */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(6.0f, 0.4f, 4.0f));
			box1.setPosition(glm::vec3(-1.0f, -2.59f, -5.0f));
			glBindTexture(GL_TEXTURE_2D, colchon);
			box1.render();

			shaderDirectionLight.turnOff();
			glBindTexture(GL_TEXTURE_2D, 0);
			//cylinder.render();
		}

		else if (cambioIluminacion == 2)
		{
			// Iluminación

			shaderPointLight.turnOn();
			glUniform3fv(shaderPointLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(0.5f, 0.0f, 0.0f)));
			glUniform3fv(shaderPointLight.getUniformLocation("light.direction"), 1, glm::value_ptr(glm::vec3(-0.3f, -1.0f, -0.2f)));
			//glUniform3fv(shaderPointLight.getUniformLocation("light.position"), 1, glm::value_ptr(camera->getPosition()));
			//glUniform3fv(shaderPointLight.getUniformLocation("light.direction"), 1, glm::value_ptr(camera->getFront()));
			glUniform3fv(shaderPointLight.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
			glUniform3f(shaderPointLight.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
			glUniform3f(shaderPointLight.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
			glUniform3f(shaderPointLight.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
			glUniform3f(shaderPointLight.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
			glUniform3f(shaderPointLight.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
			glUniform3f(shaderPointLight.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
			//glUniform1f(shaderPointLight.getUniformLocation("light.cutOff"), glm::cos(glm::radians(12.5f)));
			//glUniform1f(shaderPointLight.getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(15.0f)));
			glUniform1f(shaderPointLight.getUniformLocation("light.constant"), 1.0f);
			glUniform1f(shaderPointLight.getUniformLocation("light.linear"), 0.14f);
			glUniform1f(shaderPointLight.getUniformLocation("light.quadratics"), 0.07f);
			glUniform1f(shaderPointLight.getUniformLocation("material.shininess"), 32.0);

			glBindTexture(GL_TEXTURE_2D, madera);
			cylinder.setProjectionMatrix(projection);
			cylinder.setViewMatrix(view);
			cylinder.setShader(&shaderPointLight);
			cylinder.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cubeModelMatrix);
			glBindTexture(GL_TEXTURE_2D, fachada);
			cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cylinder.getSlices() * 3, cubeModelMatrix);
			glBindTexture(GL_TEXTURE_2D, pasto);
			cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3 + cylinder.getSlices() * 3, cylinder.getSlices() * 3, cubeModelMatrix);
			glBindTexture(GL_TEXTURE_2D, 0);


			//cylinder.setShader(&shaderTexture);
			cylinder.setProjectionMatrix(projection);
			cylinder.setViewMatrix(view);
			cylinder.setPosition(glm::vec3(2.0, -3.6, -3.0));
			cylinder.setScale(glm::vec3(0.1, 0.7, 0.1));
			glBindTexture(GL_TEXTURE_2D, madera);
			cylinder.render();

			cylinder.setProjectionMatrix(projection);
			cylinder.setViewMatrix(view);
			cylinder.setPosition(glm::vec3(-4.0, -3.6, 0.0));
			cylinder.setScale(glm::vec3(0.1, 0.7, 0.1));
			glBindTexture(GL_TEXTURE_2D, madera);
			cylinder.render();

			/* Cubo casa */
			box.setProjectionMatrix(projection);
			box.setViewMatrix(view);
			box.setShader(&shaderPointLight);
			box.setScale(glm::vec3(10.0f, 8.0f, 20.0f));
			box.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));


			/* Fachada*/
			glBindTexture(GL_TEXTURE_2D, fachada);
			box.render(0.0, 6.0);
			/* Pared derecha*/
			glBindTexture(GL_TEXTURE_2D, pared);
			box.render(6.0, 6.0);
			/* Pared izquierda */
			glBindTexture(GL_TEXTURE_2D, pared);
			box.render(18.0, 6.0);
			/*Suelo*/
			glBindTexture(GL_TEXTURE_2D, suelo);
			box.render(30.0, 6.0);
			/*Pared fondo*/
			glBindTexture(GL_TEXTURE_2D, paredFondo);
			box.render(12.0, 6.0);

			/* Jardin */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setShader(&shaderPointLight);
			box1.setScale(glm::vec3(20.0f, 20.0f, 40.0f));
			box1.setPosition(glm::vec3(0.0f, 5.99f, 0.0f));
			/* Jardin xD*/
			glBindTexture(GL_TEXTURE_2D, pasto);
			box1.render(30.0, 6.0);

			/* Televisión */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(0.2f, 2.0f, 3.0f));
			box1.setPosition(glm::vec3(4.79f, 1.0f, -5.0f));
			glBindTexture(GL_TEXTURE_2D, TV);
			box1.render();

			/* Ropero */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(5.0f, 4.0f, 1.0f));
			box1.setPosition(glm::vec3(2.0f, -2.0f, -9.5f));
			glBindTexture(GL_TEXTURE_2D, vestidor);
			box1.render(0.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, MB);
			box1.render(18.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, MB);
			box1.render(24.0, 6.0);


			/* Buró */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
			box1.setPosition(glm::vec3(-4.0f, -3.0f, 0.0f));
			glBindTexture(GL_TEXTURE_2D, buro);
			box1.render(6.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render(12.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render(0.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render(24.0, 6.0);

			/* Mesa */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(1.0f, 5.0f, 5.0f));
			box1.setPosition(glm::vec3(-4.0f, -3.0f, 6.0f));
			glBindTexture(GL_TEXTURE_2D, mesa);
			box1.render(6.0, 6.0);


			/* Cama */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(6.0f, 0.51f, 4.0f));
			box1.setPosition(glm::vec3(-1.0f, -3.0f, -5.0f));
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render();

			/* Cama */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(6.0f, 0.4f, 4.0f));
			box1.setPosition(glm::vec3(-1.0f, -2.59f, -5.0f));
			glBindTexture(GL_TEXTURE_2D, colchon);
			box1.render();

			//cylinder.render();
			shaderPointLight.turnOff();
			glBindTexture(GL_TEXTURE_2D, 0);
		}


		else if (cambioIluminacion == 3)
		{
			shaderSpotLight.turnOn();
			//glUniform3fv(shaderSpotLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(lightModelmatrix3 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f))));
			//glUniform3fv(shaderSpotLight.getUniformLocation("light.direction"), 1, glm::value_ptr(glm::vec3(-0.3f, -1.0f, -0.2f)));
			glUniform3fv(shaderSpotLight.getUniformLocation("light.position"), 1, glm::value_ptr(camera->getPosition()));
			glUniform3fv(shaderSpotLight.getUniformLocation("light.direction"), 1, glm::value_ptr(camera->getFront()));
			glUniform3fv(shaderSpotLight.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
			glUniform3f(shaderSpotLight.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
			glUniform3f(shaderSpotLight.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
			glUniform3f(shaderSpotLight.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
			glUniform3f(shaderSpotLight.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
			glUniform3f(shaderSpotLight.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
			glUniform3f(shaderSpotLight.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
			glUniform1f(shaderSpotLight.getUniformLocation("light.cutOff"), glm::cos(glm::radians(12.5f)));
			glUniform1f(shaderSpotLight.getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(15.0f)));
			glUniform1f(shaderSpotLight.getUniformLocation("light.constant"), 1.0f);
			glUniform1f(shaderSpotLight.getUniformLocation("light.linear"), 0.14f);
			glUniform1f(shaderSpotLight.getUniformLocation("light.quadratics"), 0.07f);
			glUniform1f(shaderSpotLight.getUniformLocation("material.shininess"), 32.0);

			/* Aqui va*/
			glBindTexture(GL_TEXTURE_2D, madera);
			cylinder.setProjectionMatrix(projection);
			cylinder.setViewMatrix(view);
			cylinder.setShader(&shaderSpotLight);
			cylinder.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cubeModelMatrix);
			glBindTexture(GL_TEXTURE_2D, fachada);
			cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cylinder.getSlices() * 3, cubeModelMatrix);
			glBindTexture(GL_TEXTURE_2D, pasto);
			cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3 + cylinder.getSlices() * 3, cylinder.getSlices() * 3, cubeModelMatrix);
			glBindTexture(GL_TEXTURE_2D, 0);


			//cylinder.setShader(&shaderTexture);
			cylinder.setProjectionMatrix(projection);
			cylinder.setViewMatrix(view);
			cylinder.setPosition(glm::vec3(2.0, -3.6, -3.0));
			cylinder.setScale(glm::vec3(0.1, 0.7, 0.1));
			glBindTexture(GL_TEXTURE_2D, madera);
			cylinder.render();

			cylinder.setProjectionMatrix(projection);
			cylinder.setViewMatrix(view);
			cylinder.setPosition(glm::vec3(-4.0, -3.6, 0.0));
			cylinder.setScale(glm::vec3(0.1, 0.7, 0.1));
			glBindTexture(GL_TEXTURE_2D, madera);
			cylinder.render();

			/* Cubo casa */
			box.setProjectionMatrix(projection);
			box.setViewMatrix(view);
			box.setShader(&shaderSpotLight);
			box.setScale(glm::vec3(10.0f, 8.0f, 20.0f));
			box.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));


			/* Fachada*/
			glBindTexture(GL_TEXTURE_2D, fachada);
			box.render(0.0, 6.0);
			/* Pared derecha*/
			glBindTexture(GL_TEXTURE_2D, pared);
			box.render(6.0, 6.0);
			/* Pared izquierda */
			glBindTexture(GL_TEXTURE_2D, pared);
			box.render(18.0, 6.0);
			/*Suelo*/
			glBindTexture(GL_TEXTURE_2D, suelo);
			box.render(30.0, 6.0);
			/*Pared fondo*/
			glBindTexture(GL_TEXTURE_2D, paredFondo);
			box.render(12.0, 6.0);

			/* Jardin */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setShader(&shaderSpotLight);
			box1.setScale(glm::vec3(20.0f, 20.0f, 40.0f));
			box1.setPosition(glm::vec3(0.0f, 5.99f, 0.0f));
			/* Jardin xD*/
			glBindTexture(GL_TEXTURE_2D, pasto);
			box1.render(30.0, 6.0);

			/* Televisión */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(0.2f, 2.0f, 3.0f));
			box1.setPosition(glm::vec3(4.79f, 1.0f, -5.0f));
			glBindTexture(GL_TEXTURE_2D, TV);
			box1.render();

			/* Ropero */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(5.0f, 4.0f, 1.0f));
			box1.setPosition(glm::vec3(2.0f, -2.0f, -9.5f));
			glBindTexture(GL_TEXTURE_2D, vestidor);
			box1.render(0.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, MB);
			box1.render(18.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, MB);
			box1.render(24.0, 6.0);


			/* Buró */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(2.0f, 2.0f, 2.0f));
			box1.setPosition(glm::vec3(-4.0f, -3.0f, 0.0f));
			glBindTexture(GL_TEXTURE_2D, buro);
			box1.render(6.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render(12.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render(0.0, 6.0);
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render(24.0, 6.0);

			/* Mesa */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(1.0f, 5.0f, 5.0f));
			box1.setPosition(glm::vec3(-4.0f, -3.0f, 6.0f));
			glBindTexture(GL_TEXTURE_2D, mesa);
			box1.render(6.0, 6.0);


			/* Cama */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(6.0f, 0.51f, 4.0f));
			box1.setPosition(glm::vec3(-1.0f, -3.0f, -5.0f));
			glBindTexture(GL_TEXTURE_2D, madera);
			box1.render();

			/* Cama */
			box1.setProjectionMatrix(projection);
			box1.setViewMatrix(view);
			box1.setScale(glm::vec3(6.0f, 0.4f, 4.0f));
			box1.setPosition(glm::vec3(-1.0f, -2.59f, -5.0f));
			glBindTexture(GL_TEXTURE_2D, colchon);
			box1.render();
			/* Aquí termina xD*/

			//cylinder.render();
			shaderSpotLight.turnOff();
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.001;

		sphere.setShader(&shaderColor);
		sphere.setColor(glm::vec3(0.4f, 0.3f, 0.6f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.setScale(glm::vec3(1.0f, 1.0f, 1.0f));
		sphere.enableWireMode();
		sphere.render(lightModelmatrix);

		// Se Dibuja el Skybox
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

		shaderCubeTexture.turnOn();
		GLuint cubeTextureId = shaderCubeTexture.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sphere.setShader(&shaderCubeTexture);
		sphere.setProjectionMatrix(projection);
		/* Quitamos la posición de la esfera */
		/*sphere.setViewMatrix(glm::mat4(glm::mat3(view)));*/
		sphere.setViewMatrix(view);
		/* Podemos "salir" del skybox*/
		sphere.setScale(glm::vec3(75.0f, 75.0f, 75.0f));
		sphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);

		shaderCubeTexture.turnOff();

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}