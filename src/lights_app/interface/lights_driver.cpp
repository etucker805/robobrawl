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

// also, btw I should thread the calls to each strip, I think... itl take like 9ms per write to all of the strips......

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


   void render_strip(int strip_index){
      auto strip = *list_of_strip_lists[strip_index];
      auto data_strip = list_of_data_lists[strip_index];

      for(int j = 0; j < strip.size(); j++){
         auto led = strip[j];
         uint32_t rgbw = (color_to_rgbw_map[led][0] << 24) +  // r?
                           (color_to_rgbw_map[led][1] << 16) +  //g?
                           (color_to_rgbw_map[led][2] << 8) +   //b?
                           color_to_rgbw_map[led][3];          //w?
         data_strip[j] = rgbw;
      }
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
   }


   void light_centered_circle(int rad, Colors color){
      for( int i = 0; i < 3; i++){
         auto strip_list = list_of_strip_lists[i];
         for(int j = 0; j < strip_list->size(); j++){
            // if get pos.x.y.z**2 <= rad ig
            std::pair<float, float> pos = get_pos_of_led(i, j);
            (*strip_list)[j] = ((pos.first * pos.first + pos.second * pos.second) <= rad ? color : Colors::COLOR_BLACK);
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

   //outside chase clockwise 
   void outside_chase(int chaseres=1){
      if(!anim_timer.is_running()){
         anim_timer.start();
      }

      // now we know anim timer is running
      // total outside size
      int ring_size = 4 * NUM_LED_PER_STRAIGHT;

      //sooo mod timer / some const by the size of the tings turn on those leds...
      int this_num = anim_timer.elapsed_msec() % ring_size;
      for(int i  = 0; i < chaseres; i++){
         // so for every chaser
         int myIndex = this_num + (ring_size/chaseres)*i; // the ring index, blue short end = 0, then go blue short -> blue long -> orange short -> orange long
         if(host_corner_color == "BLUE"){
            if(myIndex < NUM_LED_PER_STRAIGHT){
               // write to short stright 

            } else if (myIndex < NUM_LED_PER_STRAIGHT*2){
               // wright to straight of the long bit
            } 

         } else {


         }


         //timer % ring_size -> whatever index to trun on that + ring / chasers gives the others 
      
      }

   }


   //some kinda corrnwers in tinng 
   // ^ this but also explode



}