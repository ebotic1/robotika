#pragma once
#include <gui/gl/View.h>
#include "./../../viewMovement/include/viewMovement.h"
#include "./../../viewMovement/include/noclip.h"
#include "../../common/include/Extrusion.h"

#include <gui/gl/Camera.h>

class ViewGL : public ViewMove
{

	

	GLuint _shaderProgram = 0;

	GLuint _vertexArray = 0;
	GLuint _vertexBuffer = 0;
	GLuint _MVPmat = 0;
	glm::mat4 _perspectiveMatrix;

	GLuint _texture = 0;
	gui::Texture _diceText;


	cnt::PushBackVector<float>vertex;


	NoclipMove _player;

public:
	ViewGL() :
		_diceText(":dice"),
		_player({ 0,0,2 }, 0.5)
	{
		gui::gl::Context reqContext;
		createContext(reqContext, { gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::CursorEnterLeave });

	
	}


	void onInit() override
	{
		_shaderProgram = linkProgram(":shVert", ":shFrag");
		if (_shaderProgram == 0)
			return;

		//gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);
		//gui::gl::Context::enable(gui::gl::Context::Flag::CullFace);
		//setAnimation(true);

		GLfloat kocka [] =
		{
			0.0f, 0.0f,0.0f, 0.0f, 0.0f,
			1.0f, 0.0f,0.0f, 1.0f, 0.0f,
			1.0f, 1.0f,0.0f, 1.0f, 1.0f,
			0.0f, 1.0f,0.0f, 0.0f, 1.0f,
			0.0f, 1.0f,1.0f, 0.0f, 0.0f
		};



		glm::vec2 oblik[] =
		{
			{0.0f,0.0f},
			{1.0f,0.0f},
			{1.0f,1.0f}
		};

		cnt::PushBackVector<td::UINT4>inds;

		Extrusion ext(oblik, 3);
		ext.setDirection({ 0,0,1 });
		ext.setKeepCentered(true);
		ext.setAddMidpoints(true, 20);
		//ext.Extrude({ {0,5,0},{0,0,3},{0,-2,0}, {0,0,8},  {0,-3,0}, {0,0,-1} }, vertex, inds);
		/*
		ext.Extrude({ {0,5,0} }, vertex, inds);
		ext.Extrude({ {0,0,3}}, vertex, inds);
		ext.Extrude({ {0,-2,0}}, vertex, inds);
		ext.Extrude({ {0,0,8}}, vertex, inds);
		ext.Extrude({ {0,-3,0} }, vertex, inds);
		ext.Extrude({ {0,0,-1} }, vertex, inds);
		ext.Extrude({ {0,0,-5} }, vertex, inds);
		ext.Extrude({ {0,0,-5} }, vertex, inds);
		//_vec = ext.Extrude({ {3,0,0} }, vertex, inds); // {1, 0, 5}
		//_vec = ext.Extrude({ {0,0,5}, {5,5,5}, {5,5,5} }, vertex, inds);
		
		
		//ext.Extrude(ext.generateSmoothPoints({ {0,0,0}, {6,-15,6}, {0, 5, 4}, {0,7,7},  {0,7,7},  {0,7,7}, { -6,-11,-31 } }, 3), vertex, inds); // {6,11,31}
		/*
		glGenTextures(1, &_texture);
		glBindTexture(GL_TEXTURE_2D, _texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _diceText.getWidth(), _diceText.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, _diceText.getBytes());
		glGenerateMipmap(GL_TEXTURE_2D);

		*/

		glGenVertexArrays(1, &_vertexArray);
		glBindVertexArray(_vertexArray);

		glGenBuffers(1, &_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex[0]) * vertex.size(), &vertex[0], GL_STATIC_DRAW);

		/*
		GLint pozicija2 = glGetAttribLocation(_shaderProgram, "texturePos");
		glVertexAttribPointer(pozicija2, 2, GL_FLOAT, false, sizeof(GLfloat) * 5, (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(pozicija2);
		*/

		GLint pozicija = glGetAttribLocation(_shaderProgram, "position");
		glVertexAttribPointer(pozicija, 3, GL_FLOAT, false, sizeof(GLfloat) * 3, (GLvoid*)nullptr);
		glEnableVertexAttribArray(pozicija);

		

		

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			mu::dbgLog("ERROR! OpenGL error in setupVBO! Error code = %x", error);
			assert(false);
		}


		float fov = 45.0f; // Field of view in degrees
		float aspectRatio = 16.0f / 9.0f; // Aspect ratio of the viewport
		float nearClip = 0.1f; // Near clipping plane
		float farClip = 100.0f; // Far clipping plane

		// Create a perspective matrix
		_perspectiveMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);

		_MVPmat = glGetUniformLocation(_shaderProgram, "MVP");

		setMovement(&_player);

	}


	void OnDraw(const gui::Rect& rect)
	{

		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(_shaderProgram);
		glBindVertexArray(_vertexArray);

		/*
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);
		GLint poz = glGetUniformLocation(_shaderProgram, "texture0");
		glUniform1i(poz, 0);
		*/

		glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer);//
		
		glm::mat4 mat = _perspectiveMatrix * _player.getMatrix();
		
		glUniformMatrix4fv(_MVPmat, 1, GL_FALSE, &mat[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, vertex.size()/3);


		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			mu::dbgLog("ERROR! OpenGL error in setupVBO! Error code = %x", error);
			assert(false);
		}



		glBindBuffer(GL_ARRAY_BUFFER, 0);//

		glBindVertexArray(0);
		glUseProgram(0);


	}

	~ViewGL() {
		if (_shaderProgram == 0)
			return;
		glDeleteProgram(_shaderProgram);
	}

};
