/*
lights layoput: 
                  (0,0) cause vga
                    O --------------*
    orange corrner->|\                /|
                    |  \            /  |
                    |    \        /    |
                    |      \    /      |
                    |                  |
                    |                  |
                    |      /     \     |
                    |    /         \   |
                    |  /             \ |
                    |/                \| <- blue corner. 
                       *---------------O (100,100 )

*/

// -- need a runtime? flag for witch corrner this is.
// I may need to set up this whole thing in our apartment to test it, so I should probablu make this acturlay good and modualr code stuff
// wait wut if I made it all math, so like < y= -x or sething, then every lid gets a point in space, ooh hte computaion for above/below is hard tho right? no! just plug in the points.
// Id have to do a comp for every led compute thing, but ehhhhh
// its appealing I suppose... Id need a way to write in the functions, or is that just in the case state is this then if in this geometry be blue or womething? could be cool .
// id have to set up where the diagonals are, cause edges are easy, Id need to find how many are in a side, so counting.
// I think thast what ill do
// or should I make someone else (anuva!) do it cauase math and me ont jibe. 

//56 straight, 28 diag, probably 

#include "lights_interface.hpp"

#include <cmath>

namespace robobrawl{
   
   // diag only, long, striaght
   std::pair<float, float> get_pos_of_led(int strip_index, int led_index){
      std::pair<float, float> pos;
      switch (strip_index)
      {
      case 0:
         pos.first =  std::sqrt(2)  * (float)led_index; 
         pos.second = std::sqrt(2) * (float)led_index; 

         break;
      
      case 1:

         //harnd one haaaaaaaaaaaaaa
         if(led_index <= NUM_LED_PER_STRAIGHT){
            // just a stright
            pos.first = 0;
            pos.second = (float)led_index * (NUM_LED_PER_STRAIGHT / 100);
         
         } else {
            //
            pos.first  = (std::sqrt(2) * (float)(led_index-NUM_LED_PER_STRAIGHT));
            pos.second = 100 - (std::sqrt(2) * (float)(led_index-NUM_LED_PER_STRAIGHT));
         
         }
         break;

      case 2:
         pos.first = (NUM_LED_PER_STRAIGHT / 100 ) * (float)led_index;
         break;
      
      default:
         break;
      }

      if(host_corner_color == "BLUE"){
         pos.first = 100 - pos.first;
         pos.second = 100 - pos.second;
      }

      return pos;

   }

   void render_leds(){
      for(int i = 0; i < 3; i++){
         auto strip = *list_of_strip_lists[i];
         auto data_strip = list_of_data_lists[i];

         for(int j = 0; j < strip.size(); j++){
            auto led = strip[j];
            uint32_t rgbw = (color_to_rgbw_map[led][0] << 24) +  // r?
                            (color_to_rgbw_map[led][1] << 16) +  //g?
                            (color_to_rgbw_map[led][2] << 8) +   //b?
                             color_to_rgbw_map[led][3];          //w?
            data_strip[j] = rgbw;
         }
      }

      // whtever the render call is. I could try to make this better, only call all this if I have a channge... but Im fast as fk boiiiii so who cares
   
   }


   void light_centered_circle(int rad, Colors color){
      for( int i = 0; i < 3; i++){
         auto strip_list = list_of_strip_lists[i];
         for(auto led : list_of_strip_lists){

         }
      }
   }

   void light_solid(Colors color){
      for(auto  strip : list_of_strip_lists){
         for(auto led : *strip){
            led = color;
         }
      }
   }

}