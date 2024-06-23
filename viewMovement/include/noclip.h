#pragma once	
#include "./PlayerMove.h"
#include <set>

class NoclipMove : public PlayerMove
{
	float speed;
public:
	NoclipMove(glm::vec3 position = { 0,0,0 }, float speed = 1) :
		PlayerMove(position),
		speed(speed)
	{

	}

	void move(const std::set<int>& keyboardPool, float timeStep) override
	{
		if (keyboardPool.empty())
			return;
		glm::vec3 direction = cam.getDirection();
		direction *= (keyboardPool.contains('w') ? 1 : 0) - (keyboardPool.contains('s') ? 1 : 0);
		direction += cam.getRight() * ((keyboardPool.contains('d') ? 1.0f : 0.0f) - (keyboardPool.contains('a') ? 1 : 0));

		//up/down kretanje
		//auto up = glm::normalize(glm::cross(cam.getDirection(), cam.getRight()));
		glm::vec3 up = { 0,1,0 };
		direction += up * ((keyboardPool.contains(65507) ? 1.0f : 0.0f) - (keyboardPool.contains(' ') ? 1 : 0));
		if (glm::length(direction) < 0.1)
			return;
		cam.move(glm::normalize(direction) * 0.01f * speed * timeStep);
	}

	void mouseMove(glm::vec2 delta)
	{
		delta *= 0.1;
		cam.rotate(delta.x, delta.y);
		
	}
};

