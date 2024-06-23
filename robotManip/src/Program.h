#pragma once
#include <gui/gl/Program.h>

class Program : public gui::gl::Program{

protected:
    
    void onLinked() override{
        _matrixLocation = getUniformLocation("MVP");
        assert(_matrixLocation >= 0);
    }
    
    void onError() override{
        assert(false);
    }
    
public:
    Program(){}

    void setMVP(const glm::mat4x4& mvp){
        setUniformValue(_matrixLocation, mvp);
    }
    
};
