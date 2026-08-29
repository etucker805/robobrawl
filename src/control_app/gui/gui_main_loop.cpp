#include "gui.hpp"

void robobrawl::gui_main_loop()
{
do{

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

    gui_draw_selection_menu();

    ImGui::Begin("testing");
    {





    }
    ImGui::End();


    ImGui::EndFrame();

} while(0);


}


//may split these into seperatet files.... 
int robobrawl::gui_draw_selection_menu()
{
    return 0;
}

int robobrawl::gui_draw_contol_menu()
{
    return 0;
}

int robobrawl::gui_draw_timer()
{
    return 0;
}
