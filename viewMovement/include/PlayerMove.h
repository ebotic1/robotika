#pragma once
#include "./camera.h"
#include <set>

class PlayerMove {
protected:
	Camera cam;
public:
	PlayerMove(glm::vec3 position = { 0,0,0 }) :
		cam(position)
	{

	}
	virtual void move(const std::set<int>& keyboardPool, float timeStep) = 0;
	virtual void mouseMove(glm::vec2 delta) = 0;
	virtual glm::mat4 getMatrix() {
		return cam.getViewMatrix();
	}
};
