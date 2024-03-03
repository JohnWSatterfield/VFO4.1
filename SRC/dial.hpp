/* 
  Digital VFO with Rotation Dial
 
    File:   dial.hpp
    Author: JF3HZB / T.UEBO
 
    Created on July 1, 2023
*/

#ifndef DIAL_H
#define	DIAL_H
#include"config.h"

#if DISP_SIZE == CUSTOM_DISP 
  #include "ST7789_170x320.hpp"
#endif

#if DISP_SIZE == LARGE_DISP
  #include "ST7789_240x320.hpp"
#endif

#if DISP_SIZE == SMALL_DISP
  #include "ST7735S_128x160.hpp"
#endif

//#include "ILI9341_240x320.hpp"
//#include "SSD1331_64x96.hpp"
//#include "ST7789_240x240.hpp"
//#include "ST7789_135x240.hpp"
//#include "ST7735S_80x160.hpp"

extern LGFX lcd;
extern LGFX_Sprite sp;
extern LGFX_Sprite sprites[2];
extern bool flip;
extern int sprite_height; 

void LCD_setup(void);

class DIAL
{
  private:
  float resoSub; //[deg]
  float resoMain; //[deg]
  int32_t TickResoMain = freq_tick_main;
  int32_t TickResoSub = freq_tick_sub;
  int hoff = center_offset;                  //move dial left or right from center position
  int voff = top_position;                   //move dial 
  float radiusMain;
  float radiusSub;

  public:
  float tnMain = -TNCL_main;
  float tnSub = -TNCL_sub;
  float tickPitchMain = tick_pitch_main;
  float tickPitchSub = tick_pitch_sub; 

  float tickWSub = tw_sub1;
  float tick5WSub = tw_sub5;
  float tick10WSub = tw_sub10;

  float tickLSub = tl_sub1;
  float tick5LSub = tl_sub5;
  float tick10LSub = tl_sub10;

  float tickWMain = tw_main1;
  float tick5WMain = tw_main5;
  float tick10WMain = tw_main10;

  float tickLMain = tl_main1;
  float tick5LMain = tl_main5;
  float tick10LMain = tl_main10;

  float LenPointer = DP_len;
  float WidthPointer = DP_width;

  DIAL()
  {
    #ifdef MAINDIAL_IS_OUTSIDE
    radiusMain = dial_radius;
    radiusSub  = dial_radius - dial_space;
    #else if
    radiusSub  = dial_radius;
    radiusMain = dial_radius - dial_space;
    #endif

    resoMain =  (180.0f/M_PI)*tickPitchMain/(float)radiusMain;
    resoSub =  (180.0f/M_PI)*tickPitchSub/(float)radiusSub;

  }

  void draw(int32_t freq, int yoff);  
};


#endif	/* DIAL_H */
