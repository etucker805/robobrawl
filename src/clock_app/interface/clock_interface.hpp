#pragma once

#include <vector>
#include <unordered_map>
#include <map>
#include <string>

#include "clock_app/roles/clock.hpp" 

#include <ws2811/ws2811.h>

#define TARGET_FREQ             WS2811_TARGET_FREQ // Default 800kHz
#define DMA_CHANNEL                     10
#define GPIO_PIN                18
#define LED_COUNT               96
#define LED_BRIGHTNESS              255 // 0-255


namespace robobrawl{

    //Structures.... yayayyayyayy
    extern ws2811_led_t led_data_vec[LED_COUNT]; // all off ? idk what this is for realy... ill have to look at how pi libs work.

    //------------------------------------------------------------------
    // strip config 

    extern ws2811_t ledstring;

    extern std::vector<std::string> mapNames ;

    extern std::map<std::string, std::vector<int>> TEST_MAP ;

    extern std::unordered_map<Colors, std::vector<uint8_t>> color_to_rgb;

    extern std::unordered_map<int, std::vector<bool>> digit_to_segment_map;

    extern std::vector<Colors> leds;  // start off




    // driver headders
    int leds_init();

    std::vector<bool> getSevenSegmentDisplay(int digit);

    int ledShow(); // display / update the whole thing

    void changeNumLed(const std::vector<bool>& ledVec, const std::string& digit_name, Colors mainColor);

    std::vector<int> formatTime(int sec, int min, int doColon);
    
    void writeSolid(Colors color);
    
    void writeGear(Colors color, bool leftGear);

    void writeTime(Colors color, int minuets, int seconds, int doColon);

    //tree thing, should I thread this bit.........................
    void refresh_clock(Clock* device);

}