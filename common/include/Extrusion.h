#pragma once
#include "ExtrusionLib.h"
#include <gui/gl/View.h>
#include <gui/Key.h>
#include <gui/Texture.h>
#include <gui/FileDialog.h>
#include <gui/gl/CommandList.h>
#include <gui/gl/Buffer.h>
#include <gui/gl/Textures.h>

EXTRUSION_SHLIB_API void Extrude(gui::gl::Point2d* vertices, int nVertices, gui::gl::Buffer& gpuBuffer, float* dx, float* dy, float* dz, int points);



class EXTRUSION_SHLIB_API Extrusion {

	glm::vec2 *points;
	glm::vec2* pointsNormalized = nullptr;
	int pointCnt;

	glm::mat4 rollBackMatrix = glm::mat4(1.0f);
	glm::mat4 totalRotation = glm::mat4(1.0f);
	bool finishedSetup = false;
	bool calledAgain = false;

	glm::vec3 direction = {0.0f , 0.0f, 0.0f }; //starting direction (if 0 then defined by extrusion path)
	bool outsideVisible = true; //otside side visible
	bool addMidpoint = true;
	float maxAngle = glm::radians(90.01);
	bool keepCentered = true;

	bool addNormals = false;
	bool addTextureCoord = false;
	float repeatx = 1.0f, repeaty = 1.0f;
	cnt::PushBackVector<float>xTexture, yTexture;
	cnt::PushBackVector<int>_steps;
	int addBlank = 0;
	std::vector<GLfloat> saveState;

	float repeat = 1;

	int coords = 3;

	void AddIndices(std::vector<GLuint>& indices, int start);
	void setxTexture();
	void setyTexture(const std::vector<glm::vec3>& path, int size);
	//pomocna funkcija za kreiranje indices niza za omotac

public:
	Extrusion(glm::vec2 *points, int pointCnt);
	Extrusion(const Extrusion&) = delete;
	Extrusion(Extrusion&&) = delete;
	void reset();

	std::vector<glm::vec3> generateSmoothPoints(const std::vector<glm::vec3>& roughPoints, int pointsPerLenght, const glm::vec3 *startAngle = nullptr, const glm::vec3 *endAngle = nullptr);

	void Extrude(const std::vector<glm::vec3>& path, std::vector<GLfloat>& verts, std::vector<GLuint>& inds);
	void Extrude(const std::vector<glm::vec3> &path, gui::gl::Buffer& glBuf, gui::gl::Textures::Unit TU = gui::gl::Textures::Unit::T0, int Texind = 0); //inds i verts su vektori za buffer
	//path je ovdje delta odnosno definise za koliko da se pomjeri oblik a ne gdje da se pomjeri. Ovako mi bilo lakse i logicnije ali nije tesko dobiti niz delte za niz tacaka samo se oduzmu clanovi
	//ova funkcija treba biti izmjenjena da ne vraca vektor nego da primi referencu na 2 vektora kao u profesorovoj gui::gl::triangulate funkciji
	//ne bi bilo lose i da postoji neka verzija koja automatski stavlja sve u buffer ali nije hitno bitnije su druge stvar

	void setDirection(const glm::vec3& direction);
	glm::vec3 getDirection();

	void setVisibleSide(bool outsideVisible = true);
	void setAddMidpoints(bool addMidpoint = true, float maxDegree = 90); //ugao u stepenima
	void setKeepCentered(bool keepShapeCenteredInPath = true);

	void setAddNormals(bool addNormals = false);
	void setAddTextureCoordinates(bool addTextureCoord = false, int x = 1, int y = 1);
	void setAddBlankSpace(int reserveSpacePerVertex);

	int getNormalsOffset();
	int getTextureCoordOffset();
	int getBlankOffset();

	int getVertexSize();

	void setRepeat(float rep);

	~Extrusion();
};