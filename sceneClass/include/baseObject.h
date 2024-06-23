#pragma once
#include <vector>
#include <glm/mat4x4.hpp>
#include <functional>
#include <gui/gl/Buffer.h>
#include <stack>
#include <gui/Color.h>

struct DrawElementsIndirectCommand
{
	GLuint count;
	GLuint instanceCount;
	GLuint firstIndex;
	GLint baseVertex;
	GLuint baseInstance;
};

class object;

class baseObject {
	friend class rootObject;


protected:
	glm::mat4 MVP;
	std::vector<object*>children;
	bool updateMVP = true;
	bool updateVertices = true;
	unsigned int ID;

public:
	baseObject(glm::mat4 MVP = glm::mat4(1.0f), unsigned int ID = 0);

	class Iterator {
		object* ptr;
		std::stack<object*> pointerStack;
	public:
		Iterator(baseObject* ptr);
	public:
		Iterator(object* ptr);
		object& operator*() const { return *ptr; }
		Iterator& operator++();
		bool operator!=(const Iterator& other) const { return ptr != other.ptr; }
		bool operator==(const Iterator& other) const { return ptr == other.ptr; }
		Iterator& discardBranch();
	};
	
	Iterator begin();
	Iterator end();




	virtual void onIterate();

	const glm::mat4& getMVP() const{
		return MVP;
	}

	glm::mat4& setMVP() {
		updateMVP = true;
		return MVP;
	}



	virtual ~baseObject() 
	{
		auto ptr = children.begin();
		while (ptr != children.end()) {
			delete* ptr;
			++ptr;
		}
	}
	

protected:

};



class object : public baseObject
{
	friend class rootObject;
	baseObject* parent;
	GLuint offset, indexCnt, baseVertex = 0;
	GLuint program, VAO;

	glm::vec4 color;
	bool updateColor = true;

public:
	object(glm::mat4 MVP, GLuint program, GLuint VAO, GLuint offset, GLuint indexCnt, glm::vec4 color, std::function<void(object*)> update = 0);
	object(glm::mat4 MVP, GLuint program, GLuint VAO, GLuint offset, GLuint indexCnt, td::ColorID color, std::function<void(object*)> update = 0);
	object(glm::vec3 pos, GLuint program, GLuint VAO, GLuint offset, GLuint indexCnt, glm::vec4 color, std::function<void(object*)> update = 0);
	object(glm::vec3 pos, GLuint program, GLuint VAO, GLuint offset, GLuint indexCnt, td::ColorID color, std::function<void(object*)> update = 0);



	void setColor(td::ColorID boja) {
		updateColor = true;
		gui::Color::getRGBA(boja, color[0], color[1], color[2], color[3]);
	}

	void setColor(glm::vec4 boja) {
		updateColor = true;
		color = boja;
	}

	const glm::vec4& getColor() {
		return color;
	}

	void setVertices(GLuint offset, GLuint indexCnt) {
		updateVertices = true;
		this->offset = offset;
		this->indexCnt = indexCnt;
	}

	void setElementOffset(GLuint baseVertex) {
		updateVertices = true;
		this->baseVertex = baseVertex;
	}

	std::function<void(object *)> update;
	Iterator begin() { return Iterator(this); }

protected:
	virtual void onIterate() {
		if (update)
			update(this);
	}

	 void setUpCommand(DrawElementsIndirectCommand& cmnd) {
		cmnd.count = indexCnt;
		cmnd.instanceCount = 1;
		cmnd.firstIndex = offset;
		cmnd.baseVertex = baseVertex;
		cmnd.baseInstance = ID;
	}

	 void setUpCommandArray(DrawElementsIndirectCommand& cmnd) {
		cmnd.count = indexCnt;
		cmnd.instanceCount = 1;
		cmnd.firstIndex = offset;
		cmnd.baseVertex = ID;
		//cmnd.baseInstance = 1;
	}

};


