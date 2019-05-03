#ifndef QUEUETRANSFORMATION_H
#define QUEUETRANSFORMATION_H

#include <iostream>
#include <deque>
#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/glew.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class QueueTransformation {
public:
	QueueTransformation();
	void translatef(float x, float y, float z);
	void rotatef(float angle, float x, float y, float z);
	void scalef(float x, float y, float z);
	void pushMatrix();
	void popMatrix();
	glm::mat4 getTopMatrix();
private:
	std::deque<glm::mat4> queue;
};

#endif // QUEUETRANSFORMATION_H