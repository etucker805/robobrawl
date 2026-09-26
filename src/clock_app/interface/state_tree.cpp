// swithc on state, call fucntions in clock_driver to output our thing. yeah.
#include "clock_interface.hpp"

#define BLUE_GEAR_SIDE 1
#define ORANGE_GEAR_SIDE 0


namespace robobrawl{


    // some kninda update state, pass in device
    void refresh_clock(Clock* device){

        // what do I do on what states. 
        switch (device->current_state)
        {
        case States::STATE_IDLE_OFF:
        case States::STATE_IDLE_ON:
            // clock off
            writeSolid(Colors::COLOR_BLACK);
            writeGear(Colors::COLOR_BLACK, BLUE_GEAR_SIDE);
            writeGear(Colors::COLOR_BLACK, ORANGE_GEAR_SIDE);
            break;

        case States::STATE_READY_WAIT: // this is also off. is that what I want? 
            writeSolid(Colors::COLOR_BLACK);
            writeGear(Colors::COLOR_BLACK, BLUE_GEAR_SIDE);
            writeGear(Colors::COLOR_BLACK, ORANGE_GEAR_SIDE);
            break;

        case States::STATE_BLUE_READY:
            writeGear(Colors::COLOR_BLUE, BLUE_GEAR_SIDE); // blue is left I so decree
            break;
        
        case States::STATE_ORANGE_READY:
            writeGear(Colors::COLOR_ORANGE, ORANGE_GEAR_SIDE);
            break;

        case States::STATE_BOTH_READY:
            writeGear(Colors::COLOR_BLUE, BLUE_GEAR_SIDE); 
            writeGear(Colors::COLOR_ORANGE, ORANGE_GEAR_SIDE);
            break;


        case States::STATE_COUNTDOWN_3_2_1: // this is nessisary. I dont believe in default. 
        case States::STATE_RUNNING_MATCH:
        case States::STATE_PAUSED_MATCH:
        case States::STATE_MATCH_EMERGENCY_STOP:
        case States::STATE_MATCH_TIME_EXPIRE:
        case States::STATE_TAPOUT_BLUE:
        case States::STATE_UNSTICK_BLUE:
        case States::STATE_WIN_BLUE:
        case States::STATE_TAPOUT_ORANGE:
        case States::STATE_UNSTICK_ORANGE:
        case States::STATE_WIN_ORANGE:
            // same display the time, just color changes, ill do that in here
            // we think that time is good, so poll the timer and display that. 
            int64_t time_remaining = device->timer_.remaining_time_sec();
            if(time_remaining < 0) time_remaining = 0;

            Colors clock_color;
            // @TODO determine color
            if( device->current_state == States::STATE_COUNTDOWN_3_2_1 ){
                    clock_color = Colors::COLOR_WHITE;
            } else if(device->current_state == States::STATE_RUNNING_MATCH ) {
                // time - based color, could make smoolth transition thing later. 
                clock_color = Colors::COLOR_WHITE;
            } else if(device->current_state == States::STATE_UNSTICK_BLUE ||
                    device->current_state == States::STATE_UNSTICK_ORANGE ||
                    device->current_state == States::STATE_PAUSED_MATCH) {
                        clock_color = Colors::COLOR_YELLOW;
            } else if(device->current_state == States::STATE_MATCH_EMERGENCY_STOP||
                    device->current_state == States::STATE_MATCH_TIME_EXPIRE) {
                        clock_color = Colors::COLOR_RED;
            } else if(device->current_state == States::STATE_WIN_BLUE||
                    device->current_state == States::STATE_TAPOUT_ORANGE) {
                        clock_color = Colors::COLOR_BLUE;
            } else if(device->current_state == States::STATE_WIN_ORANGE || 
                    device->current_state == States::STATE_TAPOUT_BLUE){
                        clock_color = Colors::COLOR_ORANGE;
            } else {
                clock_color = Colors::COLOR_WHITE; // in case I messed any.
            }
            
            
            if(time_remaining >= 60){
                writeTime(Colors::COLOR_WHITE, time_remaining / 60, time_remaining % 60, true);
            }
            else{
                int64_t time_remaining_ms = device->timer_.remaining_time_ms();
                if(time_remaining_ms < 0) time_remaining_ms = 0;
                writeTime(Colors::COLOR_YELLOW, time_remaining_ms / 1000, time_remaining_ms/10, false);
            }

            break;
                
        case States::STATE_INDIVIDUAL_OVERRIDE:
            // I gott figure out how to make this do htis thing right... haaaaa
            break;
        default:
            // uh what.
            break;
        }

    }

}


        