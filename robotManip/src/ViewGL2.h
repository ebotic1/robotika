#pragma once
#include <gui/gl/View.h>
#include <gui/Key.h>
#include <gui/Texture.h>
#include "Program.h"
#include <gui/gl/CommandList.h>
#include <gui/gl/Buffer.h>
#include "./../../viewMovement/include/viewMovement.h"
#include "./../../viewMovement/include/noclip.h"
#include "../../common/include/Extrusion.h"

#include <gui/gl/Camera.h>

//Testni program sa natID klasama
class ViewGL2 : public ViewMove
{
    using Base = gui::gl::View;
    gui::Texture _frameTexture; //for frame export
    gui::Texture _diceText; //to map on cube
    glm::mat4 _perspectiveMatrix;
    glm::mat4 _mvpMatrix;
    
    gui::gl::Textures _textures;
    gui::gl::Buffer _gpuBuffer;
    
    Program _program;
    gui::gl::Command* pMtxSetterCmd = nullptr;
    gui::gl::Command* pCubeCmd = nullptr;

    NoclipMove _player;
    

    std::vector<GLfloat> _vec;
    
//    const unsigned int nTriangles = 6 * 2;   // Number of triangles rendered
    td::BYTE _usedTexture = 0; //0 or 1
public:
    ViewGL2()
        : _diceText(":dice")
        , _player({ 0,0,-3 }, 0.5)
    {
        //specify buffer layouts before creatin the context
        _gpuBuffer.init(64, 100, 100, { gui::gl::DataType::vec3});

        gui::gl::Context reqContext(gui::gl::Context::Animation::Yes);
        createContext(reqContext, { gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::CursorEnterLeave });
        //setAnimation(true);
    }

    bool setupShaders()
    {
        if (!_program.link(":shVert", ":shFrag"))
            return false;
        return true;
    }
    
    //setup data and send it to GPU, create command list
            
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

        pMtxSetterCmd = _gpuBuffer.createCommand();
        pMtxSetterCmd->createMVPSetter(&_mvpMatrix);
        _gpuBuffer.encode(pMtxSetterCmd);

        GLfloat kocka[] =
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
            {1.0f,1.0f},
            {0.0f,1.0f}
        };

        cnt::PushBackVector<float>vertex;
        cnt::PushBackVector <td::UINT4> inds;

        Extrusion ext(oblik, 4);
        ext.setDirection({ 0,0,1});
        ext.setAddMidpoints(false);
        ext.setVisibleSide(false); // false -> unutrasnjost je vidljiva
        //_vec = ext.Extrude({ {0, 0, 1 } }, vertex, inds);
        //_vec = ext.Extrude({ {2,2,0}, {0,0,3},  {0,0,3}, {0,-2,0} }, vertex, inds);
        //ext.Extrude(ext.generateSmoothPoints({ {0,0,0}, {6,-15,6}, {0, 5, 4}, {0, 7, -7}, {8,-9.5, -5} }, 1), vertex, inds);
        
        float *verts = new float[vertex.size()];
        float* poc = verts;
        for (auto x : vertex)
            *(poc++) = x;
        _gpuBuffer.appendVertices(verts, vertex.size() / 3);
        _gpuBuffer.appendIndices(inds);

        pCubeCmd = _gpuBuffer.createCommand();
        //pCubeCmd->createDrawArrays(gui::gl::Primitive::Triangles, 0, _vec.size() / 3);
        pCubeCmd->createDrawElements(gui::gl::Primitive::Triangles, 0, inds.size());
       // pCubeCmd->setColor(td::ColorID::Black);
        _gpuBuffer.encode(pCubeCmd);

        if (!_gpuBuffer.commit())
        {
            mu::dbgLog("ERROR! Cannot commit buffer to GPU");
            return;
        }

        _program.setBuffer(&_gpuBuffer);
        
        
        // Set up the perspective parameters
        float fov = 45.0f; // Field of view in degrees
        float aspectRatio = 16.0f / 9.0f; // Aspect ratio of the viewport
        float nearClip = 0.1f; // Near clipping plane
        float farClip = 100.0f; // Far clipping plane
        
        _perspectiveMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);
        gui::gl::Context::enable(gui::gl::Context::Flag::CullFace);

        setMovement(&_player);
        delete[] verts;
    }
    
    void OnDraw(const gui::Rect& rect) override
    {
        // Clear
        gui::gl::Context::clear(td::ColorID::White);
        gui::gl::Context::clear({gui::gl::Context::Clear::Color, gui::gl::Context::Clear::Depth});
        _mvpMatrix = _perspectiveMatrix * _player.getMatrix();

        _program.activate();
        
        _program.execute();

        // Unbind program
        _program.deActivate();
    }

    ~ViewGL2()
    {
        makeCurrentContext();
    }
    
};
