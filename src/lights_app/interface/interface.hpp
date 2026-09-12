#pragma once 

#include <vector>
#include <map>

#include <unordered_map>
#include <string> 

#include "robobrawl/device/robobrawl_lights.hpp"


namespace robobrawl{

    //Structures
    //71.42mm section btw
    // ~4m strights, 2m diags 
    //      (56)            (28) (probably)
    std::vector<Colors> diag_stort_strip( 28, Colors::COLOR_BLACK);
    std::vector<Colors> long_strip( 56+28, Colors::COLOR_BLACK);
    std::vector<Colors> straight_strip( 56, Colors::COLOR_BLACK);

    std::unordered_map<Colors, std::vector<int>> color_to_rgbw_map = {
      
    };

    std::vector<std::vector<Colors>*> list_of_strip_lists = {&diag_stort_strip, &long_strip, &straight_strip};

    // funct to take strip indx, led indx, return x,y approx
    std::pair<float, float> get_pos_of_led(int strip_index, int led_index);









}
