#pragma once
#include "baseObject.h"
#include <map>
#include <unordered_set>






class rootObject : public baseObject
{

	int maxObjects;
	GLuint UBOmat, UBOcolor;
	GLuint uboMatricesBuffer, uboColorBuffer,commands;
	bool buffersChanged = true;

	std::vector<DrawElementsIndirectCommand> commandsBuffer;
	std::vector<glm::mat4> MVPbuffer;
	std::vector<glm::vec4> colorsBuffer;

	std::vector<unsigned int> colorsToUpdate;
	unsigned int colorsToUpdateIndex = 0;

	std::stack<int>  freeIndexes;
	std::map<GLuint, std::map<GLuint, std::unordered_set<object *>>> bufferTree;
	std::map <GLuint, bool> VAOhasElementBuffer;


	bool addObject(object* objectToAdd)
	{
		for (auto& obj : *objectToAdd) {
			bufferTree[obj.program][obj.VAO].emplace(&obj);

			if (freeIndexes.size() != 0) {
				obj.ID = freeIndexes.top();
				freeIndexes.pop();
			}
			else {
				if (MVPbuffer.size() == maxObjects) {
					delete objectToAdd;
					return false;
				}
				obj.ID = MVPbuffer.size();
				MVPbuffer.resize(MVPbuffer.size() + 1);
			}
		}
		return true;
	}

	void updateCpuMemSingle(object& ptr) {
		ptr.onIterate();




		if (ptr.updateColor) {
			if (colorsToUpdateIndex < colorsToUpdate.size())
				colorsToUpdate[colorsToUpdateIndex++] = ptr.ID;

			colorsBuffer[ptr.ID] = ptr.color;
			ptr.updateColor = false;
		}
	}

	void updateCpuMem(baseObject *o) {
		for (auto &ptr : *o)
		{
			updateCpuMemSingle(ptr);
			MVPbuffer[ptr.ID] = MVPbuffer[ptr.parent->ID] * ptr.MVP;

		}

	}
	
	
public:
	enum class action { update, upload, draw, setUpVAOdeduction};
	bool forceUpdateMVP = false;

	rootObject(GLuint UBObindingPointMVPmat4, GLuint UBObindingPointColorVec4, int maxObjects):
		maxObjects(maxObjects),
		UBOmat(UBObindingPointMVPmat4),
		UBOcolor(UBObindingPointColorVec4),
		MVPbuffer(1),
		baseObject()
	{
		glGenBuffers(1, &uboMatricesBuffer);
		glGenBuffers(1, &uboColorBuffer);
		glGenBuffers(1, &commands);
	}

	void changeBuffer(GLuint UBObindingPointMVPmat4, GLuint UBObindingPointColorVec4, int maxObjects) {
		this->maxObjects = maxObjects;
		UBOmat = UBObindingPointMVPmat4;
		UBOcolor = UBObindingPointColorVec4;
		buffersChanged = true;
	}


