/* 
  Digital VFO with Rotation Dial
 
    File:   dial.cpp
    Author: JF3HZB / T.UEBO
 
    Created on July 1, 2023
*/

#include "dial.hpp"

#define TFT_BLACK       CL_DIAL_BG      /*   0,   0,   0 */

void LCD_setup(void)                                               //set up the lcd display
{
  lcd.init();                                                      //initialize the instance of lcd
  lcd.setColorDepth(16);
  //if (lcd.width() < lcd.height()) { lcd.setRotation(lcd.getRotation()^1); } //code to auto rotate screen
  //lcd.setRotation(1);                                            //rotates the screen 90 degrees

  for (int i = 0; i < 2; i++)                                      //determine if enough memory to display in 1 or 2 writes to screen
  {
    sprites[i].setColorDepth(lcd.getColorDepth());                 //set color depth for each half of display
    sprites[i].setFont(&fonts::Font2);                             //set font to "Font 2"
    sprites[i].setTextDatum(textdatum_t::top_right);               //where the screen data is calculated from
  }

  int div = 1;                                                     //create and set divisor I to 1
  for (;;)                                                         //endless loop (needs break to leave)
  {
    sprite_height = (lcd.height() + div - 1) / div;                //first pass try full width of screen
    bool fail = false;                                             //create and set flag fail to false
    for (int i = 0; !fail && i < 2; ++i)                           //make max of 3 passes through loop
    {
      fail = !sprites[i].createSprite(lcd.width(), sprite_height); //check MC if enough memory to display screen
    }
    if (!fail) break;                                              //if memory sufficient leave loop
    for (int i = 0; i < 2; ++i)                                    //pass twice through loop
    {
      sprites[i].deleteSprite();                                   //delete the sprite memory
    }
    div++;                                                         //increment div by 1
  }

 // int div = 1;  //try 1,2,3,4,...                                //code to do same above manually
 // sprite_height = (lcd.height() + div - 1) / div;                //set sprite_height by div
 // sprites[0].createSprite(lcd.width(), sprite_height);           //set sprite 0 to half height
 // sprites[1].createSprite(lcd.width(), sprite_height);           //set sprite 1 to half height 

  sp.setColorDepth(16);                                            //set sprite color depth
  //sp.setFont(&fonts::Font6);                                     //another option using Font6
  sp.setFont(&fonts::FreeSansBold18pt7b);                          //set sprite font to true type Sans bold
  sp.setTextDatum(textdatum_t::middle_center);                     //set display calculation to middle of screen

  lcd.startWrite();                                                //make instance of lcd to begin writing to screen
}




