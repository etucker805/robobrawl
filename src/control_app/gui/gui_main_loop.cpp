#include "gui.hpp"

void robobrawl::gui_main_loop()
{
    if(glfwWindowShouldClose(window)){
        gui_cleanup();
        done = 1; 
    }

    gui_draw_selection_menu();


}


//may split these into seperatet files.... 
int robobrawl::gui_draw_selection_menu()
{

}

int robobrawl::gui_draw_contol_menu()
{

}

int robobrawl::gui_draw_timer()
{

}
