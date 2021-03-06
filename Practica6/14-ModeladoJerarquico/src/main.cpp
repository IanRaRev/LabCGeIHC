//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

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

//Camera include 
#include "Headers/FirstPersonCamera.h"

Sphere sphere(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Box box;

Shader shader;

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());
//Equivalente a FirstPersonCamera * camera = new FirstPersonCamera(); Tenemos que liberar memoria manualmente

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
/* offsetx y offset y de la cosecha*/
int lastMousePosX, offsetx;
int lastMousePosY, offsety;

/* Rotaci�n del hombro izquierdo*/
float rot1 = 0.0;
float rot2 = 0.0;
float rot3 = 0.0;


/* Rotaci�n del hombro izquierdo*/
float rot4 = 0.0;
float rot5 = 0.0;
float rot6 = 0.0;


/* Rotaci�n de la mu�eca izquierda*/
float rot7 = 0.0;
float rot8 = 0.0;
float rot9 = 0.0;


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

	shader.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");

	sphere.init();
	sphere.setShader(&shader);
	sphere.setColor(glm::vec3(0.3, 0.3, 1.0));

	cylinder.init();
	cylinder.setShader(&shader);
	cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));

	cylinder2.init();
	cylinder2.setShader(&shader);
	cylinder2.setColor(glm::vec3(0.2, 0.7, 0.3));

	box.init();
	box.setShader(&shader);
	box.setColor(glm::vec3(0.2, 0.8, 0.4));

	//camera->setSensitivity(1.0);
}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	shader.destroy();
	sphere.destroy();
	cylinder.destroy();
	cylinder2.destroy();
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

	/* Desplazamiento del mouse*/
	offsetx = xpos - lastMousePosX;
	offsety = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
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
	//
	TimeManager::Instance().CalculateFrameRate(false);
	deltaTime = TimeManager::Instance().DeltaTime;

	//De la cosecha
	/* Tecla W acerca*/
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	/* Tecla S aleja*/
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	/* Tecla D mueve hacia la derecha*/
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	/* Tecla A mueve a la izquierda*/
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);

	/* Al presionar tecla mouse izquierdo, se mueve*/
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetx, offsety, deltaTime);

	/* Rotaci�n del hombro izquierdo */
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		rot1 += 0.01;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		rot2 += 0.01;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		rot3 += 0.01;

	/* Rotaci�n del codo izquierdo */
	if(glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
		rot4 += 0.01;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
		rot5 += 0.01;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
		rot6 += 0.01;

	/* Rotaci�n de la mu�eca izquierda */
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
		rot7 += 0.01;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
		rot8 += 0.01;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS)
		rot9 += 0.01;
	offsetx = 0;
	offsety = 0;

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;
	double lastTime = TimeManager::Instance().GetTime();

	while (psi) {
		psi = processInput(true);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		// Matrix de proyeccion en perspectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		// matrix de vista
		/* La camara se acerca */
		glm::mat4 view = camera->getViewMatrix(); 


		//glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8.0f));

		// Matriz del Cilindro del torso
		glm::mat4 matrix0 = glm::mat4(1.0f);
		// Se coloca el torso en la coordenada (0, 0, -1.0)
		matrix0 = glm::translate(matrix0, glm::vec3(0.0f, 0.0f, -1.0f));
		// Matrix de la esfera 1, se coloca -0.5 unidades en el eje y debajo del torso
		glm::mat4 matrixs1 = glm::translate(matrix0, glm::vec3(0.0f, -0.5f, 0.0f));
		// Se escala el cylidro del torso

		glm::mat4 matrixs5 = glm::translate(matrix0, glm::vec3(0.0f, 0.5f, 0.0f));

	

		/* Esfera del hombro izquierdo */
		glm::mat4 matrixs6 = glm::translate(matrixs5, glm::vec3(0.3f, 0.0f, 0.0f));
		matrixs6 = glm::rotate(matrixs6, rot1, glm::vec3(0.0f, 0.0f, 1.0f));
		matrixs6 = glm::rotate(matrixs6, rot2, glm::vec3(0.0f, 1.0f, 0.0f));
		matrixs6 = glm::rotate(matrixs6, rot3, glm::vec3(1.0f, 0.0f, 0.0f));

		/* Cilindro del hombro al codo izquierdo */
		glm::mat4 matrix7 = glm::translate(matrixs6, glm::vec3(0.25f, 0.0f, 0.0f));

		/* Esfera del codo izquierdo */
		glm::mat4 matrixs7 = glm::translate(matrix7, glm::vec3(0.25f, 0.0f, 0.0f));
		matrixs7 = glm::rotate(matrixs7, rot4, glm::vec3(0.0f, 0.0f, 1.0f));
		matrixs7 = glm::rotate(matrixs7, rot5, glm::vec3(0.0f, 1.0f, 0.0f));
		matrixs7 = glm::rotate(matrixs7, rot6, glm::vec3(1.0f, 0.0f, 0.0f));

		/* Cilindro del codo a la mu�eca izquierda*/
		glm::mat4 matrix8 = glm::translate(matrixs7, glm::vec3(0.25f, 0.0f, 0.0f));

		/* Esfera de la mu�eca */
		glm::mat4 matrixs8 = glm::translate(matrix8, glm::vec3(0.25f, 0.0f, 0.0f));
		matrixs8 = glm::rotate(matrixs8, rot7, glm::vec3(0.0f, 0.0f, 1.0f));
		matrixs8 = glm::rotate(matrixs8, rot8, glm::vec3(0.0f, 1.0f, 0.0f));
		matrixs8 = glm::rotate(matrixs8, rot9, glm::vec3(1.0f, 0.0f, 0.0f));

		matrix8 = glm::rotate(matrix8, 1.5708f, glm::vec3(0.0f, 0.0f, 0.1f));
		matrix8 = glm::scale(matrix8, glm::vec3(0.1f, 0.4f, 0.1f));


		/* Cilindro de la mano izquierda*/
		glm::mat4 matrix9 = glm::translate(matrixs8, glm::vec3(0.15f, 0.0f, 0.0f));
		matrix9 = glm::rotate(matrix9, 1.5708f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix9 = glm::scale(matrix9, glm::vec3(0.2f, 0.2f, 0.2f));


		box.setProjectionMatrix(projection);
		box.setViewMatrix(view);
		//box.setPosition(glm::vec3(0.0f, 0.0f, 1.0f));
		box.enableWireMode();
		box.render(matrix9);

		/* Dibujo de la mu�eca izquierda */
		matrixs8 = glm::scale(matrixs8, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs8); 

		/* Cilindro del codo a la mu�eca*/
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		cylinder.render(matrix8);

		/* Dibujo del codo izquierdo*/
		matrixs7 = glm::scale(matrixs7, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs7);

		/* Cilindro del hombro al codo izquierdo*/
		matrix7 = glm::rotate(matrix7, 1.5708f, glm::vec3(0.0f, 0.0f, 0.1f));
		matrix7 = glm::scale(matrix7, glm::vec3(0.15f, 0.5f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		cylinder.render(matrix7);

		/* Esfera del hombro */
		matrixs6 = glm::scale(matrixs6, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs6);

		matrixs5 = glm::scale(matrixs5, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		sphere.render(matrixs5);

		matrix0 = glm::scale(matrix0, glm::vec3(0.6f, 1.0f, 0.6f));
		/* Cilindro del torso del cuerpo */
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		cylinder.render(matrix0);

		glm::mat4 matrixs2 = glm::translate(matrixs1, glm::vec3(-0.225f, 0.0f, 0.0f));
		glm::mat4 matrixs3 = glm::translate(matrixs1, glm::vec3(0.225f, 0.0f, 0.0f));
		matrixs1 = glm::scale(matrixs1, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs1);

		glm::mat4 matrix1 = glm::rotate(matrixs2, -0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix1 = glm::translate(matrix1, glm::vec3(0.0, -0.4, 0.0));

		glm::mat4 matrixs4 = glm::translate(matrix1, glm::vec3(0.0f, -0.4f, 0.0f));

		glm::mat4 matrix2 = glm::rotate(matrixs4, 0.3f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix2 = glm::translate(matrix2, glm::vec3(0.0f, -0.3f, 0.0f));
		matrix2 = glm::scale(matrix2, glm::vec3(0.1, 0.6, 0.1));
		cylinder2.setProjectionMatrix(projection);
		cylinder2.setViewMatrix(view);
		cylinder2.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		cylinder2.render(matrix2);

		matrixs4 = glm::scale(matrixs4, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		cylinder.setColor(glm::vec3(0.8, 0.3, 1.0));
		sphere.render(matrixs4);

		matrix1 = glm::scale(matrix1, glm::vec3(0.15f, 0.8f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.setColor(glm::vec3(0.1, 0.2, 0.4));
		cylinder.render(matrix1);

		matrixs2 = glm::scale(matrixs2, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs2);

		matrixs3 = glm::scale(matrixs3, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs3);


		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}