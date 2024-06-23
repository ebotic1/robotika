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

constexpr float radius = 40;
std::vector<glm::vec3> path;

bool collision(glm::vec3 position, glm::vec3 &collisionNormal, glm::vec3 &collisionPoint) 
{


    /*
    
    ovaj kod sluzi samo za neku cool demonstraciju nije napisan na optimizovan nacin ili na nacin da se moze iskoristiti u buducnosti za nesto drugo.
    samo proof of contept da se vidi sta se moze napraviti
    
    */
    if (path.size() == 0)
        return false;
    glm::vec3 projection;
    projection = (glm::dot(position, path[0]) / glm::dot(path[0], path[0])) * path[0];
    if (glm::dot(projection, path[0]) < 0) {
        collisionNormal = glm::normalize(-path[0]);
        glm::vec3 wall(1, 0, 0);
        collisionPoint = (glm::dot(position, wall) / glm::dot(wall, wall)) * wall;
        return true;
    }

    auto originalPos = position;

    float minLen = 999999;
    int minIndex = -1;

    glm::vec3 path_start(0), vektor;

    for (int i = 0; i < path.size(); ++i) {
        if (i != 0) {
            path_start += path[i-1];
        }
        vektor = path_start + path[i];

        projection = (glm::dot(position, vektor) / glm::dot(vektor, vektor)) * vektor;

        if (glm::dot(projection - path_start, vektor) < 0 || glm::dot(projection - vektor, vektor) > 0)
            continue;

        if (glm::length(projection - position) < minLen) {
            minLen = glm::length(projection - position);
            minIndex = i;
        }

    }

    if (minIndex == -1)
        int x = 3;


    position = originalPos;
    path_start = glm::vec3(0, 0, 0);

    for (int i = 0; i < path.size(); ++i) {
        if (i != 0) {
            path_start += path[i - 1];
        }

        if (i == minIndex) {
            vektor = path_start + path[i];
            projection = (glm::dot(position, vektor) / glm::dot(vektor, vektor)) * vektor;

            if (glm::dot(projection - path_start, vektor) < 0 || glm::dot(projection  - vektor, vektor) > 0)
                break;

            if (glm::length(position - projection) > radius) {
                collisionNormal = glm::normalize(position - projection);
                collisionPoint = projection + (radius - 0.001f) * collisionNormal;
                return true;
            }
            break;
        }

    }

    return false;
}


class defaultMovement : public PlayerMove {

    glm::vec3 position = { 0.0f, 0.0f, 0.0f };
    glm::vec3 velocity = { 0.0f, 0.0f, 0.0f };

public:
    defaultMovement() :
        PlayerMove({0.0f, 0.0f, 2.0f})
    {

    }

    void move(const std::set<int>& keyboardPool, float timeStep) override
    {
        glm::vec3 direction = cam.getDirection();
        direction *= (keyboardPool.contains('w') ? 1 : 0) - (keyboardPool.contains('s') ? 1 : 0);
        direction += cam.getRight() * ((keyboardPool.contains('d') ? 1.0f : 0.0f) - (keyboardPool.contains('a') ? 1 : 0));


        glm::vec3 collisionPoint, collisionNormal;
        if (collision(position + glm::vec3(0, 0.1f, 0), collisionNormal, collisionPoint) && 1 - glm::dot(collisionNormal, glm::vec3(0, 1, 0)) < 0.3) 
        {
            if (keyboardPool.contains(' ')) {
                velocity.y = -4;
            }
        }
        else {
            velocity += glm::vec3{ 0,0.009 * timeStep,0 }; //gravitacija
        }
   
   
        direction.y = 0;
        if(glm::length(direction) > 0.1)
            direction = glm::normalize(direction);
        direction *= timeStep * 0.004;

        
        velocity += direction;
       


        
        if (collision(position + velocity, collisionNormal, collisionPoint))
        {

            velocity -= (glm::dot(velocity, collisionNormal) / glm::dot(collisionNormal, collisionNormal)) * collisionNormal;
            position = collisionPoint;
       

        }


        position += velocity;
        velocity *= 0.9; //friction
        cam.setPosition(position+ glm::vec3(0,-10,0));


        

    }



    void mouseMove(glm::vec2 delta) override
    {
        delta *= 0.01;
        cam.rotate(delta.x, delta.y);
    }
};



class ViewGLTexture : public ViewMove
{
    using Base = gui::gl::View;
    gui::Texture _frameTexture; //for frame export
    gui::Texture _diceText; //to map on cube
    glm::mat4 _perspectiveMatrix;
    glm::mat4 _mvpMatrix;
    
    gui::gl::Textures _textures;
    gui::gl::Buffer _gpuBuffer;
    
    ProgramTex _program;
    gui::gl::Command* pMtxSetterCmd = nullptr;
    gui::gl::Command* pCubeCmd = nullptr;

