//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"
#include "Headers/Shader.h"
// Geometry primitives
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"

Sphere sphere(20, 20);
Cylinder cylinder(20, 20, 0.5, 0.5);
Cylinder cylinder2(20, 20, 0.5, 0.5);
Box box;

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Shader shader;

int screenWidth;
int screenHeight;

GLFWwindow * window;

bool exitApp = false;
int lastMousePosX;
int lastMousePosY;

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
	cylinder2.setColor(glm::vec3(0.8, 0.3, 1.0));

	box.init();
	box.setShader(&shader);
	box.setColor(glm::vec3(0.2, 0.8, 0.4));

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
	deltaTime = 1 / TimeManager::Instance().CalculateFrameRate(false);
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
		//matriz de proyeccion en perpectiva
		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / screenWidth, 0.01f, 100.0f);
		/* Matriz de vista */
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8.0f));

		/*Aquí iba lo que quitee xd */

		/* Matriz dell cilindro del torso */
		glm::mat4 matrix0 = glm::mat4(1.0f);
		/* Se coloca el torse en la coordenada (0,0,-1.0) */
		matrix0 = glm::translate(matrix0, glm::vec3(0.0f, 0.0f, -1.0f));
		/* Matriz de la esfera 1, se coloca -0.7 unidades en el eje y debajo del torso */
		glm::mat4 matrixs1 = glm::translate(matrix0, glm::vec3(0.0f, -0.7f, 0.0f));

		/* Matriz de la esfera 8, se coloca 0.7 unidades en el eje y encima del torso */
		glm::mat4 matrixs8 = glm::translate(matrix0, glm::vec3(0.0f, 0.7f, 0.0f));


		glm::mat4 matrix9 = glm::rotate(matrixs8, -0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix9 = glm::translate(matrix9, glm::vec3(0.0, 0.35, 0.0));


		/* Dinujamos cilindro matrix0*/
		matrix0 = glm::scale(matrix0, glm::vec3(0.7f, 1.3f, 0.6f)); /* Tamaño del cilindro */
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix0);



		/* Esferas torso superior */
		glm::mat4 matrixs10 = glm::translate(matrixs8, glm::vec3(0.35f, 0.0f, 0.0f));
		glm::mat4 matrixs9 = glm::translate(matrixs8, glm::vec3(-0.35f, 0.0f, 0.0f));
		/* Esferas torso inferior */
		glm::mat4 matrixs3 = glm::translate(matrixs1, glm::vec3(+0.23f, 0.0f, 0.0f));
		glm::mat4 matrixs2 = glm::translate(matrixs1, glm::vec3(-0.23f, 0.0f, 0.0f));
		/* Dibujamos la esfera central que está debajo del torso */


		matrixs1 = glm::scale(matrixs1, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs1);

		/* Cilindro del brazo derecho */
		glm::mat4 matrix5 = glm::rotate(matrixs10, -0.2f, glm::vec3(0.0f, 0.0f, 3.0f));

		/* Cilindro del brazo izquierdo */
		glm::mat4 matrix7 = glm::rotate(matrixs9, -0.2f, glm::vec3(0.0f, 0.0f, -1.0f));

		/*Declaramos la esfera que va entre matrix5 y matrix6 */
		glm::mat4 matrixs12 = glm::translate(matrix5, glm::vec3(0.75f, 0.0f, 0.0f));

		/*Declaramos la esfera que va entre matrix7 y matrix8 */
		glm::mat4 matrixs11 = glm::translate(matrix7, glm::vec3(-0.75f, 0.0f, 0.0f));



		/* Declaramos el antebrazo izquierdo */
		glm::mat4 matrix6 = glm::rotate(matrixs12, -0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix6 = glm::translate(matrix6, glm::vec3(0.4, 0.0, 0.0));

		/* Declaramos el antebrazo izquierdo */
		glm::mat4 matrix8 = glm::rotate(matrixs11, -0.2f, glm::vec3(0.0f, 0.0f, 0.5f));
		matrix8 = glm::translate(matrix8, glm::vec3(-0.4, 0.0, 0.0));


		/* Declaramos la esfera de la punta superior del pie izquierdo*/
		glm::mat4 matrixs13 = glm::translate(matrix8, glm::vec3(-0.35f, 0.0f, 0.0f));
		/* Declaramos la esfera de la punta superior de la pie derecha*/
		glm::mat4 matrixs14 = glm::translate(matrix6, glm::vec3(+0.35f, -0.0f, 0.0f));


		/* Dibujamos la esfera matrixs12 */
		matrixs12 = glm::scale(matrixs12, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs12);

		matrix5 = glm::translate(matrix5, glm::vec3(0.37f, 0.0f, 0.0f));
		matrix5 = glm::scale(matrix5, glm::vec3(0.75, 0.15, 0.15));


		/* Dibujamos cilindro del brazo derecho */
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		//cylinder.enableWireMode();
		cylinder.render(matrix5);

	


		
		matrix6 = glm::scale(matrix6, glm::vec3(0.75, 0.12, 0.15));
		/* Dibujamos cilindro del brazo derecho */
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix6); 





		/* Dibujamos la esfera matrixs11 */ 
		matrixs11 = glm::scale(matrixs11, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs11);

		matrix7 = glm::translate(matrix7, glm::vec3(-0.37f, 0.0f, 0.0f));
		matrix7 = glm::scale(matrix7, glm::vec3(0.75, 0.15, 0.15));


		/* Dibujamos cilindro del brazo izquierdo */
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix7);

		matrix8 = glm::scale(matrix8, glm::vec3(0.75, 0.12, 0.15));
		/* Dibujamos cilindro del brazo derecho */
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix8);
		
