#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <string>

#include "clock_app/roles/clock.hpp" 

#include <ws281x/ws2811.h>

#define TARGET_FREQ             WS2812_TARGET_FREQ // Default 800kHz
#define DMA                     10
#define GPIO_PIN                18
#define LED_COUNT               96
#define BRIGHTNESS              255 // 0-255


namespace robobrawl{

    //Structures.... yayayyayyayy
    ws2811_led_t led_data_vec[LED_COUNT]; // all off ? idk what this is for realy... ill have to look at how pi libs work.
    ws2811_t ledstring = {
        freq = TARGET_FREQ,
        dmanum = DMA,
        channel[0].gpionum = GPIO_PIN,
        channel[0].count = LED_COUNT,
        channel[0].strip_type = WS2811_STRIP_GRB,
        channel[0].brightness = 255,
        channel[0].leds = led_data_vec
    };


    std::vector<std::string> mapNames = {"dig1", "dig2", "dig3", "dig4", "colonUp", "colonDown", "decimal", "gear1", "gear2"};

    std::map<std::string, std::vector<int>> TEST_MAP = {
        {"dig1",        {20, 5, 8, 14, 17, 23, 11}},
        {"dig2",        {41, 26, 29, 35, 38, 44, 32}},
        {"dig3",        {65, 50, 53, 59, 62, 68, 56}},
        {"dig4",        {86, 71, 74, 80, 83, 89, 77}},
        {"colonUp",     {48}},
        {"colonDown",   {47}},
        {"decimal",     {46}},
        {"gear1",       {0, 1, 2, 3}},
        {"gear2",       {92, 93, 94, 95}},
    };

    std::unordered_map<Colors, std::vector<uint8_t>> color_to_rgb = {
        {Colors::COLOR_BLACK,  {0, 0, 0}},
        {Colors::COLOR_BLUE,   {0, 0, 255}},
        {Colors::COLOR_GREEN,  {0, 255, 0}},
        {Colors::COLOR_ORANGE, {200, 100, 0}},
        {Colors::COLOR_RED,    {255, 0, 0}},
        {Colors::COLOR_WHITE,  {255, 255, 255}},
        {Colors::COLOR_YELLOW, {255, 255, 0}}
    };

    std::unordered_map<int, std::vector<bool>> digit_to_segment_map = {
        {0, {true, true, true, false, true, true, true}}, // top, br, bott ,mid, tr, tl , bl  ??? wtf
        {1, {false, true, false, false, true, false, false}},
        {2, {true, false, true, true, true, false, true}},
        {3, {true, true, true, true, true, false, false}},
        {4, {false, true, false, true, true, true, false}},
        {5, {true, true, true, true, false, true, false}},
        {6, {true, true, true, true, false, true, true}},
        {7, {true, true, false, false, true, false, false}},
        {8, {true, true, true, true, true, true, true}},
        {9, {true, true, true, true, true, true, false}}

    };

    std::vector<Colors> leds(96, Colors::COLOR_BLACK);  // start off


    //------------------------------------------------------------------
    // strip config 

    ws2811_t ledstring = {
        .freq = TARGET_FREQ,
        .dmanum = DMA_CHANNEL,
        .channel = {
            [0] = {
                .gpionum = GPIO_PIN,
                .invert = 0,
                .count = LED_COUNT,
                .strip_type = WS2811_STRIP_GRB,         // WS2812 usually uses GRB order
                .brightness = LED_BRIGHTNESS,
            },
            [1] = {
                .gpionum = 0,                           // Channel 1 unused
                .count = 0,
            }
        }
    };




    // driver headders
    int leds_init();

    std::vector<bool> getSevenSegmentDisplay(int digit);

    int ledShow(); // display / update the whole thing

    void changeNumLed(std::vector<bool> ledVec, std::string digit_name, float seconds, Colors mainColor);

    std::vector<int> formatTime(int sec, int min, int doColon);
    
    void writeSolid(Colors color);
    
    void writeGear(Colors color, bool leftGear);

    void writeTime(Colors color, int minuets, int seconds, int doColon);

    //tree thing, should I thread this bit.........................
    void refresh_clock(Clock* device);

}