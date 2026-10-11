// implemet all the led functions
#include "clock_app/interface/clock_interface.hpp"


ws2811_led_t robobrawl::led_data_vec[LED_COUNT]; // all off ? idk what this is for realy... ill have to look at how pi libs work.

//------------------------------------------------------------------
// strip config 

ws2811_t robobrawl::ledstring = {
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

std::vector<std::string> robobrawl::mapNames = {"dig1", "dig2", "dig3", "dig4", "colonUp", "colonDown", "decimal", "gear1", "gear2"};

std::map<std::string, std::vector<int>> robobrawl::TEST_MAP = {
    {"dig1",        {20, 5, 8, 14, 17, 23, 11}},
    {"dig2",        {41, 26, 29, 35, 38, 44, 32}},
    {"dig3",        {65, 50, 53, 59, 62, 68, 56}},
    {"dig4",        {86, 71, 74, 80, 83, 89, 77}},
    {"colonUp",     {48}},
    {"colonDown",   {47}},
    {"decimal",     {46}},
    {"gear1",       {1, 2, 3, 4}},
    {"gear2",       {92, 93, 94, 95}},
};

std::unordered_map<robobrawl::Colors, std::vector<uint8_t>> robobrawl::color_to_rgb = {
    {robobrawl::Colors::COLOR_BLACK,  {0, 0, 0}},
    {robobrawl::Colors::COLOR_BLUE,   {0, 0, 255}},
    {robobrawl::Colors::COLOR_GREEN,  {0, 255, 0}},
    {robobrawl::Colors::COLOR_ORANGE, {200, 100, 0}},
    {robobrawl::Colors::COLOR_RED,    {255, 0, 0}},
    {robobrawl::Colors::COLOR_WHITE,  {255, 255, 255}},
    {robobrawl::Colors::COLOR_YELLOW, {255, 255, 0}}
};

std::unordered_map<int, std::vector<bool>> robobrawl::digit_to_segment_map = {
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

std::vector<robobrawl::Colors> robobrawl::leds(96, robobrawl::Colors::COLOR_BLACK);  // start off

namespace robobrawl{


    int leds_init(){
        if (ws2811_init(&ledstring) != WS2811_SUCCESS){
            std::cerr << "LED init failed" << std::endl;
            return 1;
        }
        return 0;
    }



    std::vector<bool> getSevenSegmentDisplay(int digit){ // this is a nessisarty funtion trust me. 
        return digit_to_segment_map[digit];
    }



    int ledShow(){ // render 
        // display / update the whole thing
        // idk how to do this yet... 
        if(ws2811_render(&ledstring) != WS2811_SUCCESS){
            std::cerr << "LED render fail" << std::endl;
            return 1;
        }
        return 0;

    } 

    void changeNumLed(std::vector<bool> ledVec, std::string digit_name, float seconds, Colors mainColor){
         for (int i = 0; i < 7; i++)
        {
            if (ledVec[i])
            {
            leds[TEST_MAP[digit_name][i] - 1] = mainColor;
            leds[TEST_MAP[digit_name][i]] = mainColor;
            leds[TEST_MAP[digit_name][i] + 1] = mainColor;
            }
            else
            {
            leds[TEST_MAP[digit_name][i] - 1] = Colors::COLOR_BLACK;
            leds[TEST_MAP[digit_name][i]] = Colors::COLOR_BLACK;
            leds[TEST_MAP[digit_name][i] + 1] = Colors::COLOR_BLACK;
            }
            // either write a func to do the set _ pixel stuff or find a way to do it here? 
        }
        
        ledShow();
    }

    std::vector<int> formatTime(int sec, int min, int doColon){
        std::vector<int> ret(5, 0);
        ret[0] = min/10;
        ret[1] = min%10;
        ret[2] = doColon ? 1 : 0;
        ret[3] = sec/10;
        ret[4] = sec%10;

        return ret;
    }

    void writeSolid(Colors color){
        auto colorVals = color_to_rgb[color];

        for (std::string name : mapNames)
        {
            if (name != "gear1" && name != "gear2" && name != "colonUp" && name != "colonDown" && name != "decimal")
            { // let the grars be probably...

            for (auto num : TEST_MAP[name])
            {
                // ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, num - 1, colorVals[0], colorVals[1], colorVals[2]));
                // ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, num, colorVals[0], colorVals[1], colorVals[2]));
                // ESP_ERROR_CHECK(led_strip_set_pixel(led_strip, num + 1, colorVals[0], colorVals[1], colorVals[2]));
                ledstring.channel[0].leds[num-1] = ((uint32_t)colorVals[1]) << 16 +
                                                   ((uint32_t)colorVals[0]) << 8 + 
                                                   ((uint32_t)colorVals[2]);
                ledstring.channel[0].leds[num] =   ((uint32_t)colorVals[1]) << 16 +
                                                   ((uint32_t)colorVals[0]) << 8 + 
                                                   ((uint32_t)colorVals[2]);
                ledstring.channel[0].leds[num+1] = ((uint32_t)colorVals[1]) << 16 +
                                                   ((uint32_t)colorVals[0]) << 8 + 
                                                   ((uint32_t)colorVals[2]);
                

            }
            }
        }

        ledShow();

    }

    void writeGear(Colors color, bool leftGear){
        auto colorVals = color_to_rgb[color];
        std::string gear = "gear1"; // no clue witch one is left / right, or blue / orange.
        if (leftGear)
        {
            gear = "gear2";
        }
        for (auto num : TEST_MAP[gear])
        {
            
            // led_strip_set_pixel(led_strip, num, colorVals[0], colorVals[1], colorVals[2]);
            ledstring.channel[0].leds[num] = ((uint32_t)colorVals[1]) << 16 +
                                             ((uint32_t)colorVals[0]) << 8 + 
                                             ((uint32_t)colorVals[2]);

        }

        ledShow();

    }

    void writeTime(Colors color, int minuets, int seconds, int doColon){
        std::vector<int> vecTime = formatTime(seconds, minuets, doColon); 

        std::vector<bool> num1leds = getSevenSegmentDisplay(vecTime[0]); 
        std::vector<bool> num2leds = getSevenSegmentDisplay(vecTime[1]); 
        std::vector<bool> num3leds = getSevenSegmentDisplay(vecTime[3]); 
        std::vector<bool> num4leds = getSevenSegmentDisplay(vecTime[4]); 


        changeNumLed(num1leds, "dig4", seconds, color); //sec added for color
        changeNumLed(num2leds, "dig3", seconds, color);
        changeNumLed(num3leds, "dig2", seconds, color);
        changeNumLed(num4leds, "dig1", seconds, color);
        ledShow();

    }


}

