//
//  Created by Izudin Dzafic on 28/07/2020.
//  Copyright © 2020 IDz. All rights reserved.
//
#pragma once
#include <gui/Window.h>
#include "MenuBar.h"
#include "ViewGL.h"
#include "ViewGL2.h"
#include "ViewGLTexture.h"
#include "ViewGLGenerisanje.h"
#include "viewPipes.h"

class MainWindow : public gui::Window
{
private:
protected:
    ViewPipes _viewCubeMap;
public:
    MainWindow()
    : gui::Window(gui::Geometry(50, 50, 1200, 600))
    {
        setCentralView(&_viewCubeMap);
    }
};
