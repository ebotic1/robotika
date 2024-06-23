#include "baseObject.h"

object::object(glm::mat4 MVP, GLuint program, GLuint VAO, GLuint offset, GLuint indexCnt, glm::vec4 color, std::function<void(object*)> update):
	baseObject(MVP),
	update(update),
	offset(offset),
	indexCnt(indexCnt),
	VAO(VAO),
	program(program),
	color(color)
{
}

object::object(glm::mat4 MVP, GLuint program, GLuint VAO, GLuint offset, GLuint indexCnt, td::ColorID boja, std::function<void(object*)> update):
	baseObject(MVP),
	update(update),
	offset(offset),
	indexCnt(indexCnt),
	VAO(VAO),
	program(program)
{
	gui::Color::getRGBA(boja, color[0], color[1], color[2], color[3]);
}

object::object(glm::vec3 pos, GLuint program, GLuint VAO, GLuint offset, GLuint indexCnt, glm::vec4 color, std::function<void(object*)> update):
	update(update),
	offset(offset),
	indexCnt(indexCnt),
	VAO(VAO),
	program(program),
	color(color)
{
	MVP[3] = glm::vec4(pos, 1.0f);
}

object::object(glm::vec3 pos, GLuint program, GLuint VAO, GLuint offset, GLuint indexCnt, td::ColorID boja, std::function<void(object*)> update):
	update(update),
	offset(offset),
	indexCnt(indexCnt),
	VAO(VAO),
	program(program)
{
	MVP[3] = glm::vec4(pos, 1.0f);
	gui::Color::getRGBA(boja, color[0], color[1], color[2], color[3]);
}
