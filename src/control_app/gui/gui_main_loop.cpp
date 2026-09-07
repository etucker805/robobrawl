#include "gui.hpp"

ImVec4 robobrawl::purple = ImVec4(100, 0, 100, 1);
ImVec4 robobrawl::yellow = ImVec4(253, 253, 150, 1);
ImVec4 robobrawl::orange = ImVec4(255, 95, 5, 1);
ImVec4 robobrawl::white  = ImVec4(255, 255, 255, 1);
ImVec4 robobrawl::black  = ImVec4(0, 0, 0, 1);
ImVec4 robobrawl::cyan   = ImVec4(60, 200, 250, 1 );
ImVec4 robobrawl::grey   = ImVec4(100, 100, 100, 1);
ImVec4 robobrawl::green  = ImVec4(0, 200, 0, 1);
ImVec4 robobrawl::clear_color       = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
std::map<robobrawl::SystemId, bool> robobrawl::open_control_window_ = {};
std::map<robobrawl::States, std::string> robobrawl::state_to_string_map;


void robobrawl::gui_main_loop()
{
    if (robobrawl::window == nullptr) {
        // std::cout << "AH!" << std::endl;
        // done = true;
        return;
    }

    glfwPollEvents();

    if (glfwWindowShouldClose(robobrawl::window)) {
        done = true;
        return;
    }

    if (glfwGetWindowAttrib(robobrawl::window, GLFW_ICONIFIED) != 0) {
        ImGui_ImplGlfw_Sleep(10);
        return;
    }


    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    for (auto device : robobrawl_devices) {
        gui_draw_selection_menu(device);
        gui_draw_contol_menu(device);
    }

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(robobrawl::window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(robobrawl::window);
    ImGui::EndFrame();

}


//may split these into seperatet files.... 
int robobrawl::gui_draw_selection_menu(Robobrawl_Control* device)
{
    ImGui::Begin("Select System");

    for(auto system_id : known_devices.known_systems_ ){
        // @TODO push style for claimed -----------------
        if(system_id == "") {continue;}
        if(std::find(device->claimed_systems_.begin(), device->claimed_systems_.end(), system_id) != device->claimed_systems_.end()){
            ImGui::PushStyleColor(1, cyan);   
        } else {
            ImGui::PushStyleColor(1, grey);   
        }
        if(ImGui::Button(system_id.c_str(), ImVec2(100, 30))){
            //locate... I think I have to..
            open_control_window_[system_id] = !open_control_window_[system_id];
        }
        ImGui::PopStyleColor();

    }

    ImGui::End();


    return 0;
}

int robobrawl::gui_draw_contol_menu(Robobrawl_Control* device)
{
    for(auto system_open_ : open_control_window_){
    if(system_open_.second){
        SystemId current_system = system_open_.first;
        robobrawl::States current_state = device->states_[current_system];
        
        ImGui::Begin(system_open_.first.c_str());
        {


            /*
                systemName
                -----------------------------------------
                |   Start            |   set Max time   |
                |----------------------------------------
                |   Pause / resume   | STOP             |
                |--------------------------=-------------
                | WIN A              | WIN B            |
                | ---------------------------------------
                | open deives screen | claim system req |
                | ---------------------------------------
                | some random data down here I thikn.   |
                |                                       |
                |                                       |
                -----------------------------------------
            */
            

            // is it started ? is it in a good state to start? then colors~~~~
            if(current_state == robobrawl::States::STATE_BOTH_READY || current_state == robobrawl::States::STATE_COUNTDOWN_3_2_1){
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::green);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, robobrawl::green);                
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::grey);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, robobrawl::grey);

            }
            if(ImGui::Button("Start", ImVec2(200, 100))){
                // try to create a timer, if we alrasy have one no prob, just rejected. 
                device->timer_manager_.create(current_system, std::chrono::seconds(device->system_match_times_[current_system]));
                // now we are sure there is a timer, make it the right one. 
                device->timer_manager_.set_starting_time(current_system, std::chrono::seconds(device->system_match_times_[current_system])); 
                current_state =  robobrawl::States::STATE_COUNTDOWN_3_2_1; 
                device->states_[current_system] = robobrawl::States::STATE_COUNTDOWN_3_2_1;
                device->send_state_change(current_system, current_state, device->system_match_times_[current_system] );

                //start that timer!!!!
                device->timer_manager_.start(current_system);

                std::cout << "Started match on system " << current_system << " with " 
                                << device->system_match_times_[current_system]/60 << ":"
                                << ( device->system_match_times_[current_system]%60 )/10 
                                << device->system_match_times_[current_system]%10 
                                << " time" << std::endl;
            }
            ImGui::PopStyleColor(2);
            ImGui::SameLine();

            // the thing for teh thing of time stuff... yay


            ImGui::InputInt("Set Match Time (sec)", &device->system_match_times_[current_system]);
            
            //check if paused, or if were running, or whatever... 
            if(current_state == robobrawl::States::STATE_RUNNING_MATCH){
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::yellow);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, robobrawl::yellow);      
            } else if (current_state == robobrawl::States::STATE_PAUSED_MATCH){
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::green);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, robobrawl::green);      
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::grey);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, robobrawl::grey);      
            }
            
            if(ImGui::Button("Pause", ImVec2(200, 100))){

                current_state =  robobrawl::States::STATE_COUNTDOWN_3_2_1; 
                device->states_[current_system] = robobrawl::States::STATE_COUNTDOWN_3_2_1;
                device->send_state_change(current_system, current_state, device->system_match_times_[current_system] );
                std::cout << "Started match on system " << current_system << " with " 
                                << device->system_match_times_[current_system]/60 << ":"
                                << ( device->system_match_times_[current_system]%60 )/10 
                                << device->system_match_times_[current_system]%10 
                                << " time" << std::endl;

            }
            ImGui::PopStyleColor(2);
            ImGui::SameLine();
            
            // we can kinda check state, but this will just state = FREAK OUT
            ImGui::Button("STOP AH!", ImVec2(200, 100));

            // make blue, only on timeout I think... ? or running to mabes
            ImGui::Button("Win Blue", ImVec2(200, 100));

            ImGui::SameLine();

            ImGui::Button("Win Orange", ImVec2(200, 100));
            
            // should this be a popoup or a new fucntion with a begin? can I begin in begin?
            ImGui::Button("Open Devices Screen", ImVec2(200, 100)); 

            ImGui::SameLine();

            // only green when we dont have it I guess.. 
            ImGui::Button("Claim This System!", ImVec2(200, 100));

            //other data in text, we could push a new font size I guess.. 

            ImGui::Text("state: %s", robobrawl::state_to_string_map[current_state].c_str());



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
