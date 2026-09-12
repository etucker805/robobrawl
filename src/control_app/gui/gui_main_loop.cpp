#include "gui.hpp"

ImVec4 robobrawl::purple = ImVec4(0.5, 0, 0.5, 1);
ImVec4 robobrawl::yellow = ImVec4(0.98, 0.98, 0.5, 1);
ImVec4 robobrawl::orange = ImVec4(1, 0.3, 0.03, 1);
ImVec4 robobrawl::white  = ImVec4(1, 1, 1, 1);
ImVec4 robobrawl::black  = ImVec4(0, 0, 0, 1);
ImVec4 robobrawl::cyan   = ImVec4(0.2, 0.8, 0.9, 1 );
ImVec4 robobrawl::grey   = ImVec4(0.6, 0.6, 0.5, 1);
ImVec4 robobrawl::green  = ImVec4(0, 0.9, 0, 1);
ImVec4 robobrawl::red    = ImVec4(0.9, 0, 0, 1);
ImVec4 robobrawl::blue    = ImVec4(0, 0, 0.8, 1);


ImVec4 robobrawl::clear_color       = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
std::map<robobrawl::SystemId, bool> robobrawl::open_control_window_ = {};
std::map<robobrawl::States, std::string> robobrawl::state_to_string_map;


//idl to sting mostly, and otherr message vars

std::map<robobrawl::DeviceRole, std::string> robobrawl::role_to_string_map = {
    {robobrawl::DeviceRole::ROLE_LIGHTS, "Arena / Lighting"},
    {robobrawl::DeviceRole::ROLE_BUTTON, "Button"},
    {robobrawl::DeviceRole::ROLE_CLOCK, "Clock"},
    {robobrawl::DeviceRole::ROLE_CONTROL, "Controler"},
    {robobrawl::DeviceRole::ROLE_UNKNOWN, "Unknown"}
}; 
int robobrawl::current_selected_button_color_index = 0;

std::map< robobrawl::ButtonColor, std::string> robobrawl::button_color_list = {
    {robobrawl::ButtonColor::BUTTON_BLUE, "BUTTON_BLUE" },
    {robobrawl::ButtonColor::BUTTON_ORANGE, "BUTTON_ORANGE" },
    {robobrawl::ButtonColor::BUTTON_UNKNOWN, "BUTTON_UNKNOWN" }
}; 


//global command
robobrawl::SystemId robobrawl::command_new_system;

    //button
robobrawl::ButtonColor robobrawl::command_new_button_color;

    //clock
