#include "lights_interface.hpp"


/*
for refrence: 

enum Colors{
    COLOR_GREEN,
    COLOR_ORANGE,
    COLOR_BLUE,
    COLOR_YELLOW,
    COLOR_RED,
    COLOR_BLACK, // off
    COLOR_WHITE,
};



enum States{ // may need to add transition sttas, but I dotn think so atm
    STATE_IDLE_ON, // just white lights, buttons doing something, ect 
    STATE_IDLE_OFF, // everything off
    // STATE_IDLE_ANIMS, 
    STATE_READY_WAIT, // go into dark arena, ready buttons up
    // STATE_BLUE_READY_QUESTION, // valid thing to expand to I think
    STATE_BLUE_READY, // blue flash, blue button goes green, clock blue gear.
    STATE_ORANGE_READY, // ^
    STATE_BOTH_READY, // hold at orange / blue color I think, or we go into some animation before 3-2-1
    STATE_COUNTDOWN_3_2_1, // either do white arena, or some kind of anim on count.
    STATE_RUNNING_MATCH,   // white lights, tapout/unstick buttons up, yeha
    STATE_PAUSED_MATCH,     // lights yelow, indication on button, clock paused
    STATE_UNSTICK_BLUE,     // lights stay white? or do I auto pause? gussy question I think.
    STATE_UNSTICK_ORANGE,   // ^
    STATE_TAPOUT_ORANGE,    // stop clock, winner lights color, might do some idle anim or something.
    STATE_TAPOUT_BLUE,      //^
    STATE_WIN_ORANGE,       // go to win staet, orange lights, lorage buttons, popup on the gui 
    STATE_WIN_BLUE,         // ^
    STATE_MATCH_TIME_EXPIRE,   // red lights, clock on 0, buttons on 'times up' or somehting 
    STATE_MATCH_EMERGENCY_STOP, // everything red, clock paused tho
    STATE_INDIVIDUAL_OVERRIDE, // only state where individual color commands are accepted. 
    CONTROL_FLAG_LAST
};



*/



namespace robobrawl{

    std::vector<Colors> diag_stort_strip( NUM_LED_PER_DIAG, Colors::COLOR_BLACK);
    std::vector<Colors> long_strip( NUM_LED_PER_DIAG + NUM_LED_PER_STRAIGHT, Colors::COLOR_BLACK);
    std::vector<Colors> straight_strip( NUM_LED_PER_STRAIGHT, Colors::COLOR_BLACK);

    std::unordered_map<Colors, std::vector<int>> color_to_rgbw_map = {
        {Colors::COLOR_BLACK, {0, 0, 0, 0}} //ect....
    };

    std::vector<std::vector<Colors>*> list_of_strip_lists = {&diag_stort_strip, &long_strip, &straight_strip};

    // also std::pair<float, float> get_pos_of_led(int strip_index, int led_index);
    // strip index id the list_of_strip_lists index, returns pair of [0,100] vals, x,y
    // 0,0 is  orange corrner, 100,100 is blue corner. 

    // this error just does not exist
    void refresh_lights(Lights* device){

        switch (device->current_state)
        {
        case States::STATE_IDLE_OFF:
            //off
            ///call function that I dont know about right now... 
            light_solid(Colors::COLOR_BLACK);
            break;
        case States::STATE_IDLE_ON:
        case States::STATE_RUNNING_MATCH:
            // just on states
            light_solid(Colors::COLOR_WHITE);
            break;

        case States::STATE_MATCH_EMERGENCY_STOP:
        case States::STATE_MATCH_TIME_EXPIRE:
            //reds
            light_solid(Colors::COLOR_RED);
            break;
        
        case States::STATE_PAUSED_MATCH:
        case States::STATE_UNSTICK_BLUE:
        case States::STATE_UNSTICK_ORANGE:
            // yellow stops
            light_solid(Colors::COLOR_YELLOW);
            break;

        case States::STATE_WIN_BLUE:
        case States::STATE_TAPOUT_ORANGE:
            // blue wins 
            light_solid(Colors::COLOR_BLUE);
            break;

        case States::STATE_WIN_ORANGE:
        case States::STATE_TAPOUT_BLUE:
            //orange wins
            light_solid(Colors::COLOR_ORANGE);
            break;

        // I kinda think the rest are anims that I dont quite have.....

        
        default:
            // imma make this go off i guess
            light_solid(Colors::COLOR_BLACK);
            break;
        }

        render_leds();
    }



    // make a bunch of functions to do the fnacy colors pls.
    // notes on some of the things we want to do 
    // 3-2-1: 
    //      do like a cenerlight-add a ring - whhole lights if you know what I mean
    // idle on should just be whie
    // all of the running staes should be white
    // pause / unstick should be yellow 
    // when someone wins (sin / tapbout) go the color of the winner 
    // red on emergency and timeout
    //I think thast all I got.
    // you may have to store a last_state thing, state will just change without you knowing here.
    //if you know when it changes could do cool anims I guess... that seems like alot ofo work tho.
    // also device->timer_.time_remaining_sec() & time_remaining_ms and stuff is there. I could give you that class, witch is actual like 5 files... haaaaaaaaaaa    

}
