#pragma once

/*
* Definition of ENGINE2D_API macro for MSVC DLL import/export
* CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS works for everything except global and static variables
* Hence, we need to define ENGINE2D_API to decorate classes that have static variables
*/
#ifdef _MSC_VER
    #ifdef ENGINE2D_EXPORTS
        #define ENGINE2D_API __declspec(dllexport)
    #else
        #define ENGINE2D_API __declspec(dllimport)
    #endif
#else
    #define ENGINE2D_API
#endif