robobrawl::Colors robobrawl::command_new_display_color;
robobrawl::TimeData robobrawl::command_new_display_time;




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

    ImGui::Separator();
    if(ImGui::Button("All Devices")){
        ImGui::OpenPopup("Device List Window");
    }

    if(ImGui::BeginPopup("Device List Window")){
        gui_draw_device_list(device);
        ImGui::EndPopup();
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
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::grey);

            }
            if(ImGui::Button("Start", ImVec2(200, 100))){
                // try to create a timer, if we alrasy have one no prob, just rejected. 
                device->timer_manager_.create(current_system, std::chrono::seconds(3));
                device->timer_manager_.set_handler([device](const CountdownTimer::Id &id) {
                    device->handle_timer_expired(id);
                    }
                );
                // now we are sure there is a timer, make it the right one. 
                device->timer_manager_.set_expiration_time_ns(current_system, std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::seconds(3)).count() + std::chrono::system_clock::now().time_since_epoch().count()); 
                current_state =  robobrawl::States::STATE_COUNTDOWN_3_2_1; 
                device->states_[current_system] = robobrawl::States::STATE_COUNTDOWN_3_2_1;
                device->send_state_change(current_system, current_state, TimeData(std::chrono::system_clock::now().time_since_epoch().count() + convert_time_to_nsec(device->system_match_times_[current_system])));

                //start that timer!!!!
                device->timer_manager_.start(current_system);

                std::cout << "Started match on system " << current_system << " with " 
                                << device->system_match_times_[current_system].min << ":"
                                << ( device->system_match_times_[current_system].sec%60 )/10 
                                << device->system_match_times_[current_system].sec%10 
                                << " time" << std::endl;
            }
            ImGui::PopStyleColor();
            ImGui::SameLine();

            // the thing for teh thing of time stuff... yay

            ImGui::Text("Minuets: ");
            ImGui::SameLine();            
            ImGui::InputInt("Set Match Time (min)", &device->system_match_times_[current_system].min);
            ImGui::SetCursorPos(ImVec2(217, 70));
            ImGui::Text("Seconds: ");
            ImGui::SameLine();            
            ImGui::InputInt("Set Match Time (sec)", &device->system_match_times_[current_system].sec);

            ImGui::SetCursorPos(ImVec2(8, 138));

            //check if paused, or if were running, or whatever... 
            if(current_state == robobrawl::States::STATE_RUNNING_MATCH){
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::yellow);
            } else if (current_state == robobrawl::States::STATE_PAUSED_MATCH){
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::green);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::grey);
            }
            
            if(ImGui::Button("Pause", ImVec2(200, 100))){
                // NOTE: I check here for if the timer is running for some reason, or if we think it should be.
                if(device->timer_manager_.is_running(current_system) || device->states_[current_system] == robobrawl::States::STATE_RUNNING_MATCH){
                    // running, so pause
                    device->timer_manager_.pause(current_system);
                    current_state =  robobrawl::States::STATE_PAUSED_MATCH; 
                    device->states_[current_system] = robobrawl::States::STATE_PAUSED_MATCH;
                    device->send_state_change(current_system, current_state, device->timer_manager_.get_remaining_time_sec(current_system) );
                    std::cout << "Paused match on system " << current_system << " with " 
                                    << device->timer_manager_.get_remaining_time_sec(current_system)/60 << ":"
                                    << ( device->timer_manager_.get_remaining_time_sec(current_system)%60 )/10 
                                    << device->timer_manager_.get_remaining_time_sec(current_system)%10 
                                    << " time" << std::endl;
                } else if ( device->states_[current_system] == robobrawl::States::STATE_PAUSED_MATCH || 
                            device->states_[current_system] == robobrawl::States::STATE_MATCH_EMERGENCY_STOP){
                    // running, so pause
                    device->timer_manager_.resume(current_system);
                    current_state =  robobrawl::States::STATE_RUNNING_MATCH; 
                    device->states_[current_system] = robobrawl::States::STATE_RUNNING_MATCH;
                    device->send_state_change(current_system, current_state, device->timer_manager_.get_remaining_time_sec(current_system) );
                    std::cout << "Resume match on system " << current_system << " with " 
                                    << device->timer_manager_.get_remaining_time_sec(current_system)/60 << ":"
                                    << ( device->timer_manager_.get_remaining_time_sec(current_system)%60 )/10 
                                    << device->timer_manager_.get_remaining_time_sec(current_system)%10 
                                    << " time" << std::endl;
                } else {
                    std::cout << "tried to pause resume on not runnning or stoped timer states, invalid?" << std::endl;
                }

            }
            ImGui::PopStyleColor();
            ImGui::SameLine();
            
            // we can kinda check state, but this will just state = FREAK OUT

            if(current_state == robobrawl::States::STATE_MATCH_EMERGENCY_STOP){
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::green);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::red);
            }

            if(ImGui::Button("STOP AH!", ImVec2(200, 100))){
                current_state =  robobrawl::States::STATE_MATCH_EMERGENCY_STOP; 
                device->states_[current_system] = robobrawl::States::STATE_MATCH_EMERGENCY_STOP;
                device->timer_manager_.pause(current_system); // just in case, cause why not.
                device->send_state_change(current_system, current_state, convert_time_to_nsec(device->system_match_times_[current_system]) );
                std::cout << "Stoped Match on system: " << current_system << std::endl;
            }
            ImGui::PopStyleColor(1);


            // make blue, only on timeout I think... ? or running to mabes
            
            if(current_state == robobrawl::States::STATE_MATCH_TIME_EXPIRE){
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::blue);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::grey);
            }
            
            if(ImGui::Button("Win Blue", ImVec2(200, 100))){
                current_state =  robobrawl::States::STATE_WIN_BLUE;
                device->states_[current_system] =  robobrawl::States::STATE_WIN_BLUE;
                device->timer_manager_.pause(current_system);
                device->send_state_change(current_system, current_state, device->timer_manager_.get_remaining_time_sec(current_system));
            }
            ImGui::PopStyleColor(1);
            ImGui::SameLine();


            // Win orange button yayyayayay just blue biut its orange! yay.
            if(current_state == robobrawl::States::STATE_MATCH_TIME_EXPIRE){
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::orange);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::grey);
            }
            if(ImGui::Button("Win Orange", ImVec2(200, 100))){
                current_state =  robobrawl::States::STATE_WIN_ORANGE;
                device->states_[current_system] =  robobrawl::States::STATE_WIN_ORANGE;
                device->timer_manager_.pause(current_system);
                device->send_state_change(current_system, current_state, device->timer_manager_.get_remaining_time_sec(current_system));
            }
            ImGui::PopStyleColor(1);
            
            // should this be a popoup or a new fucntion with a begin? can I begin in begin?
            
            ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::grey);
            if(ImGui::Button("Set System State", ImVec2(200, 100))){
                ImGui::OpenPopup(current_system.c_str());
            } 
            ImGui::PopStyleColor(1);

            if(ImGui::BeginPopup(current_system.c_str())){
                    

                if(ImGui::Button("Idle Off")){
                    current_state = States::STATE_IDLE_OFF;
                    device->update_state_of_system(current_system, current_state, 0);
                }
                 
                if(ImGui::Button("Idle On")){
                    current_state = States::STATE_IDLE_ON;
                    device->update_state_of_system(current_system, current_state, 0);
                }

                if(ImGui::Button("Wait For Ready")){
                    current_state = States::STATE_READY_WAIT;
                    device->update_state_of_system(current_system, current_state, 0);
                }

                //others I want to allow this transition too...

                if(ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
            
                ImGui::EndPopup();    
            }



            ImGui::SameLine();
            // only green when we dont have it I guess.. 
            if(std::find(device->claimed_systems_.begin(), device->claimed_systems_.end(), current_system) == device->claimed_systems_.end()){
                // dont have it, uh makes it green I guess.. 
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::green);
            } else {
                ImGui::PushStyleColor(ImGuiCol_Button, robobrawl::grey);
            }


            if(ImGui::Button("Claim This System!", ImVec2(200, 100))){
                // note: I dont think anything actualy cares about this at present... 
                device->send_claim_system(current_system);
            }

            ImGui::PopStyleColor();

            //other data in text, we could push a new font size I guess.. 
            ImGui::Separator();

            ImGui::Text("state: %s", device->get_state_string(current_state).c_str());

            ImGui::Text("Time Remaining: %d", device->timer_manager_.get_remaining_time_sec(current_system));


        }
        ImGui::End();
    }
    }


    return 0;
}


