#pragma once
#include <gui/gl/View.h>
#include <gui/Key.h>
#include <gui/Texture.h>
#include "ProgramTex.h"
#include <gui/gl/CommandList.h>
#include <gui/gl/Buffer.h>
#include "./../../viewMovement/include/viewMovement.h"
#include "./../../viewMovement/include/noclip.h"
#include "../../common/include/Extrusion.h"
#include <gui/gl/Camera.h>
#include "ViewGLTexture.h"
#include <random>

#include "../../sceneClass/include/bufferManager.h"
#include "../../sceneClass/include/rootObject.h"

#include "../../polyInterpolation/include/PolyIntepolation.h"

struct showIncrementaly {
    unsigned int size, offset;
    double currentStep = 0;


    showIncrementaly(unsigned int size, unsigned int offset) :
        size(size),
        offset(offset)
    {

    }

    void operator()(object *ptr){
        if(currentStep < size)
            ptr->setVertices(offset, currentStep+=30);
    }

};


class ViewPipes : public ViewMove
{
    GLuint _program = 0, _program2;

    rootObject* root = nullptr;
    bufferManager* buffer;
    glm::mat4 _perspectiveMatrix;

    NoclipMove _player;
public:
    ViewPipes() :
        _player({ 0, 0, -1 }, 10)
    {
        gui::gl::Context reqContext(gui::gl::Context::Animation::Yes, gui::gl::DepthBuffer::Size::b16);
        createContext(reqContext, { gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::CursorEnterLeave });
    }

    bool setupShaders()
    {
        if (_program = linkProgram(":objVert", ":objFrag"))
            return true;
        return false;
    }


    void onInit() override
    {
        auto [major, minor] = getOpenGLVersion();
        mu::dbgLog("Used opengl %d.%d", major, minor);

        if (!setupShaders())
        {
            mu::dbgLog("ERROR! OpenGL cannnot setup shaders!");
            assert(false);
            return;
        }

        auto matLoc = glGetUniformBlockIndex(_program, "Matrices");
        auto colorLoc = glGetUniformBlockIndex(_program, "Colors");
        glUniformBlockBinding(_program, matLoc, 0);
        glUniformBlockBinding(_program, colorLoc, 1);
        root = new rootObject(0, 1, 100);
        

        /*

        GLfloat vert[] = {
            0,   0, 0,
            10.5, 0, 0,
            10.5, -1,   0,
            5, 5, 0
        };

        GLuint index[] = {
           0,1,2,
           0,1,3
        };



        root->appendObject(buffer->createCustom(vert, 12, index, 6, { 0,0,0 }, td::ColorID::BurlyWood));
       //root->appendObject(buffer->createCustom(vert, 12, index, 6, { 20,0,0 }, td::ColorID::Black));
       //root->appendObject(buffer->createCustom(vert, 12, index, 6, { 20,0,20 }, td::ColorID::BurlyWood));
       //root->appendObject(buffer->createCustom(vert, 12, index, 6, { 20,20,20 }, td::ColorID::BurlyWood));

        root->appendObject(buffer->createArrow({ 50,0,0 }, {-50,-50,0}, 1, 2, td::ColorID::Black));
        root->appendObject(buffer->createArrow({ 0,0,0 }, {0,-50,0}, 1, 2, td::ColorID::Red));

     */


        int pointCnt = 17;
        glm::vec2* points = new glm::vec2[pointCnt];
        constexpr float width = 5.0f;
        int ind = -1;
        float con = 2 * glm::pi<float>() / pointCnt;
        for (int i = 0; i < pointCnt; ++i)
            points[i] = { width * glm::cos(con * i), width * glm::sin(con * i) };


       

        std::vector<GLfloat> verts;
        std::vector<GLuint> inds;
        


        

        GLuint VAO, VBO, EBO;
        _program2 = linkProgram(":t1v", ":t1f");




        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*verts.size(), verts.data(), GL_DYNAMIC_DRAW);

        GLint positionAttrib = glGetAttribLocation(_program2, "position");
        glVertexAttribPointer(positionAttrib, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
        glEnableVertexAttribArray(positionAttrib);

        GLint positionAttrib2 = glGetAttribLocation(_program2, "normal");
        glVertexAttribPointer(positionAttrib2, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(positionAttrib2);


        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * inds.size(), inds.data(), GL_DYNAMIC_DRAW);

        glBindVertexArray(0);


        buffer = new bufferManager(_program2, VAO, VBO, EBO, 6);


        auto buffer2 = new bufferManager(_program, "position");
        //root->appendObject(buffer2->createArrow({ 0,0,0 }, { 70,-110,-100 }, 3, 4, td::ColorID::Black));



        Extrusion ext(points, pointCnt);
        ext.setAddNormals(true);
        ext.setAddTextureCoordinates(false);
        ext.setAddMidpoints(true, 25);
        ext.setKeepCentered(false);


        unsigned int offset = 0;
        std::vector<glm::vec3> put;
        std::srand(333);

        for(int i = 0; i< 7; ++i)
            for (int j = 0; j < 7; ++j) {
                put.clear();
                enum { up, down, left, right, forward, forward2 };

                for (int k = 0; k < 144; ++k) {
                    int r1 = std::rand() % 6;
                    int r2 = std::rand() % 60 + 20;

                    switch (r1)
                    {
                    case up:
                        put.emplace_back(0, -r2, 0);
                        break;
                    case down:
                        put.emplace_back(0, r2, 0);
                        break;
                    case left:
                        put.emplace_back(-r2, 0, 0);
                        break;
                    case right:
                        put.emplace_back(r2, 0, 0);
                        break;
                    default:
                        put.emplace_back(0, 0, r2);
                    }

                }
                int r3 = std::rand() % 146;




                ext.Extrude(put, verts, inds);
                auto ptr = buffer->createCustom(verts.data(), verts.size(), inds.data(), inds.size(), { i * 250, j*250,0 }, td::ColorID(r3));
                ptr->update = showIncrementaly(inds.size(), offset);
                offset += inds.size();
                root->appendObject(ptr);

                ext.reset();
                verts.clear();
                inds.clear();
            }





        // Set up the perspective parameters
        float fov = 45.0f; // Field of view in degrees
        float aspectRatio = 16.0f / 9.0f; // Aspect ratio of the viewport
        float nearClip = 0.1f; // Near clipping plane
        float farClip = 200000.0f; // Far clipping plane

        _perspectiveMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        //gui::gl::Context::enable(gui::gl::Context::Flag::CullFace);
        gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);

        setMovement(&_player);
        root->iterate(rootObject::action::setUpVAOdeduction);



        

    }


    bool prepareNextFrame() override
    {


        root->setMVP() = getMVPMatrix();

        root->iterate(rootObject::action::update);
        root->iterate(rootObject::action::upload);
        return true;
    }

    void OnDraw(const gui::Rect& rect) override
    {

        static bool setup = false;
        if (!setup) {
            auto VPloc = glGetUniformLocation(_program2, "VP");
            glUniformMatrix4fv(VPloc, 1, GL_FALSE, glm::value_ptr(_perspectiveMatrix));
            //setup = true;
        }

        // Clear
        //gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);
        gui::gl::Context::clear({ gui::gl::Context::Clear::Color, gui::gl::Context::Clear::Depth });

        root->iterate(rootObject::action::draw);

        

    }

    ~ViewPipes()
    {
        makeCurrentContext();
        delete root;
        delete buffer;

        if (_program)
        {
            glDeleteProgram(_program);
            _program = 0;
        }

    }

};
