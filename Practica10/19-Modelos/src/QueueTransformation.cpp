#include "Headers/QueueTransformation.h"

QueueTransformation::QueueTransformation() {
	queue.push_back(glm::mat4(1.0f));
}

void QueueTransformation::translatef(float x, float y, float z) {
	queue.back() = queue.back() * glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void QueueTransformation::rotatef(float angle, float x, float y, float z) {
	queue.back() = queue.back() * glm::rotate(glm::mat4(1.0f), angle, glm::vec3(x, y, z));
}

void QueueTransformation::scalef(float x, float y, float z) {
	queue.back() = queue.back() * glm::scale(glm::mat4(1.0f), glm::vec3(x, y, z));
}

void QueueTransformation::pushMatrix() {
	queue.push_back(queue.back());
}

void QueueTransformation::popMatrix() {
	queue.pop_back();
}

glm::mat4 QueueTransformation::getTopMatrix() {
	return queue.back();
}