int robobrawl::gui_draw_device_list(Robobrawl_Control* device){
    for(auto current_device_id : known_devices.known_device_ids()){

        // do device id, type, current system, whatever else Ive got on info
        // then send command buttons
        ImGui::Text("Device ID: ");
        ImGui::SameLine();
        ImGui::TextUnformatted( current_device_id.c_str() );

        ImGui::Text("Device Role: ");
        ImGui::SameLine();
        ImGui::TextUnformatted(role_to_string_map[known_devices.get_device_role(current_device_id)].c_str()); // need to get name of role, rather then number... 
        
        ImGui::Text("System: ");
        ImGui::SameLine();
        ImGui::TextUnformatted(known_devices.get_device_info(current_device_id).system().c_str());
        ImGui::PushID(current_device_id.c_str());
        if(ImGui::Button("Send Command")){
            ImGui::OpenPopup(current_device_id.c_str());
        }
        if(ImGui::BeginPopup(current_device_id.c_str())){
            ImGui::PushStyleColor(ImGuiCol_FrameBg, grey);
            switch (known_devices.get_device_role(current_device_id))
            {
            case robobrawl::DeviceRole::ROLE_LIGHTS:
                /* all of the fields Ill need to fill out, and the send button. so ill need to make all of those vars I guess */
                ImGui::InputText("New SystemId", &command_new_system);
                if(ImGui::Button("Send Command")){
                    device->send_lights_command(current_device_id, command_new_system);
                }
                break;

            case robobrawl::DeviceRole::ROLE_BUTTON:
            ImGui::InputText("New SystemId", &command_new_system);

                // the idea is that this is reuseable for all of them, and is not dependent on the llist being in order, probably. 
                if(ImGui::BeginCombo("New Button Color", button_color_list.at((robobrawl::ButtonColor)current_selected_button_color_index).c_str())){
                    for(int i = 0; i < button_color_list.size(); i++){
                        const bool is_selected = i == current_selected_button_color_index; 
                        if(ImGui::Selectable(button_color_list[(robobrawl::ButtonColor)i].c_str())){
                            current_selected_button_color_index = i;
                        }
                        if(is_selected){
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }


                if(ImGui::Button("send Command")){
                    std::cout << "IM TRYING" << std::endl;
                    device->send_button_command(current_device_id, command_new_system, (robobrawl::ButtonColor)current_selected_button_color_index);
                }

                break;

            case robobrawl::DeviceRole::ROLE_CLOCK:

                // time is int input
                // color is a thing again, oh boy. 


                break;

            default:
                break;
            }

            if(ImGui::Button("Close Send Popup")){
                ImGui::CloseCurrentPopup();
            }
            ImGui::PopStyleColor(1);

            ImGui::EndPopup();

        }

        ImGui::PopID();

        ImGui::Separator();
    }

    if(ImGui::Button("Close")){
        ImGui::CloseCurrentPopup();
    }
    return 0;
}



int robobrawl::gui_draw_timer()
{
    return 0;
}