    NoclipMove _player;
    defaultMovement _player2;
    

    std::vector<GLfloat> _vec;
    
public:
    ViewGLTexture()
        : _diceText(":dice")
        , _player({ 600,200,600 }, 10)
    {
        //specify buffer layouts before creatin the context
        _gpuBuffer.init(64, 100, 100, { gui::gl::DataType::vec3, gui::gl::DataType::vec2, gui::gl::DataType::vec3});

        gui::gl::Context reqContext(gui::gl::Context::Animation::Yes, gui::gl::DepthBuffer::Size::b16);
        createContext(reqContext, { gui::InputDevice::Event::Keyboard, gui::InputDevice::Event::CursorMove, gui::InputDevice::Event::CursorEnterLeave });
        //setAnimation(true);
    }

    bool setupShaders()
    {
        if (!_program.link(":shTxVert", ":shTxFrag"))
            return false;
        return true;
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

        pMtxSetterCmd = _gpuBuffer.createCommand();
        pMtxSetterCmd->createMVPSetter(&_mvpMatrix);
        _gpuBuffer.encode(pMtxSetterCmd);




        constexpr int cnt = 6;
        glm::vec2 krug[cnt];

        for (int i = 0; i < cnt; ++i)
            krug[i] = { radius * glm::cos(i * 2 * glm::pi<float>() / cnt), radius * -glm::sin(i * 2 * glm::pi<float>() / cnt) };

        glm::vec2 oblik[] =
        {
            {0.0f,0.0f},
            {1.0f,0.0f},
            {1.0f,1.0f},
            {0.0f,1.0f}
        };

        cnt::PushBackVector<float>vertex;
        cnt::PushBackVector <td::UINT4> inds;


        Extrusion ext(krug, cnt);
        //ext.setDirection({ 0,-1,0 });
        ext.setVisibleSide(true); // false -> unutrasnjost je vidljiva
        ext.setAddNormals(true);
        ext.setAddTextureCoordinates(true);
        ext.setKeepCentered(false); //https://youtube.com/clip/UgkxxG3qWNZqE-1Riy6wdIEjqOfGL1ow8whg?si=BEviOwfKR5D8Q7_V
        //ext.Extrude({ {0, 0, 1 } }, _gpuBuffer);
        //ext.Extrude({ {2,2,0}, {0,0,3},  {0,0,3}, {0,-2,0} }, vertex, inds);
        //ext.Extrude(ext.generateSmoothPoints({ {0,0,0}, {6,-15,6}, {0, 5, 4}, {0, 7, -7}, {8,-9.5, -5} }, 1), _gpuBuffer);

        ext.setAddMidpoints(true, 50);
        ext.setRepeat(400);

        //ext.Extrude({ {0,0,10}, {0,30,0}, {0,0,30} }, _gpuBuffer);
        //ext.Extrude({ {0,0,10}, {0,30,0}, {0,0,30} }, _gpuBuffer);
        //ext.Extrude({ {0,0,10}, {0,30,0}, {0,0,30} }, _gpuBuffer);

        path = ext.generateSmoothPoints({ {0,0,300}, {50,0,300}, {100,50,300}, {100,50,300}, {50,0,300}, {0,-50,300} }, 2);

        ext.Extrude(path , _gpuBuffer);

       
        

        if (!_gpuBuffer.commit())
        {
            mu::dbgLog("ERROR! Cannot commit buffer to GPU");
            return;
        }

        _textures.append({ &_diceText });
        _program.setTextureUnit();
        _program.setBuffer(&_gpuBuffer);
        _program.setTextures(&_textures);
        
        
        // Set up the perspective parameters
        float fov = 45.0f; // Field of view in degrees
        float aspectRatio = 16.0f / 9.0f; // Aspect ratio of the viewport
        float nearClip = 0.1f; // Near clipping plane
        float farClip = 2000.0f; // Far clipping plane
        
        _perspectiveMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        gui::gl::Context::enable(gui::gl::Context::Flag::CullFace);
        gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);

        setMovement(&_player2); // PREBACITI NA &_player ZA NOCLIP POMJERANJE (BEZ KOLIZIJE)
        //gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);
        //gui::gl::Context::enable(gui::gl::Context::Flag::CullFace);
        
    }
    
    void OnDraw(const gui::Rect& rect) override
    {
        // Clear
        gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);
        gui::gl::Context::clear({gui::gl::Context::Clear::Color, gui::gl::Context::Clear::Depth});
        

        _mvpMatrix = _perspectiveMatrix * getMVPMatrix();

        _program.activate();
        
        _program.execute();

        // Unbind program
        _program.deActivate();
    }

    ~ViewGLTexture()
    {
        makeCurrentContext();
    }
    
};
