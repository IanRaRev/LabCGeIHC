//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

//Para incluirlos agregamos esta linea
#include"Headers/Shader.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


GLuint VBO, VAO, EBO;

//Instancia de shader
Shader shader;

struct Vertex {
	glm::vec3 m_Pos;
	glm::vec3 m_Color;
};

// This is for the render with index element
Vertex vertices[] =
{
	{ glm::vec3(-0.5f, -0.5f, 0.5f) , glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.5f , -0.5f, 0.5f) , glm::vec3(0.0f, 1.0f, 0.0f) },
	{ glm::vec3(0.5f ,  0.5f, 0.5f) , glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(-0.5f,  0.5f, 0.5f) , glm::vec3(1.0f, 0.0f, 1.0f) },
	{ glm::vec3(0.5f , -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f) },
	{ glm::vec3(0.5f ,  0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 1.0f) },
	{ glm::vec3(-0.5f , 0.5f, -0.5f) ,glm::vec3(0.0f, 0.0f, 1.0f) },
	{ glm::vec3(-0.5f , -0.5f, -0.5f),glm::vec3(0.0f, 1.0f, 0.0f) },
};

GLuint indices[] = {  // Note that we start from 0!
	0, 1, 2,
	0, 2, 3,
	1, 4, 5,
	1, 5, 2,
	0, 3, 6,
	0, 6, 7,
	0, 4, 1,
	0, 7, 4,
	3, 2, 5,
	3, 5, 6,
	4, 5, 6,
	4, 6, 7
};

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
void cubo();

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


	//De la cosecha. Prueba de profundidad
	glEnable(GL_DEPTH_TEST);

	//Indicamos la ruta de los shaders
	shader.initialize("../../Shaders/transformaciones.vs", "../../Shaders/transformaciones.fs");


	cubo();
}

void cubo() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	// This is for the render with index element
	glGenBuffers(1, &EBO);
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// This is for the render with index element
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]),
		(GLvoid*)sizeof(vertices[0].m_Pos));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void destroyWindow() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void destroy() {
	destroyWindow();

	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VBO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &EBO);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAO);
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

	float fovy = 0;

	while (psi) {
		psi = processInput(true);
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

		shader.turnOn();

		glBindVertexArray(VAO);

		//De la cosecha. Matriz de proyección

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float) screenWidth/screenHeight, 0.01f, 100.0f);
		GLuint locProj = shader.getUniformLocation("projection");
		glUniformMatrix4fv(locProj, 1, GL_FALSE, glm::value_ptr(projection));

		//Si cambiamos el ultimo parametro, cambiamos la distancia en que vemos el objeto
		glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -8.0));
		GLuint locView = shader.getUniformLocation("view");
		glUniformMatrix4fv(locView, 1, GL_FALSE, glm::value_ptr(view));


		/* P R I M E R  C U B I T O */
		glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0, 4.0, -4.0));	
		GLuint locModel = shader.getUniformLocation("model");
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));

		// This is for the render with index element
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		/* S E G U N D O  C U B I T O */
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 4.0f, -4.0f));	
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		/* S E G U N D O  C U B I T O */
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		/* T E R C E R  C U B I T O */
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 3.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		/* C U A R T O  C U B I T O */
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 2.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		/* Q U I N T O  C U B I T O */
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		/* S E X T O  C U B I T O */
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, 1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		/* S E P T I M O  C U B I T O */
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		/* S E P T I M O  C U B I T O */
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, 1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);


		/*	G	*/
		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 3.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 2.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 2.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.1f, 2.5f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);


		/*	E	*/
		model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 2.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 2.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(5.5f, 2.5f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 3.25f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f, 3.75f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(3.5f, 3.75f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.5f, 3.5f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 3.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.1f, 1.5f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.3f, 1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.75f, 0.75f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(3.6f, 0.6f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, 0.5f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(4.5f, 0.5f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		/*	I	*/
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, -1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -2.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -3.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, -4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-6.0f, -4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-4.0f, -4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);


		/*	H	*/
		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -2.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -3.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, -4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, -2.5f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -3.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);


		/*	C	*/
		model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, -1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -1.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -2.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -3.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, -4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, -4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);

		model = glm::translate(glm::mat4(1.0f), glm::vec3(4.0f, -4.0f, -4.0f));
		glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (GLuint *)0);



		glBindVertexArray(0);

		//De la cosecha

		shader.turnOff();

		glfwSwapBuffers(window);
	}
}

int main(int argc, char ** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}