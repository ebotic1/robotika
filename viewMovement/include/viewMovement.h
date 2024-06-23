#pragma once
#include <gui/gl/View.h>
#include <chrono>
#include "./../include/PlayerMove.h"

class ViewMove : public gui::gl::View {

protected:

	float timeStep = 0;

private:

	std::chrono::steady_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

	std::set<int> keysPressed;
	PlayerMove* movement = nullptr;
	gui::Point lastPoint;

	void onDraw(const gui::Rect& rect) override {
		timeStep = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastTime).count();
		timeStep = (timeStep > 70) ? 70 : timeStep;
		movement->move(keysPressed, timeStep);
		OnDraw(rect);
		lastTime = std::chrono::high_resolution_clock::now();
	}

public:

	virtual bool prepareNextFrame() override {
		return true;
	}

	ViewMove() {
	}

	bool onKeyPressed(const gui::Key& key) override
	{
		if(key.getCode() != 16777215)
			keysPressed.insert(key.getCode());
		return false;
	}

	bool onKeyReleased(const gui::Key& key) override
	{
		keysPressed.erase(key.getCode());
		return false;
	}

	void onCursorMoved(const gui::InputDevice& inputDevice)
	{
		moveCursorToCenter();
		if (movement != nullptr)
			movement->mouseMove({ lastPoint.x - inputDevice.getFramePoint().x, inputDevice.getFramePoint().y - lastPoint.y });
	}

	void onResize(const gui::Size& newSize) override 
	{
		lastPoint = { newSize.width / 2, newSize.height / 2 };
		showCursor(false);
	}

	void onCursorEntered(const gui::InputDevice& inputDevice) override
	{
		//lastPoint = inputDevice.getFramePoint();
	}

	void onCursorExited(const gui::InputDevice& inputDevice) override
	{
		onCursorMoved(inputDevice);
	}

	glm::mat4 getMVPMatrix() {
		return movement->getMatrix();
	}

	void setMovement(PlayerMove* mov) {
		movement = mov;
	}


	virtual void OnDraw(const gui::Rect& rect) = 0;

	virtual ~ViewMove(){}

};