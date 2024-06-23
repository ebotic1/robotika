//#include "main.hpp"
#include "Application.h"
#include <td/StringConverter.h>
#include <gui/WinMain.h>

int main(int argc, const char * argv[])
{
    Application app(argc, argv);
    app.init("BA");
    return app.run();
}

/*

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
#include "../../sceneClass/include/rootObject.h"
#include "../../sceneClass/include/objectCreator.h"




class ViewPipes : public ViewMove
{
    GLuint _program = 0, VAO = 0, VBO = 0;

    rootObject* root = nullptr;
    glm::mat4 _perspectiveMatrix;

    NoclipMove _player;
public:
    ViewPipes():
        _player({ 0, 0, 0 }, 100)
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
        glUniformBlockBinding(_program, matLoc, 0);
        root = new rootObject(0, 100);





        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);



        // Set up the perspective parameters
        float fov = 45.0f; // Field of view in degrees
        float aspectRatio = 16.0f / 9.0f; // Aspect ratio of the viewport
        float nearClip = 0.1f; // Near clipping plane
        float farClip = 2000.0f; // Far clipping plane

        _perspectiveMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        gui::gl::Context::enable(gui::gl::Context::Flag::CullFace);
        gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);

        setMovement(&_player);
        root->iterate(rootObject::action::setUpVAOdeduction);
    }


    bool prepareNextFrame() override
    {
        root->setMVP() = _perspectiveMatrix * getMVPMatrix();
        root->iterate(rootObject::action::update);
        root->iterate(rootObject::action::upload);
        return true;
    }

    void OnDraw(const gui::Rect& rect) override
    {
        // Clear
        gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);
        gui::gl::Context::clear({ gui::gl::Context::Clear::Color, gui::gl::Context::Clear::Depth });
        glUseProgram(_program);

        root->iterate(rootObject::action::draw);

        glUseProgram(0);


    }

    ~ViewPipes()
    {
        makeCurrentContext();
        delete root;

        if (_program)
        {
            glDeleteProgram(_program);
            _program = 0;
        }

        if (VAO)
        {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO && false)
        {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
    }

};


*/