/*********************************************************************************/

		/* Declaramos cilindro de la pierna izquierda */
		glm::mat4 matrix1 = glm::rotate(matrixs2, -0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix1 = glm::translate(matrix1, glm::vec3(0.0, -0.4, 0.0));
		/* Declaramos la matriz de la pierna derecha*/
		glm::mat4 matrix3 = glm::rotate(matrixs3, -0.2f, glm::vec3(0.0f, 0.0f, -1.0f));
		matrix3 = glm::translate(matrix3, glm::vec3(0.0, -0.4, 0.0));

		/*Declaramos la esfera que va entre matrix1 y matrix2 */
		glm::mat4 matrixs4 = glm::translate(matrix1, glm::vec3(0.0f, -0.4f, 0.0f));
		/* Declaramos la esfera que va entre matrix3 y matrix4*/
		glm::mat4 matrixs6 = glm::translate(matrix3, glm::vec3(+0.0f, -0.4f, 0.0f));




		/* Declaramos la matriz de la parte baja de la pierna izquierda*/
		glm::mat4 matrix2 = glm::rotate(matrixs4, -0.2f, glm::vec3(0.0f, 0.0f, -1.0f));
		matrix2 = glm::translate(matrix2, glm::vec3(0.0, -0.4, 0.0));

		/* Declaramos la matriz de la parte baja de la pierna derecha*/
		glm::mat4 matrix4 = glm::rotate(matrixs6, -0.2f, glm::vec3(0.0f, 0.0f, 1.0f));
		matrix4 = glm::translate(matrix4, glm::vec3(0.0, -0.4, 0.0));

		/* Declaramos la esfera de la punta inferior de la pierna izquierda*/
		glm::mat4 matrixs5 = glm::translate(matrix2, glm::vec3(+0.0f, -0.35f, 0.0f));
		/* Declaramos la esfera de la punta inferior de la pierna derecha*/
		glm::mat4 matrixs7 = glm::translate(matrix4, glm::vec3(+0.0f, -0.35f, 0.0f));


		matrix2 = glm::scale(matrix2, glm::vec3(0.1f, 0.6f, 0.1f));
		/* Dibujamos el cilindro matrix2 */
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix2);


		matrix4 = glm::scale(matrix4, glm::vec3(0.1f, 0.6f, 0.1f));
		/* Dibujamos el cilindro matrix4 */
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix4);

		/*Dibujamos la matriz de la cabeza */
		matrix9 = glm::scale(matrix9, glm::vec3(0.4f, 0.7f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix9);

		/* Dibujamos la esfera matrixs5*/
		matrixs5 = glm::scale(matrixs5, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs5);

		/* Dibujamos la esfera matrixs7*/
		matrixs7 = glm::scale(matrixs7, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs7);

		/* Dibujamos la esfera matrixs13*/
		matrixs13 = glm::scale(matrixs13, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs13);

		/* Dibujamos la esfera matrixs14*/
		matrixs14 = glm::scale(matrixs14, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs14);

		/* Dibujamos la esfera matrixs4*/
		matrixs4 = glm::scale(matrixs4, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs4);


		/* Dibujamos esfera matrix s6 (Rodilla derecha) */
		matrixs6 = glm::scale(matrixs6, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs6);


		/*Dibujamos la matriz de la pierna izquierda */
		matrix1 = glm::scale(matrix1, glm::vec3(0.18f, 0.7f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix1);

	


		/*Dibujamos la matriz de la pierna derecha */
		matrix3 = glm::scale(matrix3, glm::vec3(0.18f, 0.7f, 0.15f));
		cylinder.setProjectionMatrix(projection);
		cylinder.setViewMatrix(view);
		cylinder.enableWireMode();
		cylinder.render(matrix3);

		/* Dibujamos esfera inferior izquierda del torso*/
		matrixs2 = glm::scale(matrixs2, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs2);

		/* Dibujamos esfera inferior derecha del torso */
		matrixs3 = glm::scale(matrixs3, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs3);

		/* Dibujamos esfera superior izquierda del torso*/
		matrixs9 = glm::scale(matrixs9, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs9);

		/* Dibujamos esfera superior derecha del torso */
		matrixs10 = glm::scale(matrixs10, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs10);
		
		/* Dibujamos esfera central superior del torso*/
		matrixs8 = glm::scale(matrixs8, glm::vec3(0.1f, 0.1f, 0.1f));
		sphere.setProjectionMatrix(projection);
		sphere.setViewMatrix(view);
		sphere.enableWireMode();
		sphere.render(matrixs8);




		
		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}