void DIAL::draw(int32_t freq, int yoff)                            //code to draw the dial
{
  int num;
  float numf;
  float zoom_x, zoom_y;
  float angle;

  freq=abs(freq);
  
  #ifdef MODE0                                                     //if model 1 selected has text lays horizontal
  int xc = lcd.width() / 2 + hoff ;                                 //sets horizontal position of display
  int yc = voff + top_margin + (std::fmax(radiusMain, radiusSub)); //set start of analog dial
  float aoff = 0;                                                  //set rotation to none
  #endif
  #ifdef MODE1                                                     //model 2 lays vertical (like old helicrafters dial) R
  int xc = voff + top_margin + (std::fmax(radiusMain, radiusSub)); //start point cut display in half and add offset
  int yc = lcd.height() / 2 + hoff;                                //start point cut display in half and add offset
  float aoff = -90.0f;                                             //set rotation to -90 degrees
  #endif
  #ifdef MODE2                                                     //model 3 lays vertical (like old helicrafters dial) L
  int xc = lcd.width() - (voff + top_margin + (std::fmax(radiusMain, radiusSub)));//start point cut display in half and add offset
  int yc = lcd.height() / 2 + hoff;                                //start point cut display in half and add offset
  float aoff = 90.0f;                                              //set rotation to +90 degrees
  #endif  

  #ifdef MAIN_UNIT_kHz                                             //if displaying text in kHz
  TickResoMain = 10000;                                            //set main dial tick resolution
  TickResoSub = 1000;                                              //set sub dial tick resolution
  #endif

  if(TickResoSub == 10000) TickResoMain = 100000;                  //if sub dial tick resolution is KHz, set main to same


  // draw dial Background ---------------------------------------------------------
  #ifdef MODE0
  sprites[flip].fillRect(0, voff-yoff, lcd.width(), lcd.height()-voff, TFT_BLACK); //clear screen Model 1 //lcd.height()-voff
  #endif
  #ifdef MODE1
  sprites[flip].fillRect(voff, 0-yoff, lcd.width()-voff, lcd.height(), TFT_BLACK); //clear screen Model 2
  #endif
  #ifdef MODE2
  sprites[flip].fillRect(0, 0-yoff, lcd.width()-voff, lcd.height(), TFT_BLACK);    //clear screen Model3
  #endif


  // draw Sub dial ----------------------------------------------------------------
  #ifdef REV_DIAL                                                  //code to reverse direction of dial
  angle = -resoSub * (float)( freq % (TickResoSub*10) ) / (float)TickResoSub ; 
  #else if
  angle = resoSub * (float)( freq % (TickResoSub*10) ) / (float)TickResoSub ;
  #endif

  sp.createSprite(1, 1);
  sp.clear(TickSubCol);
  float pvoff;
  if (tnSub<=0) pvoff = -0.5f; else pvoff = 0.5f;
  for (int i = -100; i < 100; i++)
  {
    float a = angle - (float)i * resoSub;
    if (a > 100 || a < -100) continue;
    zoom_x = (i%5 ? tickWSub : ((i % 10) ? tick5WSub : tick10WSub));
    zoom_y = (i%5 ? tickLSub : ((i % 10) ? tick5LSub : tick10LSub));
    sp.setPivot(0, radiusSub / zoom_y + pvoff);
    sp.pushRotateZoomWithAA(&sprites[flip], xc, yc - yoff, a+aoff, zoom_x, zoom_y, 0);
  }

  // draw Sub number ----------------------------------------------------------------
  zoom_x = font_sizex_sub;
  zoom_y = font_sizey_sub;
  sp.createSprite(60, 28);

  #ifdef MODE0
  if(tnSub<=0){
    sp.setTextDatum(textdatum_t::top_center);
    sp.setPivot((float)sp.width() / 2, (radiusSub + tnSub) / zoom_y);
  }else{
    sp.setTextDatum(textdatum_t::baseline_center);
    sp.setPivot((float)sp.width() / 2, (radiusSub + tnSub) / zoom_y + (float)sp.height() );
  }
  #endif

  #ifdef MODE1
  if(tnSub<=0){
    sp.setTextDatum(textdatum_t::middle_left);
    sp.setPivot( (radiusSub + tnSub) / zoom_x, (float)sp.height() / 2);
  }else{
    sp.setTextDatum(textdatum_t::middle_right);
    sp.setPivot( (radiusSub + tnSub) / zoom_x + (float)sp.width(), (float)sp.height() / 2);
  }  
  #endif

  #ifdef MODE2
  if(tnSub<=0){
    sp.setTextDatum(textdatum_t::middle_right);
    sp.setPivot( -(radiusSub + tnSub) / zoom_x + (float)sp.width(), (float)sp.height() / 2);
  }else{
    sp.setTextDatum(textdatum_t::middle_left);
    sp.setPivot( -(radiusSub + tnSub) / zoom_x , (float)sp.height() / 2);
  }
  #endif

  sp.setTextColor(NumSubCol, TFT_BLACK);
  for (int i = -80; i < 80; i += 10)
  {
    #ifdef REV_DIAL
    float a = angle + (float)i * resoSub;
    #else if
    float a = angle - (float)i * resoSub;
    #endif

    if (a > 100 || a < -100) continue;
    sp.clear(TFT_BLACK);
    num =  (freq/(TickResoSub*10)*10 + i + 1000)*(TickResoSub/1000) % (TickResoMain/100);
    #ifdef MODE0
      if(tnSub<=0) sp.drawNumber( num , sp.width()>>1, 0 );
      else sp.drawNumber( num , sp.width()>>1, sp.height()-1 );
    #endif

    #ifdef MODE1
      if(tnSub<=0) sp.drawNumber( num , 0, sp.height()>>1  );
      else sp.drawNumber( num , sp.width(), sp.height()>>1  );
    #endif

    #ifdef MODE2
      if(tnSub<=0) sp.drawNumber( num , sp.width(), sp.height()>>1  );
      else sp.drawNumber( num , 0, sp.height()>>1  );
    #endif

    sp.pushRotateZoomWithAA(&sprites[flip], xc, yc - yoff, a, zoom_x, zoom_y, 0);
  }



  // draw Main dial -----------------------------------------------------------------------
  #ifdef REV_DIAL
  angle = -resoMain * (float)( freq % (TickResoMain*10) ) / (float)TickResoMain;
  #else if
  angle = resoMain * (float)( freq % (TickResoMain*10) ) / (float)TickResoMain;
  #endif

  sp.createSprite(1, 1);
  sp.clear(TickMainCol);
  if (tnMain<=0) pvoff = -0.5f; else pvoff = 0.5f;
  for (int i = -100; i < 100; i++)
  {
    float a = angle - (float)i * resoMain;
    if (a > 100 || a < -100) continue;
    zoom_x = (i%5 ? tickWMain : ((i % 10) ? tick5WMain : tick10WMain));
    zoom_y = (i%5 ? tickLMain : ((i % 10) ? tick5LMain : tick10LMain));
    sp.setPivot(0, radiusMain / zoom_y + pvoff);
    sp.pushRotateZoomWithAA(&sprites[flip], xc, yc - yoff, a+aoff, zoom_x, zoom_y, 0);
  }



  // draw Main number ----------------------------------------------------------------------
  zoom_x = font_sizex_main;
  zoom_y = font_sizey_main;
  sp.createSprite(85, 28);

  #ifdef MODE0
  if(tnMain<=0){
    sp.setTextDatum(textdatum_t::top_center);
    sp.setPivot((float)sp.width() / 2, (radiusMain + tnMain)/zoom_y );
  }else{
    sp.setTextDatum(textdatum_t::baseline_center);
    sp.setPivot((float)sp.width() / 2, (radiusMain + tnMain)/zoom_y + (float)sp.height() );
  }
  #endif

  #ifdef MODE1
  if(tnMain<=0){
    sp.setTextDatum(textdatum_t::middle_left);
    sp.setPivot( (radiusMain + tnMain) / zoom_x , (float)sp.height() / 2);
  }else{
    sp.setTextDatum(textdatum_t::middle_right);
    sp.setPivot( (radiusMain + tnMain) / zoom_x + (float)sp.width(), (float)sp.height() / 2);
  }  
  #endif

  #ifdef MODE2
  if(tnMain<=0){
    sp.setTextDatum(textdatum_t::middle_right);
    sp.setPivot( -(radiusMain + tnMain) / zoom_x + (float)sp.width(), (float)sp.height() / 2);
  }else{
    sp.setTextDatum(textdatum_t::middle_left);
    sp.setPivot( -(radiusMain + tnMain) / zoom_x , (float)sp.height() / 2);
  }
  #endif

  sp.setTextColor(NumMainCol, TFT_BLACK);
  for (int i = -80; i < 80; i += 10)
  {
    #ifdef REV_DIAL
    float a = angle + (float)i * resoMain;
    #else if
    float a = angle - (float)i * resoMain;
    #endif

    if (a > 100 || a < -100) continue;
    sp.clear(TFT_BLACK);                                       //

    #ifdef MAIN_UNIT_kHz
      num = abs(freq)/(100000)*100 + i*10;
    #else if
      numf = ( abs(freq)/(TickResoMain*10) + i*0.1f ) * 1e-5 * TickResoMain;
    #endif

    #ifdef MODE0
    if(tnMain<=0){
      #ifdef MAIN_UNIT_kHz
      if(num>=0) sp.drawNumber( num, sp.width()>>1, 0);
      #else if
      if(numf>=0) sp.drawFloat( fabs(numf), 1, sp.width()>>1, 0);
      #endif
    }else{
      #ifdef MAIN_UNIT_kHz
      if(num>=0) sp.drawNumber( num, sp.width()>>1, sp.height()-1);
      #else if
      if(numf>=0) sp.drawFloat( fabs(numf), 1, sp.width()>>1, sp.height()-1);
      #endif
    }
    #endif

    #ifdef MODE1
    if(tnMain<=0){
      #ifdef MAIN_UNIT_kHz
      if(num>=0) sp.drawNumber( num, 0, sp.height()>>1);
      #else if
      if(numf>=0) sp.drawFloat( fabs(numf), 1, 0, sp.height()>>1  );
      #endif
    }else{
      #ifdef MAIN_UNIT_kHz
      if(num>=0) sp.drawNumber( num, sp.width(), sp.height()>>1 );
      #else if
      if(numf>=0) sp.drawFloat( fabs(numf), 1, sp.width(), sp.height()>>1  );
      #endif
    }
    #endif

    #ifdef MODE2
    if(tnMain<=0){
      #ifdef MAIN_UNIT_kHz
      if(num>=0) sp.drawNumber( num, sp.width(), sp.height()>>1 );
      #else if
      if(numf>=0) sp.drawFloat( fabs(numf), 1, sp.width(), sp.height()>>1  );
      #endif
    }else{
      #ifdef MAIN_UNIT_kHz
      if(num>=0) sp.drawNumber( num,  0, sp.height()>>1  );
      #else if
      if(numf>=0) sp.drawFloat( fabs(numf), 1, 0, sp.height()>>1  );
      #endif
    }
    #endif

    sp.pushRotateZoomWithAA(&sprites[flip], xc, yc - yoff, a, zoom_x, zoom_y, 0);
  }

  sp.setTextDatum(textdatum_t::middle_center);

  // draw Pointer --------------------------------------------------------------------------------------------------
  sp.createSprite(1, 1);
  sp.clear(PointerCol);
  sp.setPivot(0, -0.5f);
  #ifdef MODE0
  sp.pushRotateZoomWithAA(&sprites[flip], xc, voff-yoff+top_margin-line_start, 0, WidthPointer, LenPointer, 0);
  #endif
  #ifdef MODE1
  sp.pushRotateZoomWithAA(&sprites[flip], voff, yc-yoff, -90, WidthPointer, LenPointer, 0);
  #endif
  #ifdef MODE2
  sp.pushRotateZoomWithAA(&sprites[flip], lcd.width()-voff, yc-yoff, 90, WidthPointer, LenPointer, 0);
  #endif
  

}
  
