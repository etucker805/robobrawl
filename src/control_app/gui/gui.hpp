#pragma once

#include <vector>

#include "external/imgui/imgui.h"
#include "external/imgui/backends/imgui_impl_glfw.h"
#include "external/imgui/backends/imgui_impl_opengl3.h" //

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "robobrawl/device/base.hpp"
//ah I might need the known devices or something included... or just init pass though... 


namespace robobrawl{
    //vars and shi 
    //gui vars
    GLFWwindow* window;
    float main_scale;
    ImGuiIO& io;
    ImGuiStyle& style;
    const char* glsl_version = nullptr;
    ImFont* font;

    //colors
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);



    //button vars



    //data vars? 
    extern std::vector<Robobrawl_BaseDevice*> robobrawl_devices;
    extern bool done;


    // figure out how to setup... pu into gui_setup.cpp 

    int gui_setup(/*? nothing ? */);
    
    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }


    //then do a seperate file for all of the other things... 

    void gui_main_loop();

    int gui_draw_selection_menu();

    //popups done in the thing
    int gui_draw_contol_menu();

    int gui_draw_timer();

    // I think htats all I got 
 
    void gui_cleanup();

}
