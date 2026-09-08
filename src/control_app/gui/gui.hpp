#pragma once

#include <vector>
#include <map>

#include "external/imgui/imgui.h"
#include "external/imgui/backends/imgui_impl_glfw.h"
#include "external/imgui/backends/imgui_impl_opengl3.h" //

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include "robobrawl/device/robobrawl_base.hpp"
#include "robobrawl/device/known_devices.hpp"
#include "control_app/roles/control.hpp"
//ah I might need the known devices or something included... or just init pass though... 


namespace robobrawl{
    //vars and shi 
    //gui vars
    extern GLFWwindow*        window;
    extern float main_scale;
    extern ImGuiIO io;
    extern ImGuiStyle style;
    extern ImFont* font;

    //colors
    extern ImVec4 purple;
    extern ImVec4 yellow;
    extern ImVec4 orange;
    extern ImVec4 white;
    extern ImVec4 black;
    extern ImVec4 cyan;
    extern ImVec4 grey;
    extern ImVec4 green;
    extern ImVec4 red;
    extern ImVec4 blue;
    extern ImVec4 clear_color;

    //button vars
    extern std::map<robobrawl::SystemId, bool> open_control_window_;



    //data vars? 
    extern std::vector<Robobrawl_Control*> robobrawl_devices;
    extern std::map<States, std::string> state_to_string_map; 
    extern bool done;
    extern KnownDevices known_devices;


    // figure out how to setup... pu into gui_setup.cpp 

    int gui_setup();
    
    static void glfw_error_callback(int error, const char* description)
    {
        fprintf(stderr, "GLFW Error %d: %s\n", error, description);
    }


    //then do a seperate file for all of the other things... 

    void gui_main_loop();

    int gui_draw_selection_menu(Robobrawl_Control* device);

    //popups done in the thing
    int gui_draw_contol_menu(Robobrawl_Control* device);

    int gui_draw_timer();

    // I think htats all I got 
 
    void gui_cleanup();

}
