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


//constexpr float radius = 40;
//std::vector<glm::vec3> path;

/*bool collision(glm::vec3 position, glm::vec3& collisionNormal, glm::vec3& collisionPoint)
{


    /*

    ovaj kod sluzi samo za neku cool demonstraciju nije napisan na optimizovan nacin ili na nacin da se moze iskoristiti u buducnosti za nesto drugo.
    samo proof of contept da se vidi sta se moze napraviti

    */
    /*if (path.size() == 0)
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
            path_start += path[i - 1];
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

            if (glm::dot(projection - path_start, vektor) < 0 || glm::dot(projection - vektor, vektor) > 0)
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
}*/



class ViewGLGenerisanje : public ViewMove
{
    using Base = gui::gl::View;
    gui::Texture _frameTexture; //for frame export
    gui::Texture _diceText; //to map on cube
    gui::Texture _red; 
    gui::Texture _green; 
    gui::Texture _blue; 
    glm::mat4 _perspectiveMatrix;
    glm::mat4 _mvpMatrix;
    gui::gl::Command* last;
    int korak = 0;
    int usedTex = 0;

    gui::gl::Textures _textures;
    gui::gl::Buffer _gpuBuffer;

    ProgramTex _program;
    gui::gl::Command* pMtxSetterCmd = nullptr;
    gui::gl::Command* pCubeCmd = nullptr;

    NoclipMove _player;
    defaultMovement _player2;


    std::vector<GLfloat> _vec;

public:
    ViewGLGenerisanje()
        : _diceText(":dice")
        , _red(":red")
        , _green(":green")
        , _blue(":blue")
        , _player({ 0, 0, -1000}, 100)
    {
        //specify buffer layouts before creatin the context
        _gpuBuffer.init(64, 100, 100, { gui::gl::DataType::vec3, gui::gl::DataType::vec2, gui::gl::DataType::vec3 });

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

    void Generate() {
        constexpr int cnt = 20;
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
        ext.setVisibleSide(false); // false -> unutrasnjost je vidljiva
        ext.setAddNormals(true);
        ext.setAddTextureCoordinates(true);
        ext.setKeepCentered(false); //https://youtube.com/clip/UgkxxG3qWNZqE-1Riy6wdIEjqOfGL1ow8whg?si=BEviOwfKR5D8Q7_V
        //ext.Extrude({ {0, 0, 1 } }, _gpuBuffer);
        //ext.Extrude({ {2,2,0}, {0,0,3},  {0,0,3}, {0,-2,0} }, vertex, inds);
        //ext.Extrude(ext.generateSmoothPoints({ {0,0,0}, {6,-15,6}, {0, 5, 4}, {0, 7, -7}, {8,-9.5, -5} }, 1), _gpuBuffer);

        ext.setAddMidpoints(true, 20);
        ext.setRepeat(400);

        //ext.Extrude({ {0,0,10}, {0,30,0}, {0,0,30} }, _gpuBuffer);
        //ext.Extrude({ {0,0,10}, {0,30,0}, {0,0,30} }, _gpuBuffer);
        //ext.Extrude({ {0,0,10}, {0,30,0}, {0,0,30} }, _gpuBuffer);
        std::vector<glm::vec3>rghpoints;
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> distx(0, 100);
        std::uniform_int_distribution<std::mt19937::result_type> disty(0, 100);
        std::uniform_int_distribution<std::mt19937::result_type> distz(0, 100);
        std::uniform_int_distribution<std::mt19937::result_type> dist(0, 3);
        std::uniform_int_distribution<std::mt19937::result_type> rgb(0, 2);
        /*for (int i = 0; i < 100; i++) {
            glm::vec3 pom;

            pom[0] = (int)distx(rng)-50;
            pom[1] = (int)disty(rng)-50;
            pom[2] = (int)distz(rng)-50;

            rghpoints.emplace_back(pom);
        }*/
        int x = 0, y = 0, z = 100;
        int minus = 1;
        rghpoints.emplace_back(x, y, z);
        int ux = 0, uy = 0, uz = 100;
        for (int i = 0; i < 100; i++) {

            int k = (int)dist(rng);
            switch (k) {
            case 0:
                x = 50 + (int)distx(rng);
                y = 0;
                z = -1 * minus;
                break;
            case 1:
                x = -50 - (int)distx(rng);
                y = 0;
                z = -1 * minus;
                break;
            case 2:
                y = 50 + (int)distx(rng);
                x = 0;
                z = -1 * minus;
                break;
            case 3:
                y = -50 - (int)distx(rng);
                x = 0;
                z = -1 * minus;
                break;

            }
            ux += x;
            uy += y;
            uz += z;
            if (ux >= 500 || ux <= -500 || uy >= 300 || uy <= -300) {
                ux -= x;
                uy -= y;
                uz -= z;
                i--;
                continue;
            }
            if (i % 10 == 0) minus *= -1;
            rghpoints.emplace_back(x, y, z);
        }

        //path = ext.generateSmoothPoints(rghpoints, 2);
        //path = ext.generateSmoothPoints({ {0,0,300}, {50,0,300}, {100,50,300}, {100,50,300}, {50,0,300}, {0,-50,300} }, 2);
        //path = ext.generateSmoothPoints({ {0, 0, 0}, {20, 0, 0} }, 2);



        //ext.Extrude(rghpoints, _gpuBuffer);
        //ext.Extrude({ {0, 0, 0}, {20, 0, 0} }, _gpuBuffer);

        //ZASTO OVO NE RADI?
        //ext.Extrude({ {0, 0, 0}, {20, 0, 0} }, vertex, inds);


        //ext.Extrude(rghpoints, vertex, inds);

        _gpuBuffer.reset();
        _gpuBuffer.init(64, 100, 100, { gui::gl::DataType::vec3, gui::gl::DataType::vec2, gui::gl::DataType::vec3 });
        pMtxSetterCmd = _gpuBuffer.createCommand();
        pMtxSetterCmd->createMVPSetter(&_mvpMatrix);
        _gpuBuffer.encode(pMtxSetterCmd);
        float* vert = new float[vertex.size()];
        float* poc = vert;
        for (auto x : vertex)
            *(poc++) = x;
        _gpuBuffer.appendVertices(vert, vertex.size() / 8);
        _gpuBuffer.appendIndices(inds);
        auto cmd = _gpuBuffer.createCommand();
        //pCubeCmd->createDrawArrays(gui::gl::Primitive::Triangles, 0, _vec.size() / 3);
        cmd->createDrawElements(gui::gl::Primitive::Triangles, 0, 0);
        usedTex = (int)rgb(rng);
        cmd->setTexture(gui::gl::Textures::Unit::T0, usedTex);
        _gpuBuffer.encode(cmd);


        if (!_gpuBuffer.commit())
        {
            mu::dbgLog("ERROR! Cannot commit buffer to GPU");
            return;
        }
        _textures.append({ &_red, &_green, &_blue });
        _program.setTextureUnit();
        _program.setBuffer(&_gpuBuffer);
        _program.setTextures(&_textures);

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



        Generate();

       


        // Set up the perspective parameters
        float fov = 45.0f; // Field of view in degrees
        float aspectRatio = 16.0f / 9.0f; // Aspect ratio of the viewport
        float nearClip = 0.1f; // Near clipping plane
        float farClip = 2000.0f; // Far clipping plane

        _perspectiveMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearClip, farClip);
        gui::gl::Context::enable(gui::gl::Context::Flag::CullFace);
        gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);

