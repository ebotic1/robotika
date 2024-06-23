#pragma once
#include <gui/gl/Program.h>

class ProgramTex : public gui::gl::Program{
    td::INT4 _textureSamplerLoc = -1;
protected:
    
    void onLinked() override{
        _matrixLocation = getUniformLocation("MVP");
        _textureSamplerLoc = getUniformLocation("textureSampler");
        assert(_matrixLocation >= 0 && _textureSamplerLoc >= 0);
    }
    
    void onError() override{
        assert(false);
    }
    
public:
    ProgramTex(){}

    void setMVP(const glm::mat4x4& mvp){
        setUniformValue(_matrixLocation, mvp);
    }
    
    void setTextureUnit(){
        td::UINT4 textureUnit = 0;
        setUniformValue(_textureSamplerLoc, textureUnit);
    }
};
