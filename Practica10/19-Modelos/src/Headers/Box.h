#ifndef BOX_H
#define BOX_H
#include "Abstractmodel.h"

class Box : public AbstractModel
{
public:
	Box();
	~Box();
	void setUVS(std::vector<glm::vec2> uvs);
	virtual bool rayPicking(glm::vec3 init, glm::vec3 end, glm::vec3 &intersection);
	AABB aabb;
};

#endif // BOX_H