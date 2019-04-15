/*
 * Practica 9
 * 12 abril 2019
 * Skybox
*/
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
Sphere sphere2(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Box box;

Shader shaderColor;
Shader shaderTexture;
Shader shaderCubeTexture;
Shader shaderMateriales;
Shader shaderDirectionLight; /* Imagen 2 */
Shader shaderPointLight;
Shader shaderSpotLight;

GLuint textureID1, textureID2, textureID3, textureCubeTexture;
GLuint cubeTextureID;

/* Planos que definen el cubo */
GLenum types[6] = {
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

/* Ubicación del archivo */
std::string fileNames[6] = { 
	"../../Textures/mp_met/met_ft.tga",
	"../../Textures/mp_met/met_bk.tga",
	"../../Textures/mp_met/met_up.tga",
	"../../Textures/mp_met/met_dn.tga",
	"../../Textures/mp_met/met_rt.tga",
	"../../Textures/mp_met/met_lf.tga"
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
	/* Ruta que renderiza el skybox */
	shaderCubeTexture.initialize("../../Shaders/cubeTexture.vs", "../../Shaders/cubeTexture.fs");
	shaderMateriales.initialize("../../Shaders/iluminacion_materiales_res.vs", "../../Shaders/iluminacion_materiales_res.fs");
	shaderDirectionLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/directionalLight.fs");
	shaderPointLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/pointLight.fs");
	shaderSpotLight.initialize("../../Shaders/typeLight.vs", "../../Shaders/spotLight.fs");

	sphere.init();
	sphere2.init();
	cylinder.init();
	cylinder2.init();
	box.init();

	camera->setPosition(glm::vec3(0.0f, 0.0f, 0.4f));

	// Textura Ladrillos
	int imageWidth, imageHeight;
	Texture texture("../../Textures/texturaLadrillos.jpg");
	FIBITMAP* bitmap = texture.loadImage(false);
	unsigned char * data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID1);
	glBindTexture(GL_TEXTURE_2D, textureID1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data){
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	texture.freeImage(bitmap);

	// Texture Goku
	texture = Texture("../../Textures/goku.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID2);
	glBindTexture(GL_TEXTURE_2D, textureID2);
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

	// Textura cuadritos
	texture = Texture("../../Textures/test.png");
	bitmap = texture.loadImage(false);
	data = texture.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureID3);
	glBindTexture(GL_TEXTURE_2D, textureID3);
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
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
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
	float ratio = 20.0;

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

		glBindTexture(GL_TEXTURE_2D, textureID2);
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.setShader(&shaderTexture);
		cylinder.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cubeModelMatrix);
		glBindTexture(GL_TEXTURE_2D, textureID1);
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cylinder.getSlices() * 3, cubeModelMatrix);
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3 + cylinder.getSlices() * 3, cylinder.getSlices() * 3, cubeModelMatrix);
		glBindTexture(GL_TEXTURE_2D, 0);


		cylinder2.setProjectionMatrix(projection);
		cylinder2.setViewMatrix(view);
		cylinder2.setShader(&shaderTexture);
		cylinder2.render(0, cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cubeModelMatrix);
		glBindTexture(GL_TEXTURE_2D, textureID1);
		cylinder2.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3, cylinder.getSlices() * 3, cubeModelMatrix);
		glBindTexture(GL_TEXTURE_2D, textureID3);
		cylinder2.render(cylinder.getSlices() * cylinder.getStacks() * 2 * 3 + cylinder.getSlices() * 3, cylinder.getSlices() * 3, cubeModelMatrix);
		glBindTexture(GL_TEXTURE_2D, 0);


		glBindTexture(GL_TEXTURE_2D, textureID2);
		sphere2.setShader(&shaderSpotLight);
		sphere2.setProjectionMatrix(projection);
		sphere2.setViewMatrix(view);
		sphere2.setPosition(glm::vec3(0.0, 0.0, 0.0));
		sphere2.setScale(glm::vec3(1.0, 1.0, 1.0));

		// Iluminación
		glm::mat4 lightModelmatrix = glm::rotate(cubeModelMatrix, angle, glm::vec3(0.0f, 1.0f, 0.0f));
		lightModelmatrix = glm::translate(lightModelmatrix, glm::vec3(0.0f, 0.0f, -ratio));
		shaderSpotLight.turnOn();
		/* Posición de la luz */
		//glUniform3fv(shaderSpotLight.getUniformLocation("light.position"), 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		//glUniform3fv(shaderSpotLight.getUniformLocation("light.direction"), 1, glm::value_ptr(glm::vec3(-0.3f, -1.0f, -0.2f)));
		glUniform3fv(shaderSpotLight.getUniformLocation("light.position"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3fv(shaderSpotLight.getUniformLocation("light.direction"),1, glm::value_ptr(camera->getFront()));
		glUniform3fv(shaderSpotLight.getUniformLocation("viewPos"), 1, glm::value_ptr(camera->getPosition()));
		glUniform3f(shaderSpotLight.getUniformLocation("light.ambient"), 0.2, 0.2, 0.2);
		glUniform3f(shaderSpotLight.getUniformLocation("light.diffuse"), 0.2, 0.3, 0.6);
		glUniform3f(shaderSpotLight.getUniformLocation("light.specular"), 0.5, 0.3, 0.2);
		glUniform3f(shaderSpotLight.getUniformLocation("material.ambient"), 1.0, 0.2, 0.6);
		glUniform3f(shaderSpotLight.getUniformLocation("material.diffuse"), 0.4, 0.5, 0.8);
		glUniform3f(shaderSpotLight.getUniformLocation("material.specular"), 0.5, 0.3, 0.2);
		/* Pära spotlight */
		glUniform1f(shaderSpotLight.getUniformLocation("light.cutOff"), glm::cos(glm::radians(5.0f)));
		glUniform1f(shaderSpotLight.getUniformLocation("light.outerCutOff"), glm::cos(glm::radians(7.0f)));
		/* Para spotlight */
		glUniform1f(shaderSpotLight.getUniformLocation("light.constant"), 1.0f);
		glUniform1f(shaderSpotLight.getUniformLocation("light.linear"), 0.7f);
		glUniform1f(shaderSpotLight.getUniformLocation("light.quadratics"), 1.8f);
		glUniform1f(shaderSpotLight.getUniformLocation("material.shininess"), 32.0);

		sphere2.render();
		sphere2.setPosition(glm::vec3(0.0, 0.0, -10.0));
		sphere2.render();

		cylinder2.setShader(&shaderSpotLight);
		cylinder2.setProjectionMatrix(projection);
		cylinder2.setViewMatrix(view);
		cylinder2.setPosition(glm::vec3(2.0, 0.0, 0.0));
		cylinder2.render();

		shaderSpotLight.turnOff();
		glBindTexture(GL_TEXTURE_2D, 0);

		if (angle > 2 * M_PI)
			angle = 0.0;
		else
			angle += 0.001;
		
		/* Renderizado*/

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
		/* Prueba de las caras ocultas. Bandera que descarta la geometría si no cumple la regla de la mano derecha */
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderCubeTexture.turnOn();
		GLuint cubeTextureId = shaderCubeTexture.getUniformLocation("skybox");
		glUniform1f(cubeTextureId, 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		sphere.setShader(&shaderCubeTexture);
		sphere.setProjectionMatrix(projection);
		/* Quitamos la posición de la esfera*/
		/* sphere.setViewMatrix(glm::mat4(glm::mat3(view)));*/
		sphere.setViewMatrix(view); /* Visualizamos de forma externa la esfera */
		sphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));
		/* Dibujado de la esfera*/
		sphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		shaderCubeTexture.turnOff();

		/* Renderizado*/

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}