	void iterate(action act)
	{


		if (act == action::draw) {
			glBindBufferBase(GL_UNIFORM_BUFFER, UBOmat, uboMatricesBuffer); // PROMIJENITI PROMIJENITI PROMIJENITI
			glBindBufferBase(GL_UNIFORM_BUFFER, UBOcolor, uboColorBuffer);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commands);

			unsigned int offset = 0;

			for (auto& program : bufferTree) {
				glUseProgram(program.first);
				for (auto& VAO : program.second) {
					glBindVertexArray(VAO.first);

					if (VAOhasElementBuffer[VAO.first])
						glMultiDrawArraysIndirect(GL_TRIANGLES, (void*)(sizeof(DrawElementsIndirectCommand) * offset), VAO.second.size(), sizeof(DrawElementsIndirectCommand));
					else
						glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(sizeof(DrawElementsIndirectCommand) * offset), VAO.second.size(), sizeof(DrawElementsIndirectCommand));
						

					offset += VAO.second.size();
					
				}
			}
		}

		if (act == action::update) {
			commandsBuffer.resize(MVPbuffer.size());
			colorsBuffer.resize(MVPbuffer.size());
			colorsToUpdateIndex = 0;
			colorsToUpdate.resize(MVPbuffer.size() * 0.2); //ako treba 20% boja update-ovati onda bolje update sve boje

			onIterate();
			MVPbuffer[ID] = MVP;

			if (forceUpdateMVP || updateMVP)
				updateCpuMem(this);
			else {
				auto last = end();
				for (auto it = begin(); it != last;)
				{
					auto& ptr = *it;

					updateCpuMemSingle(ptr);



					if (ptr.updateMVP) {

						MVPbuffer[ptr.ID] = MVPbuffer[ptr.parent->ID] * ptr.MVP;
						updateCpuMem(&ptr);

						ptr.updateMVP = false;
						if (it.discardBranch() == last) //discardBrach se ponasao kao ++it naravno
							break;

					}
					else
						++it;

				}
			}
			updateMVP = false;

			int i = 0;
			for (auto& program : bufferTree)
				for (auto& VAO : program.second)
					for (auto ptr : VAO.second) {
						++i;
						if (ptr->updateVertices) {
							if (VAOhasElementBuffer[ptr->VAO])
								ptr->setUpCommandArray(commandsBuffer[i]);
							else
								ptr->setUpCommand(commandsBuffer[i]);

							ptr->updateVertices = false;
						}
					}


		}


		if (act == action::upload) {
			glBindBuffer(GL_UNIFORM_BUFFER, uboMatricesBuffer);
			glBufferData(GL_UNIFORM_BUFFER, MVPbuffer.size() * sizeof(glm::mat4), MVPbuffer.data(), GL_DYNAMIC_DRAW);
			
			if (buffersChanged || (colorsToUpdateIndex >= colorsToUpdate.size())) {
				glBindBuffer(GL_UNIFORM_BUFFER, uboColorBuffer);
				glBufferData(GL_UNIFORM_BUFFER, colorsBuffer.size() * sizeof(glm::vec4), colorsBuffer.data(), GL_STATIC_DRAW);
			}
			else if (colorsToUpdateIndex > 0) {
				glBindBuffer(GL_UNIFORM_BUFFER, uboColorBuffer);
				for (int ID : colorsToUpdate)
					glBufferSubData(GL_UNIFORM_BUFFER, ID * sizeof(glm::vec4), sizeof(glm::vec4), (void*)(&colorsBuffer[ID]));
			}
			


			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, commands);
			if (commandsBuffer.size() > 1)
				glBufferData(GL_DRAW_INDIRECT_BUFFER, (commandsBuffer.size() - 1) * sizeof(DrawElementsIndirectCommand), &(commandsBuffer[1]), GL_STATIC_DRAW);
			glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

			buffersChanged = false;
		}

		if (act == action::setUpVAOdeduction)
		{
			GLint elementBufferBinding;
			for (auto& program : bufferTree)
				for (auto& VAO : program.second) {
					glGetVertexArrayiv(VAO.first, GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementBufferBinding);
					VAOhasElementBuffer[VAO.first] = (elementBufferBinding == 0);
				}

		}

	}

	void appendObject(object* obj, baseObject* Where)
	{
		if (obj == nullptr || Where == nullptr)
			return;

		if (addObject(obj)) {
			Where->children.push_back(obj);
			obj->parent = Where;
		}

	}

	void appendObject(object* obj) {
		if (obj == nullptr)
			return;

		if (addObject(obj)) {
			children.push_back(obj);
			obj->parent = this;
		}

	}

	void deleteObject(object* obj) 
	{

		auto& cnt = obj->parent->children;
		cnt.erase(std::find(cnt.begin(), cnt.end(), obj));

		for (auto& o : *obj) 
			bufferTree[o.program][o.VAO].erase(&o);
		
		if (obj->ID == MVPbuffer.size() - 1)
			MVPbuffer.resize(MVPbuffer.size() - 1);
		else
			freeIndexes.push(obj->ID);

		delete obj;

		

	}

	virtual ~rootObject() {

	}


private:



};