        setMovement(&_player); // PREBACITI NA &_player ZA NOCLIP POMJERANJE (BEZ KOLIZIJE)
        //gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);
        //gui::gl::Context::enable(gui::gl::Context::Flag::CullFace);

    }

    void OnDraw(const gui::Rect& rect) override
    {
        // Clear
        gui::gl::Context::enable(gui::gl::Context::Flag::DepthTest);
        gui::gl::Context::clear({ gui::gl::Context::Clear::Color, gui::gl::Context::Clear::Depth });


        _mvpMatrix = _perspectiveMatrix * getMVPMatrix();

        if (korak < _gpuBuffer.getNoOfIndices()-80) korak+=80;
        else {
            korak = 0;
            Generate();  //OVDJE JE PROBLEM
        }
        auto cmd = _gpuBuffer.createCommand();
        //pCubeCmd->createDrawArrays(gui::gl::Primitive::Triangles, 0, _vec.size() / 3);
        cmd->createDrawElements(gui::gl::Primitive::Triangles, 0, korak);
        cmd->setTexture(gui::gl::Textures::Unit::T0, usedTex);
        _gpuBuffer.encode(cmd);


        if (!_gpuBuffer.commit())
        {
            mu::dbgLog("ERROR! Cannot commit buffer to GPU");
            return;
        }



        _program.activate();

        _program.execute();

        // Unbind program
        _program.deActivate();
        _gpuBuffer.remove(cmd);
    }

    ~ViewGLGenerisanje()
    {
        makeCurrentContext();
    }

};
