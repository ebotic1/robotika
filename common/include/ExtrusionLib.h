#pragma once

#ifdef MU_WINDOWS
    #ifdef EXTRUSION_SHLIB_EXPORTS
        #define EXTRUSION_SHLIB_API __declspec(dllexport)
    #else
        #define EXTRUSION_SHLIB_API __declspec(dllimport)
    #endif
#else
    #ifdef EXTRUSION_SHLIB_EXPORTS
        #define EXTRUSION_SHLIB_API __attribute__((visibility("default")))
    #else
        #define EXTRUSION_SHLIB_API
    #endif
#endif


