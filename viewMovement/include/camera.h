#pragma once
#include <gui/gl/GL.h>
#include <gui/gl/Camera.h>

class Camera {

	glm::vec3 position;
	float yaw = 90;
	float pitch = 0;
	float tilt = 0; //nije implementirano u getViewMatrix

public:

	Camera(glm::vec3 position = { 0,0,0 }) :
		position(position)
	{
	}

	void move(glm::vec3 translation) {
		this->position += translation;
	}

	void rotate(float turn, float pitch = 0, float tilt = 0) {
		yaw += turn;
		while (yaw > 180 || yaw < -180)
			yaw -= (yaw > 0) ? 360 : -360;
		this->pitch += pitch;
		this->pitch = glm::clamp(this->pitch, -80.0f, 80.0f);
	}

	glm::vec3 getDirection() {
		return  glm::rotate(glm::mat4(1.0f), glm::radians(yaw), { 0,-1, 0 }) * glm::rotate(glm::mat4(1.0f), glm::radians(pitch), { 0,0, 1 }) * glm::vec4(1, 0, 0, 1); //bilo bi dobro rucno izmnoziti ovo
	}

	glm::vec3 getRight()
	{
		return glm::rotate(glm::mat4(1.0f), glm::radians(yaw - 90), { 0,-1, 0 }) * glm::rotate(glm::mat4(1.0f), glm::radians(tilt), { 0,0, 1.0f }) * glm::vec4(1, 0, 0, 1);
	}

	void setPosition(const glm::vec3 &newPos)
	{
		position = newPos;
	}

	glm::mat4 getViewMatrix() {
		return gui::gl::Camera(position[0], position[1], position[2], 0, -1, 0, yaw, pitch).getViewMatrix(); //ovo treba fino napisati
	}

};