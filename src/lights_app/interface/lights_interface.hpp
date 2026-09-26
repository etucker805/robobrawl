#pragma once 

#include <vector>
#include <map>

#include <unordered_map>
#include <string> 

#include "lights_app/roles/lights.hpp"

#include <ws2811.h>


#define NUM_LED_PER_STRAIGHT 56
#define NUM_LED_PER_DIAG 28


namespace robobrawl{

    //Structures
    //71.42mm section btw
    // ~4m strights, 2m diags 
    //      (56)            (28) (probably)
    extern std::string host_corner_color;

    Timer anim_timer;


    ws2811_led_t diag_stort_strip_data[NUM_LED_PER_DIAG];
    ws2811_led_t long_strip_data[NUM_LED_PER_STRAIGHT + NUM_LED_PER_DIAG];
    ws2811_led_t straight_strip_data[NUM_LED_PER_STRAIGHT];

    std::vector<Colors> diag_short_strip(NUM_LED_PER_DIAG, Colors::COLOR_BLACK);
    std::vector<Colors> long_strip(NUM_LED_PER_DIAG + NUM_LED_PER_STRAIGHT, Colors::COLOR_BLACK);
    std::vector<Colors> straight_strip(NUM_LED_PER_STRAIGHT, Colors::COLOR_BLACK);
    

    extern std::unordered_map<Colors, std::vector<int>> color_to_rgbw_map; 

    extern std::vector<std::vector<Colors>*> list_of_strip_lists = {
        &diag_stort_strip,
        &long_strip,
        &straight_strip
    };

    extern std::vector<ws2811_led_t*> list_of_data_lists = {
        diag_stort_strip_data,
        long_strip_data,
        straight_strip_data
    };

    // funct to take strip indx, led indx, return x,y approx
    std::pair<float, float> get_pos_of_led(int strip_index, int led_index);

    // Im Makeing the rest of this not my problem 
    void render_leds();

    void render_strip(int strip_index);
    
    void light_centered_circle(int rad, Colors color);

    void light_solid(Colors color);

    // hmmmmmmmmmmmmmmmm im out of ideas
    // if I  spawn a thread to do the anim....... or I hsould have an anim timer ig. these will be called at 1/10 ms -> 100Hz



    //state Tree 
    void refresh_lights(Lights* device);

    // start() stop() resets it, std::chrono::ms elapsed(), uint64 elapsed_msec()... yeah



}
