#pragma once
#include "./baseObject.h"
#include <gui/Color.h>

class bufferManager {

	GLuint VAO = 0, VBO = 0, EBO = 0, program;
	unsigned int offsetEl = 0, offsetVer = 0, pointsPerVertex = 3;
	GLsizeiptr maxSizeVBO = 0, currentSizeVBO = 0, maxSizeEBO = 0, currentSizeEBO = 0;

	void appendToBuffer(const GLfloat* vertices, unsigned int vertixCnt);
	void appendToBuffer(const GLuint* indexes, unsigned int indexSize);
	void adjustObject(unsigned int vertexCnt, unsigned int elementCnt, object *ptr) {
		ptr->setElementOffset(offsetVer);
		offsetEl += elementCnt;
		offsetVer += vertexCnt/pointsPerVertex;
	}

public:

	bufferManager(GLuint program, const char* positionVec3Name);
	bufferManager(GLuint program, GLuint VAO, GLuint VBO, GLuint EBO, int pointsPerVertex);

	object* createArrow(glm::vec3 startPos, glm::vec3 arrow, float width, float arrowHeadWidth, td::ColorID color, unsigned int basePointCnt = 10);
	object* createCustom(GLfloat* vertices, unsigned int vertixCnt, GLuint *indexes, unsigned int indexSize, glm::vec3 position, td::ColorID color);

	~bufferManager();

};