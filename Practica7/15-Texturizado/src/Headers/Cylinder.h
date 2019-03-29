#ifndef CYLINDER_H
#define CYLINDER_H
#define _USE_MATH_DEFINES
#include <cmath>
#include "Abstractmodel.h"

class Cylinder : public AbstractModel
{
public:
	Cylinder(int slices, int stacks, float topRadius = 1.0, float bottomRadius = 1.0, float height = 1.0);
	~Cylinder();
	void render(int indexInit, int indexSize, glm::mat4 parentTrans = glm::mat4(1.0f));
	int getSlices() {
		return this->slices;
	}

	int getStacks() {
		return this->stacks;
	}

	virtual bool rayPicking(glm::vec3 init, glm::vec3 end, glm::vec3 &intersection);

private:
	int slices;
	int stacks;
};

#endif // CYLINDER_H