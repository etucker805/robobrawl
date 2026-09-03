#include "gui.hpp"

ImVec4 robobrawl::purple = ImVec4(100, 0, 100, 1);
ImVec4 robobrawl::yellow = ImVec4(253, 253, 150, 1);
ImVec4 robobrawl::orange = ImVec4(255, 95, 5, 1);
ImVec4 robobrawl::white  = ImVec4(255, 255, 255, 1);
ImVec4 robobrawl::black  = ImVec4(0, 0, 0, 1);
ImVec4 robobrawl::cyan   = ImVec4(60, 200, 250, 1 );
ImVec4 robobrawl::grey   = ImVec4(100, 100, 100, 1);

std::map<robobrawl::SystemId, bool> robobrawl::open_control_window_ = {};


void robobrawl::gui_main_loop()
{
do{
    for(auto device : robobrawl_devices){
        glfwPollEvents();
    
        if(glfwWindowShouldClose(window)){
            done = 1; 
        }
    
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }
    
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    
        // gui_draw_selection_menu(device );
        ImGui::Begin("TEST");
        ImGui::End();
    
    
        ImGui::EndFrame();
    }
    

} while(0);


}


//may split these into seperatet files.... 
int robobrawl::gui_draw_selection_menu(Robobrawl_Control* device)
{
    ImGui::Begin("Select System");

    for(auto system_id : known_devices.known_systems_ ){
        // @TODO push style for claimed -----------------
        if(std::find(device->claimed_systems_.begin(), device->claimed_systems_.end(), system_id) != device->claimed_systems_.end()){
            ImGui::PushStyleColor(1, cyan);   
        } else {
            ImGui::PushStyleColor(1, grey);   
        }
        if(ImGui::Button(system_id.c_str(), ImVec2(200, 100))){
            open_control_window_[system_id] = !open_control_window_[system_id];
        }
        ImGui::PopStyleColor();

    }

    ImGui::End();
    return 0;
}

int robobrawl::gui_draw_contol_menu()
{
    for(auto system_open_ : open_control_window_){
    if(system_open_.second){
        ImGui::Begin(system_open_.first.c_str());
        {

            // grey out everyting if not claimed,
            
            
            
            
            
            
            
        }
        ImGui::End();
    }
    }


    return 0;
}

int robobrawl::gui_draw_timer()
{
    return 0;
}
