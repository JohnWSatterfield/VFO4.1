/*------------------------------------------------------------------

   VFO System for ESP32 micro controllers

      Designed to work on Atlas 210X
      
      by J. Satterfield / KI5IDZ <john.satterfield@mindspring.com>

      Version VFO 9.62 - Feb 17, 2024
          
      based on work by JF3HZB / T.UEBO
--------------------------------------------------------------------*/
/*The following were contributions by others for software and hardware design:
 * dial.cpp, display.cpp, graph.cpp software Author: JF3HZB / T.UEBO
 * Modified by: John M. Price (WA2FZW) 05/28/19 - Version 5.5
 * Original dial.cpp modified by T.UEBO 7/1/23
 * No longer using program by John M. Price but his work inspired this prgram
 * si5351.h - Si5351 library for Arduino
 * Copyright (C) 2015 - 2016 Jason Milldrum <milldrum@gmail.com>
 *                           Dana H. Myers <k6jq@comcast.net>
 * si5351mcu.h - si5351 library for Arduino 
 * Copyright (C) 2017 Pavel Milanes <pavelmc@gmail.com>
 * PCF8574 GPIO Port Expand Copyright by Renzo Mischianti
 * (c) 2017 Renzo Mischianti www.mischianti.org All right reserved.
 * Many defines derived from clk-si5351.h in the Linux kernel.
 * Sebastian Hesselbarth <sebastian.hesselbarth@gmail.com>
 * Rabeeh Khoury <rabeeh@solid-run.com>
 * do_div() macro derived from /include/asm-generic/div64.h in
 * the Linux kernel.
 * Copyright (C) 2003 Bernardo Innocenti <bernie@develer.com>
 * Display software drivers are LovyanGFX ver 1.1.12
 * Copyright (c) 2020 lovyan03 (https://github.com/lovyan03)
 * https://github.com/lovyan03/LovyanGFX/blob/master/license.txt
 * 
 * This current version VFO4.1 9.62
 *
 * Original By: T.Uebo (JF3HZB) 02/10/2019
 * Modified by: John M. Price (WA2FZW) 05/28/19 - Version 5.5
 * do_div() macro derived from /include/asm-generic/div64.h in
 * the Linux kernel.
 * Copyright (C) 2003 Bernardo Innocenti <bernie@develer.com>
 * https://github.com/lovyan03/LovyanGFX/blob/master/license.txt
 * 
 * This current version VFO4.1 9.62
 *
 * Uses ESP32 micro controller normally without SRAM
 *  Uses Atlas Crystal Filter 5.645, 5.520 or Drake 5.645 MHz software selectable 
 *  Optional uses Atlas C.O. or SI5351 C.O. software selectable
 *  Uses 1.9 inch 170x320 ST7789 with Serial SPI interface
 *  Uses 1.9 inch 170x320 ST7789 with 8 bit parallel interface
 *  Uses 1.77 inch 128x160 ST7735 with Serial SPI interface
 *  Uses 2.0 inch 240x320 ST7789 with Serial SPI interface
 *  Arduino board: ESP32-T7 mini Wrover Flash 2MB, PSRAM "QSPI PSRAM" usually not needed
 *  Arduino board: ESP32-S2 mini Flash 2MB, PSRAM: "QSPI PSRAM" usually not needed
 *  Arduino board: ESP32-S3 mini Flash 4MB, PSRAM: "QSPI PSRAM" usually not needed
 *  Arduino board: ESP32-S3 zero Flash 4MB, PSRAM: "QSPI PSRAM" usually not needed
 *  Arduino board: ESP32-1732S019 S3 R8N16 Flash 16MB, 8M "OPI PSRAM" W/integrated Display
 *  Arduino board: ESP32-S3-S3R8 Flash 16MB, PSRAM: "QSPI PSRAM" W/integrated Display
 *  Uses Library "LovyanGFX ver. 1.1.9"
 *  
 *  - This is Version VFO4.1 9.62 -
 *  Changes needed for 
 *  

ESP32 Arduino
ESP32

------------------------------------------------*/
/*--------------------------------------------------------
   Library Include Files
----------------------------------------------------------*/

#include <stdint.h>
#include <Arduino.h>          // Standard Arduino stuff
#include "config.h"           // Hardware configuration (pin assignments & display type)
#include "dial.hpp"           //https://github.com/tjlab-jf3hzb/Digital_VFO_with_analog_dial
#if SI5351_DRV == ETHERKIT    // Compiler directive for using ETHERKIT SI5351 drivers
  #include "si5351.h"         // Copyrite (C) 2015 Jason Milldrum <milldrum@gmail.com>, Dana H. Myers <k6jq@comcast.net>
#endif
#if SI5351_DRV == MCU         // Compiler directive for using ETHERKIT SI5351 drivers
  #include "si5351mcu.h"      // Copyright (C) 2017 Pavel Milanes <pavelmc@gmail.com>
#endif
#include "encodersetup.h"     //use when encoder is connected to i2c encoder shield
#if BAND_TYPE == DIGITAL      //Compiler directive if PCF8574 digital expander is use
  #include "pcf8574inputpins.h"//custom starup for pcf8574 chip
#endif
#ifdef CLOCK_OK               //Compiler directive if clock is used
  #include <RTClib.h>         //https://github.com/adafruit/RTClib
  #include <ESP32Time.h>      //Copyright (c) 2021 Felix Biego
#endif
/*-----------------------------------------------------------------------------
 *       Type Declarations
-----------------------------------------------------------------------------*/
#if SI5351_DRV == ETHERKIT    // Compiler directive for using ETHERKIT SI5351 drivers
  Si5351 si5351;              // Ehtherkit driver
#endif
#if SI5351_DRV == MCU         // Compiller directive for using MCU SI5351 drivers
  Si5351mcu si5351;           // MCU driver
#endif
#ifdef CLOCK_OK               // Compiler directive if Clock is used
  RTC_DS3231 rtc;             //Clock type declartion
  ESP32Time rtc1(3600);       //ESP32 time type declaration
#endif

/*-----------------------------------------------------------------------------
 *       Global Variables
-----------------------------------------------------------------------------*/
//The following are the default memory frequencies for each band
#if PREFERENCE == CLINT
long freqa[5] = { 3853000, 3900000, 3916000, 3950000, 5000000};  //80M band memory presets
long freqb[5] = { 7155000, 7162000, 7235000, 7255000, 5000000};  //40M band memory presets
long freqc[5] = {14235000,14250000,14300000,14325000,10000000};  //20M band memory presets
long freqd[5] = {21285000,21300000,21320000,21350000,15000000};  //15M band memory presets
long freqe[5] = {28385000,28425000,28450000,28500000,28900000};  //10M band memory presets
int recall[5] = {2,1,3,2,2};                                     //prefered memory recall at startup
#else

long freqa[5] = { 3846000, 3916000, 3987500, 3950000, 5000000};  //80m band memory presets
long freqb[5] = { 7185000, 7235000, 7275000, 7279000, 10000000};  //40m band memory presets
long freqc[5] = {14235000,14250000,14300000, 14332000, 15000000};  //20m band memory presets
long freqd[5] = {21285000,21300000,21320000, 21350000, 15000000};  //15m band memory presets
long freqe[5] = {28385000,28450000,28785000, 28500000, 28900000};  //10m band memory presets
int recall[5] = {2,3,2,1,1};      //prefered memory recall at startup
#endif

// The following are the default band ranges for each band using General License frequencies
const long begofBand[5] = {3803000,7178000,14225000,21275000,28300000}; //General Class
const long endofBand[5] = {4000000,7300000,14348000,21448000,28998000}; //All bands

/*-------------------------------------------------------
   Display settings
--------------------------------------------------------*/
LGFX lcd;                       //instance of LGFX display screen
LGFX_Sprite sp;                 //instance of LGFX display sprite
LGFX_Sprite sprites[2];         //instance of LGFX display sprite matrix
bool flip;                      //flag for LGFX
int sprite_height;              //height of sprites in display
DIAL dial;                      //instance of DIAL for drawing the analog display

/*-------------------------------------------------------
   Frequency settings for dial program
--------------------------------------------------------*/
uint8_t f_fchange = 1;         // if frequency changed, set this flag to 1
int32_t offset_frq = RFFREQ;   // Offset Frequency[Hz] RFFREQ
int32_t car_frq = CWFREQ;      // Carrier Frequncy[Hz] CWFREQ
int32_t Dial_frq = DFREQ;      // Dial frequency DFREQ
float L =0.0;                   // variable for dial length

/*----------------------------------------------------------------------------------
    Global flags
-----------------------------------------------------------------------------------*/
long frq = OPPFREQ;       //Std Atlas C. Filter: 8605000 
long freq = DFREQ;       //Variable for holding frequency
long dispFreq = DFREQ;   //Variable for displaying Frequency 
long ifFreq = IF;        //Original CF 5520000, Newer CF 5645000, Drake CF 5643600 IF
long oppFreq = OPPFREQ;  //Original CF 5523300, Newer CF 5648300, Drake CF 5646400 OPPFREQ
long cwFreq = CWFREQ;    //Original CF 5521200, Newer CF 5646200, Drake CF 5644800 CWFREQ
long fstep = 1000;       //Variable for holding frequency steps
long offSet = 0;         //Initial offset at 0
unsigned long currentMillis = 0;  //variable for counting timer 0
unsigned long currentMillis1 = 0; //variable for counting timer 1
unsigned long previousMillis = 0; //Timer 1 flag
unsigned long previousMillis1 = 0;//Timer 2 flag
unsigned long currentTest = 0;    //Timer 3 
unsigned long previousTest = 0;   //Timer 3 flag
unsigned long STEP_time = 0;      //variable for interrupt timing (future)
unsigned long last_STEP_time = 0; //variable for interrupt timing (future)
int memory=1;                     //Variable for keeping track of current memory displayed
int ActiveBand = 3;               //Variable for keeping track of current band
int OldBand = 3;                  //Variable for keeping track of last band
int HoldBand = 0;                 //Tempory variable of current band
int delaytime1 = 2000;            //Delay time in ms for frequency scanning
int delaytime2 = 5000;            //Delay time in ms for memory scanning

/*----------------------------------------------------------------------------------
    Control flags
-----------------------------------------------------------------------------------*/
bool slideSwitch = LOW;           //Flag for mode of upper or lower SSB
bool slideSwitchLast = LOW;       //variable for last position of SSB mode
bool cwpos = LOW;                 //Flag for CW mode switch position
bool cwposLast = HIGH;            //variable for last position of CW mode
bool VFOsState = LOW;             //Flag for VFOs variable set high uses pin 11
bool VFOState = HIGH;             //Flag for VFO variable set high uses pin 12
bool VFOStateLast = HIGH;         //variable for last position of VFO variable
bool on_off_flag=HIGH;            //Flag for turning encoder off if VFO is off
bool txflag = LOW;                //Flag for transmitting (not in use)
bool memoryLast = HIGH;           //Variable Flag used to change memory
bool MEMlast = HIGH;              //Flag for MEM button
bool STEPlast = HIGH;             //Flag for STEP button
bool SCANlast = HIGH;             //Flag for SCAN button
bool bandLast = HIGH;             //Band Flag used to change band
bool intclock = LOW;              //flag for internal clock
bool f_dchange = true;            //flag for rewriting screen
byte OPTlast;                     //Flagg for OPT button
char mem_str[8];                  //Character strings for display memory displayed
char band_str[8];                 //Character strings for display band displayed
char step_str[8];                 //Character strings for display frequency step displayed
char time_str[5];                 //Character strings for display time displayed
byte stp = 1;                     //Variable for keeping track of current step frequency
byte count = ActiveBand;          //Variable for keeping track of current band position       count was nul not set to 3
byte countLast;                   //Variable for last position of band
bool scanning = false;            //Flag for frequency scanning mode
bool memScanning = false;         //Flag for memory scanning mode


//----------The following are functions held in active memory------------
void IRAM_ATTR serialprintcount();




//-----------------------------------Setup--------------------------------
void setup() {
  char str[64];                  //String used by display
  bool i2c_found;                //flag to determine if i2c is working   
  Serial.begin(BIT_RATE);       //initialize serial monitor communication rate
    
//--------------------------Diagnostic routine for I2C connection --------
  #if P_STATUS == TESTING
    test_i2c();                  //run routine for testing the I2C connections
  #endif

//-----------------Initialize the display -------------------------------
    LCD_setup();
    delay(100);
    print_Splash();
    Dial_frq = freq;             // set dial to freq

// -------------------Start serial and initialize pcf8574 & encoder
  #if BAND_TYPE == DIGITAL       // Compiler directive if PCF8574 is used
    delay(100);
    pinSetup();                  // Set the pcf8574 pins 
    Serial.println("pinSetup"); //Diagnostics
    delay(100);
  #endif
  
    setupencoder();              // Set up the encoder
    delay(100);

//---------------------- Etherkit Set up the si5351---------------------
#if SI5351_DRV == ETHERKIT    // Compiler directive for using ETHERKIT SI5351 drivers
    i2c_found = si5351.init(SI5351_CRYSTAL_LOAD_8PF, CRYSTAL, CORRECTION); //set up crystal frequency
    if (i2c_found) {                            //if I2C working properly
      Serial.println("si5351 OK");             //diagnostics
      on_off_flag=HIGH;                         //set on_off_flat HIGH and let encoder work
    } else {                                    //I2C not working communication with si5351 is off
      Serial.println("si5351 not OK");         //diagnostics
      Serial.println("on_off_flag is low");    //diagnostics
      on_off_flag=LOW;                          //set on_off_flat HIGH and stop encoder from work
    }
    si5351.drive_strength( SI5351_CLK0, SI5351_DRIVE_8MA );   //set up CLK0 for VFO
    si5351.set_int(SI5351_CLK0,1);                            //initialize CLK0
    si5351.output_enable(SI5351_CLK0, 1);                     //enable output of CLK0
    set_frequency();                                          //set VFO strtup frequency
    #if CO == SI5351_CO
      si5351.drive_strength( SI5351_CLK1, SI5351_DRIVE_6MA ); //set up CLK1 for Carrier Oscillator C.O.
      si5351.set_int(SI5351_CLK1,1);                          //initialize CLK1
      si5351.output_enable(SI5351_CLK1, 1);                   //enable output of CLK1
      si5351.set_freq((ifFreq) * 100ULL, SI5351_CLK1);        // set frequency CLK1 to IF frequency Norm Position    
    #endif
    delay(50);                                                //to give the si5351 chip time to initialize
#endif

//-----------------------Set up the MCU si5351 Driver ------------------
#if SI5351_DRV == MCU
    si5351.init(CRYSTAL_MCU);                 // Initialize the crystal frequency
    si5351.correction(CORRECTION_MCU);        // Set up crystal frequency
    si5351.setPower(0,SIOUT_8mA);             // Set output power of CLI0 Si5351
    set_frequency();                          // Set initial frequency of Si5351 CLK0
    si5351.enable(0);                         // enable output of CLK0
    #if CO == SI5351_CO
      si5351.setPower(1,SIOUT_6mA);             // Set output power of CLK1 of Si5351
      si5351.setFreq(ifFreq,1);                 // Set initial frequency of si5351 CLK1
      si5351.enable(1);                         // enable output of CLK1
    #endif
    si5351.reset();                           // Reset PLL of Si5351
    i2c_found = si5351.isEnabled(0);          // See if si5351 is on
    if (i2c_found) {
      Serial.println("si5351 OK");            // Si5351 is operating
      on_off_flag=HIGH;                       // Set operating flag true
    } else {
      Serial.println("si5351 not OK");        // Si5351 not operating
      on_off_flag=LOW;                        // Set operating flag false
    }
    delay(50);                               // to give the si5351 chip time to initialize
#endif
//---------------------------------Set up the clock module ---------------
  #ifdef CLOCK_OK                              // Compiler directive if Using clock   
    if (rtc.begin()) {                         // Set up and start real time clock (rtc)
      Serial.println("RTClock Started");       //diagnostics clock working
    } else { //rtc.adjust(DateTime(2023, 8, 22, 15, 41, 20));//if clock didn't start, adjust DateTime
      Serial.println("Could not find RTC");    //diagnostic for RTC
    }                                          //diagnostics clock not found

//---------------------------------Set the correct time ------------------ 
      //uncomment next line to set clock year,month,date,hour,minute,second
      //rtc.adjust(DateTime(2024, 9, 19, 18, 51, 20));  //(yr,mn,day,hr,min,sed)
      //immediatly after setting time put               // comment marks back, compile and reload program
//------------------------------------------------------------------------
    DateTime now = rtc.now();         //read time from clock
    Serial.print(now.hour(), DEC);    //print time to the serial monitor diagnostics
    Serial.print(':');                //diagnostics
    Serial.println(now.minute(), DEC);//diagnostics
  #endif

//---------------------------------Initialize input pins------------------
    //
    #ifdef TX1
      pinMode(TX1,INPUT);             //setup TX1 input not in use
    #endif
    #ifdef OPT
      pinMode(OPT,INPUT_PULLUP);      //setup OPT input pin 
    #endif
    pinMode(VFO,INPUT);               //setup VFO input pin (from EXT. OSC. 9 pin plug)
    pinMode(cw, INPUT);               //Setup cw input pin
    pinMode(STEP, INPUT_PULLUP);      //Setup switch STEP Encoder Step Rate
    #ifdef MEM_OK                     //Compiler directive if using memory features
     pinMode(MEM, INPUT_PULLUP);      //Setup switch MEM Memory Position
     #ifdef SCAN
     pinMode(SCAN, INPUT_PULLUP);     //Setup switch SCAN
     #endif
    #endif
    #ifdef LOCK_OK                    // Compiler directive if using frequency lock
     #ifdef LOCK
      pinMode(LOCK,INPUT_PULLUP);     // Setup LOCK input pin 
     #endif
    #endif
    #if BAND_TYPE == ANALOG           // Compiler directive if using analog resistor matrix for band control
      pinMode(SS1,INPUT_PULLUP);      // Setup SS1 input pin   
      pinMode(BAND,INPUT);            // setup BAND input pin
    #endif 
    #if MC_TYPE == S3R8               // Compiler directive for LilyGo T S3R8 Display
      pinMode(PIN_POWER_ON, OUTPUT);  // triggers the EN pin
      digitalWrite(PIN_POWER_ON, HIGH);// Turn EN pin on
    #endif
    #if CO == ATLAS_CO                //C.D. if using Atlas Carrier Oscillator
      VFOsState = HIGH;               //Set flag VFOsState to HIGH
    #endif
    #if CO == SI5351_CO               //if using SI5351 to generate Carrier Oscillator
      VFOsState = LOW;                //set flag VFOsState to LOW
    #endif
    #if P_STATUS == RUN               //if processor status is set to RUN
      #ifdef VFO_OK
        VFOState = digitalRead(VFO);    //read the input of VFO from pin 4 - comment out to test 
      #endif
      setsiflag();                    //Internal / external VFO routine to turn VFO on or off 
    #endif

//    txflag=digitalRead(TX1);        //flag for transmitting w/o lp filters not in use

//-----------------Initialize variables and input pins -------------------
    #if BAND_TYPE == DIGITAL          // Compiler directive using PCF8574
     if (buttonChange[0] == LOW) ActiveBand = 1; //80m 
     if (buttonChange[1] == LOW) ActiveBand = 2; //40m
     if (buttonChange[2] == LOW) ActiveBand = 3; //20m
     if (buttonChange[3] == LOW) ActiveBand = 4; //15m
     if (buttonChange[4] == LOW) ActiveBand = 5; //10m
     readSetPins();                   //Read pins of PCF8574 expansion module for band position
    #endif
    locked = HIGH;                   //flag for frequency lock unlock encoder = High
    stp = 1;                          //sets the encoder step frequency to 1kHz
    setstep1();                       //Steps down and loads 1kHz into the first encoder step   
    #if BAND_TYPE == ANALOG           // Compiler directive using resistor network for band control
      ActiveBand = analogRead(BAND);  //Read value of Band Resistor Net
      ActiveBand = ActiveBand/BANDDIV;//integer division 0=80M,1=40M,2=20M,3=15M,4=10M
      ActiveBand = ++ActiveBand;      //increase ActiveBand by 1
    #endif
    OldBand = ActiveBand;             //set the current active band equal to the Oldband
    HoldBand = OldBand;               //set Holdband to the Oldband
    count = ActiveBand;               //Sets the band (frequency) when started up
    memory=recall[count-1];           //sets memory of current band to default
    countLast=count;                  //sets last band selected equal to current band
    switch (count) {                  //code for recalling saved frequencies
      case 1: freq = freqa[memory-1];break; //80m //select starting frequency for 80m band
      case 2: freq = freqb[memory-1];break; //40m //select starting frequency for 40m band
      case 3: freq = freqc[memory-1];break; //20m //select starting frequency for 50m band
      case 4: freq = freqd[memory-1];break; //15m //select starting frequency for 15m band
      case 5: freq = freqe[memory-1];break; //10m //select starting frequency for 10m band   
    }
    bandpresets();                     //initializes the frequency for each band.*/
}

void print_Splash() {                  //routine to print start-up screen defined in config.h
  lcd.setTextColor(CL_SPLASH);         //set color from config.h 
  lcd.setFont(&fonts::Font4);          //set font size
  #if DISP_SIZE == SMALL_DISP          //if small display, scale text size
    lcd.setFont(&fonts::Font2);        //set font size
    lcd.setTextSize(0.9f);             //set text size to 1/2 normal size
  #endif           
  #if DISP_SIZE == LARGE_DISP || DISP_SIZE == CUSTOM_DISP //if large or custom display
    lcd.setTextSize(1.0f);             //set text size to full size
  #endif
  lcd.setCursor( 0.5f*(lcd.width()-lcd.textWidth(NAME) ), 0.1f*lcd.height() ); //where to write Name intro
  lcd.printf( NAME );                  //send name intro to display
  lcd.setCursor( 0.5f*(lcd.width()-lcd.textWidth(VERSIONID) ), 0.3f*lcd.height()); //where to write Version ID
  lcd.printf(VERSIONID);               //send version ID to display
  lcd.setCursor( 0.5f*(lcd.width()-lcd.textWidth(ID) ), 0.5f*lcd.height());  //where to write ID
  lcd.printf(ID);                      //sendi ID to display
  delay(2000);                         //time to read display
  lcd.clear(BGCol);                    //clear screen
}

void print_Splash2() {                  //routine to print start-up screen defined in config.h
  lcd.setTextColor(CL_SPLASH);         //set color from config.h 
  lcd.setFont(&fonts::Font4);          //set font size
  #if DISP_SIZE == SMALL_DISP          //if small display, scale text size
    lcd.setTextSize(0.5f);             //set text size to 1/2 normal size
  #endif           
  #if DISP_SIZE == LARGE_DISP || DISP_SIZE == CUSTOM_DISP //if large or custom display
    lcd.setTextSize(1.0f);             //set text size to full size
  #endif
  lcd.setCursor( 0.5f*(lcd.width()-lcd.textWidth(NAME) ), 0.1f*lcd.height() ); //where to write Name intro
  lcd.printf( NAME );                  //send name intro to display
  lcd.setCursor( 0.5f*(lcd.width()-lcd.textWidth(VERSIONID) ), 0.3f*lcd.height()); //where to write Version ID
  lcd.printf(VERSIONID);               //send version ID to display
  lcd.setCursor( 0.5f*(lcd.width()-lcd.textWidth(ID) ), 0.5f*lcd.height());  //where to write ID
  lcd.printf(ID);                      //sendi ID to display
  delay(2000);                         //time to read display
  lcd.clear(BGCol);                    //clear screen
}

void serialprintcount() {                           // Encoder routine
  long encodercount = 0;                            //variable used by encoder to count motion
  long encodercountlast = 0;                        //Flag used to control encoder output
  if(on_off_flag&&locked) {                         //if on_off_flag & locked are high, read encoder
  if (quad >= EncoderStep || quad <= -EncoderStep){ //Test for encoder position moved
    if (quad > EncoderStep) quad = EncoderStep;     //Test for clock wise motion
    if (quad <-EncoderStep) quad = -EncoderStep;    //Test for CCW motion
    encodercount += quad/EncoderStep;               //increment encoder count
    freq = freq + long((quad * fstep)/EncoderStep); //update frequency to encoder change
    f_dchange = 1;                                  //set display flag to 1
    //Serial.println(freq);                         //diagnostics
    quad = 0;                                       //reset quad value
  } 
}
}


//--------------------Begin main loop here------------------------------------
void loop() {    

  int delaytime1 = 2000;                            //Delay time in ms for frequency scanning
  int delaytime2 = 5000;                            //Delay time in ms for memory scanning
  int delaytime3 = 300;                             //Delay time in ms for interrupts (future)
  
//---------------------    check encoder   ---------------------------
    serialprintcount();                             //Check if encoder has changed

//-------------Check encoder and write frequency change to display 1 & 2
  if (f_dchange == 1) {                             //if display flag is HIGH
      update_Display();                             //update the display
      f_dchange = 0;                                //reset display flag LOW
  }

//--------------------- initialize pushbutton flags ---------------------------
  #if BAND_TYPE == DIGITAL                          // Compiler directive using PCF8574
    if (keyPressed) {                               //Pcf8574 interrupt triggered
      updatePins();                                 //update status of pcf8574 pins
      readSetPins();                                //read the band position 
      keyPressed = false;
    }
  #endif
//------------------- start 1 minute RTC update clock -------------------------
    currentMillis = millis();                       //start currentMillis counter value
    if (currentMillis - previousMillis >= 60000) {  //look for difference since last update
      #ifdef CLOCK_OK                               //Compiler directive if using clock
       f_dchange = 1;                               //set display flag to 1
       DateTime now = rtc.now();                    //read time from clock
      #endif
      previousMillis = currentMillis;               //restart timer value 
    }
  
//---------------- determine if mode switch set to CW position -------------
  setcw();                                          //run setcw routine

//-------------------------Additional Testing Diagnostics ---------------------
  #if P_STATUS != RUN 
    test_IO();                                      //Compiler Directive for Test Code
  #endif
   
//----------------------Slide switch position Norm/Opp ------------------------
    set_Slide_Switch();
     
//---------Code for handling band switch settings and changes----------
  #if BAND_TYPE == ANALOG                          // Compiler directive using resistor matrix band change
    band_Change_Analog();                          // Run resistor matrix Band Change routine
  #endif
  
  #if BAND_TYPE == DIGITAL                         // Compiler directive Code using PCF8574 for band change
    band_Change_PCF8574();                         // Run PCF8574 Band Change routine
  #endif
   
//---------Code for handling push button inputs----------------- 
  if (!digitalRead(STEP)&&STEPlast) setstep1();    //check if STEP has been pressed
  if (digitalRead(STEP)&&!STEPlast) {STEPlast = HIGH; delay(50);} //reset the STEP to high state 
  #ifdef MEM_OK                                    // Compiler directive if using memory functions
    if (!digitalRead(MEM)&&MEMlast) mem_change();  //check if MEM has been pressed next memory (A,B or C) 
    if (digitalRead(MEM)&&!MEMlast) {MEMlast = HIGH; delay(50);}  //reset the MEM to high state
  #endif   
  #ifdef OPT
  if (!digitalRead(OPT)&&OPTlast) opt_change();    //check if OPT has been pressed
  if (digitalRead(OPT)&&!OPTlast) {OPTlast = HIGH; delay(50);}    //reset the OPT to high state
  #endif
  #ifdef MEM_OK                                   // Compiler directive if using memory functions
  #ifdef SCAN
  if (!digitalRead(SCAN)&&SCANlast) {              //check if SCAN has been pressed
      memScanning = true;                          //set memScanning flag true
      SCANlast = LOW;                              //set SCANlast flag low
     }
  #endif
  #endif
  #ifdef LOCK_OK 
    #ifdef LOCK                                 //Compiler directive if using frequency lock function
     if (!digitalRead(LOCK))locked = LOW;else locked = HIGH; // Read LOCK, High is unlocked, low is locked
    #endif
  #endif 

//--------Code to handle Scan functions--------------------
  #ifdef MEM_OK                                    // Compiler directive if using memory functions
   MemScanning();                                  // scan different memories every 5 seconds
   Scanning();                                     // scan by 1 KHz defined range
  #endif
} //end of loop

void RamPSRam () {
      //determine ESP32 memory and PSRAM values
    log_d("Total heap: %d", ESP.getHeapSize());  //Esp heap memory size
    log_d("Free heap: %d", ESP.getFreeHeap());   //ESP heap memory available
    log_d("Total PSRAM: %d", ESP.getPsramSize());//ESP PSRAM memory size
    log_d("Free PSRAM: %d", ESP.getFreePsram()); //ESP PSRAM memory available
}

void test_IO() {
  currentTest = millis();                                       //Start Test timer
  if (currentTest - previousTest >= 20000) {                    //if 20 seconds after last test
    Serial.println(frq);                                        //Actual frequency sent to si5351
    Serial.println(freq);                                       //Displayed frequency (Actual - IF)
    Serial.print("IF Offset: "); Serial.println(offSet);        //IF offset frequency
    Serial.print("Norm: "); Serial.println(slideSwitch);        //state of Norm/OPP 0/1 0=Norm 1=OPP
    Serial.print("VFO on_off: "); Serial.println(on_off_flag);  //state of on_off_flag to see if si5351 is 1=on or 0=off
    Serial.println(ifFreq);                                         //IF frequency
    Serial.print("CF: "); Serial.println(CF);                   //State of CF compiler directory 1-OLDER, 2=NEWER, 3=DRAKE
    Serial.print("CO Si=0: "); Serial.println(VFOsState);       //VFOsState flag 1=Atlas C.O. 0= si5351 C.O.
    Serial.print("Sprite Height"); Serial.println(sprite_height);//height of a sprite
    previousTest = currentTest;                                 //reset currentTest flag
  }
}

//----------------------------code to test I2C -------------------
void test_i2c() {                                               //Code to test I2C
for (byte i = 3; i < 120; i++)
  {
    Wire.beginTransmission (i);                                 //Start Wire function 
    if (Wire.endTransmission () == 0)                           //check to see if wire function working
      {
      Serial.print ("Found address: ");                         //Report if i2c address fornd
      Serial.print (i, DEC);                                    //print the address found
      Serial.print (" (0x");
      Serial.print (i, HEX);
      Serial.println (")");
      count++;
      //delay (1);  
      } // end of good response
  } // end of for loop
  Serial.println ("Done.");                                     //Report function completed
  Serial.print ("Found ");                                      //Report how many found
  Serial.print (count, DEC);                                    //Report
  Serial.println (" device(s).");                               //Report
}

void Scanning() {                                 // Code for doing scanning functions
  #ifdef SCAN
    while (scanning) {                              //Scanning frequency 1KHz per 2 seconds
       currentMillis1 = millis();                 //resetting timer 1
       if (currentMillis1 - previousMillis1 >= delaytime1) { //look for difference since last update
        scan();                                   //call scan subroutine 
        previousMillis1 = currentMillis1;         //reset timer flag
        //Serial.println("scanning = true");      //diagnostics
       }
       if (digitalRead(SCAN)&&!SCANlast) {SCANlast = HIGH; delay(20);} //reset the button6 to high state
       if (!digitalRead(SCAN)&&(SCANlast)) {      //if Button7 (Scan) is pressed 2nd time
         scanning = false;                        //reset scanning flag
         SCANlast=LOW;                            //reset scan button flag
         memScanning = false;                     //set memScanning flag to true
         //Serial.println("all false exit");      //diagnostics
         } //end of if
         delay(100);                              //Debouncing delay
    } //end of while 
    if (digitalRead(SCAN)&&!SCANlast) {SCANlast = HIGH; delay(20);} //reset the button6 to high state   
  #endif
}

void MemScanning() {                              // Code for doing mem scanning functions
  #ifdef SCAN
    while (memScanning) {                           // start 5 seconds between frequency while scanning 
      currentMillis1 = millis();                  //start currentMillis counter value
      if (currentMillis1 - previousMillis1 >= delaytime2) { //look for difference since last update
        mem_change();                             //memory change routine also updates display
        previousMillis1 = currentMillis1;         //restart timer value 
        //Serial.println("memScanning = true");   //diagnostics
      }
      if (digitalRead(SCAN)&&!SCANlast) {SCANlast = HIGH; delay(20);} //reset the SCAN to high state
      if (!digitalRead(SCAN)&&(SCANlast)) {       //if SCAN is pressed, exit scanning mode
        scanning = true;                          //set scan mode flag to false
        //Serial.println("exit memScanning");     //diagnostics
        memScanning = false;                      //set memory scanning mode flat to false
        SCANlast = LOW;                           //reset scan button flag
        f_dchange = 0;                            //set flag to write to screen upon exiting
      } //end if if
      delay(100);                                 //Debouncing delay
    } //end of while
    if (digitalRead(SCAN)&&!SCANlast) {SCANlast = HIGH; delay(20);} //reset the button6 to high state
  #endif
}

void band_Change_Analog() {                       // Code to change bands using resistor matrix
  #if BAND_TYPE == ANALOG                         // Compiler directive if analog resistor matrix is used
    int ab = (analogRead(BAND));                  // Read value and integer division 0=80M,1=40M,2=20M,3=15m,4=10M
    ActiveBand = ab/BANDDIV;                      // divide raw data BAND by divisor BANDDIV
    //Serial.print(ab);                           // diagnostics to read raw data BAND
    //delay(2000);                                // wait 2 seconds
    ActiveBand = ++ActiveBand;                    // increase ActiveBand by 1
    OldBand = ActiveBand;                         // reset Band flag
    if ((HoldBand != OldBand)) {                  // check if switch position has changed
      countLast = count;                          // reset countLast band flag
      count=ActiveBand;                           // reset count band flag to Active Band
      band_change();                              // make a band change
      HoldBand = OldBand;                         // reset the band change detection variable
     } 
   #endif   
}

void band_Change_PCF8574() {                      // Code to change bands using PCF8574
  #if BAND_TYPE == DIGITAL                        // Compiler directive if digital I/O PCF8574 is used
    if (buttonChange[0] ==LOW) {                  //if the band switch is on the 80m position
     // Serial.println("80m Band");               //diagnostic for seeing Band Switch position
      ActiveBand = 1;                             //set variable for band selection 80m
      OldBand = 1;                                //set variable to detect of a band change has been made
      if (!txflag) {                              //bypass the LP Filter not in use
      pcf8574.digitalWrite(5,LOW);                //Write output for LP Filter multiplexer
      pcf8574.digitalWrite(6,LOW);                //Write output for LP Filter multiplexer
      }
    } 
    if (buttonChange[1] ==LOW) {                  //if the band switch is on the 40m position
     //  Serial.println("40m Band");              //diagnostic for seeing Band Switch position
      ActiveBand = 2;                             //set variable for band selection 40m 
      OldBand = 2;                                //set variable to detect of a band change has been made
      if (!txflag) {                              //Test if transmitting not in use
      pcf8574.digitalWrite(5,HIGH);               //Write output for LP Filter multiplexer
      pcf8574.digitalWrite(6,LOW);                //Write output for LP Filter multiplexer
      }
     } 
    if (buttonChange[2] ==LOW) {                  //if the band switch is on the 20m position
    //  Serial.println("20m Band");               //diagnostic for seeing Band Switch position
      ActiveBand = 3;                             //set variable for band selection 20m 
      OldBand = 3;                                //set variable to detect of a band change has been made
      if (!txflag) {                              //Test if transmitting not in use
      pcf8574.digitalWrite(5,LOW);                //Write output for LP Filter multiplexer
      pcf8574.digitalWrite(6,LOW);                //Write output for LP Filter multiplexer
      }
     } 
    if (buttonChange[3] ==LOW) {                  //if the band switch is on the 15m position
     // Serial.println("15m Band");               //diagnostic for seeing Band Switch position
      ActiveBand = 4;                             //set variable for band selection 15m 
      OldBand = 4;                                //set variable to detect of a band change has been made
      if (!txflag) {                              //Test if transmitting not in use
      pcf8574.digitalWrite(5,HIGH);               //Write output for LP Filter multiplexer
      pcf8574.digitalWrite(6,LOW);                //Write output for LP Filter multiplexer
      }
     } 
    if (buttonChange[4] ==LOW) {                  //if the band switch is on the 10m position
    //  Serial.println("10m Band");               //diagnostic for seeing Band Switch position
      ActiveBand = 5;                             //set variable for band selection 10m 
      OldBand = 5;                                //set variable to detect of a band change has been made
      if (!txflag) {                              //Test if transmitting not in use
      pcf8574.digitalWrite(5,LOW);                //Write output for LP Filter multiplexer
      pcf8574.digitalWrite(6,HIGH);               //Write output for LP Filter multiplexer
      }
     } 
     if (HoldBand != OldBand) {                   //check if switch position has changed
      countLast = count;                          //reset countLast band flag
      count=ActiveBand;                           //reset count band flag to Active Band
      band_change();                              //make a band change
      HoldBand = OldBand;                         //reset the band change detection variable
     }//-----------------Code to bypass LP Filter if needed--------------------------
     #ifdef TX1
     txflag=digitalRead(TX1);                    //code for bypassing LP Filter when transmitting not in use
      if (txflag) {                               //if in transmit mode
      pcf8574.digitalWrite(5,LOW);                //Write output for LP Filter multiplexer
      pcf8574.digitalWrite(6,LOW); 
      }
      //Write output for LP Filter multiplexer
     #endif
   #endif
} //end band change


void set_Slide_Switch() {                         //slide switch change code
  if (cwpos == LOW)   {                           // if mode switch in not in CW Position
    cwposLast = HIGH;                             // set cwposLast to Low 
    #if BAND_TYPE == DIGITAL                      // Compiler directive using PcF8574
      if (buttonChange[7]==LOW) slideSwitch = LOW;// read SS1 set slideSwitch1 LOW=NORM
      if (buttonChange[7]==HIGH)slideSwitch = HIGH;// read SS1 set slideSwitch1 HIGH=OPP
      #ifdef SS1PIN_OK                            //DIGITAL but using SS1 Pin
        slideSwitch = digitalRead(OPT);           //useful if still using ss1 pin normally comment out
      #endif
    #endif
    #if BAND_TYPE == ANALOG                       // Compiler directive if analog resistor matrix used
      slideSwitch = digitalRead(SS1);             // read slideSwitch1 pin 16
    #endif
    if (slideSwitch != slideSwitchLast) {         // if slideSwitch not on Norm first time around
      Serial.println("switch changed");           // diagnostic for slideswitch1
      if (slideSwitch == LOW) {                   // slideSwitch P16 on set low
      offSet = 0;                                 // reset offset variable
      bandlist();                                 // display the correct band frequency & offset of each band 
      #if CO == SI5351_CO
       #if SI5351_DRV == ETHERKIT    // Compiler directive for using ETHERKIT SI5351 drivers    
        if (!VFOsState) si5351.set_freq((ifFreq) * 100ULL, SI5351_CLK1);  // set CLK1 to the IF frequency Norm Position
       #endif
       #if SI5351_DRV == MCU
        if (!VFOsState) {
          si5351.setFreq(1,ifFreq);
          si5351.enable(1);                         // enable output of CLK1
        }
       #endif
      #endif
      f_dchange = 1;                              // flag to display screen upon exit
      slideSwitchLast = slideSwitch;              // set slideSwitchLast to Low for one pass through if statement
      #if P_STATUS != RUN                           // Compiler Directive for Test Code
        Serial.println("switch not in CW");       // diagnostics
        Serial.println("switch in norm");         // diagnostic for slideswitch1 norm position
      #endif
      }  //end if slideswitch = low
    else {                                        // alternative for opp position of slideswitch
      #if CO == SI5351_CO                          // Compiler directive if using si5351 for carrier oscillator
       #if SI5351_DRV == ETHERKIT                 // Compiler directive for using ETHERKIT SI5351 drivers
        if (!VFOsState) si5351.set_freq((oppFreq) * 100ULL, SI5351_CLK1); // set up CLK1 to the IF frequency opp Position
       #endif
       #if SI5351_DRV == MCU                     // Compiler directive for using MCU SI5351 drivers
        if (!VFOsState) {
          si5351.setFreq(1,oppFreq); // Set up CLK1 to the IF frequency opp Position
          si5351.enable(1);                         // enable output of CLK1
       }      
       #endif 
      #endif       
      offSet = OFFSET;                          // set value of offSet to defined OFFSET
      bandlist();                               // display the correct band frequency & offset of each band
      f_dchange = 1;                            // flag to display screen upon exit
      slideSwitchLast = slideSwitch;            // set slideSwitchLast to Low for one pass through if statement
      Serial.println("switch not in CW");       //diagnostics
      Serial.println("switch in opp");          // diagnostic for slideswitch1 opp position
      }  // end else
    }  //end if slideswitch
  }  //end if cwpos = low
}

//--------------------------- Code to detect CW mode -----------------------
void setcw() {                                     // Code for detecting if transmitter in CW mode
  cwpos = digitalRead(cw);                         // check if mode switch is in CW Position
  if ((cwpos) && (cwposLast)) {                    // if mode switch in on cw first time around
   #if CO == SI5351_CO                              // If using Atlas CLK1 for carrier oscillator
    #if SI5351_DRV == ETHERKIT                     // Compiler directive for using ETHERKIT SI5351 drivers
      if (!VFOsState) si5351.set_freq((cwFreq) * 100ULL, SI5351_CLK1);   // set CLK1 to the cwFreq Norm Position
    #endif
    #if SI5351_DRV == MCU                          // Compiler directive for using MCU SI5351 drivers
      if (!VFOsState) {
        si5351.setFreq(1,cwFreq);     // Set CLK1 to the cwFreq Norm Position
        si5351.enable(1);                         // enable output of CLK1
      }
    #endif
   #endif
   offSet = CWOFFSET;                              // set value of offSet to defined CWOFFSET
   bandlist();                                     // display the correct band frequency & offset of each band  
   f_dchange = 1;                                  // set display flag HIGH
   cwposLast = LOW;                                // set cwposLast to High for a single pass through if statement
   Serial.println("mode switch in CW");            //diagnostics
   slideSwitchLast = !slideSwitchLast;             //reset slide switch flag
  } else if (cwpos == LOW) cwposLast = HIGH;
} //end setcw

//----------------------------code to handle frequency step -------------------
void setstep1() {                           /*procedure to set (advance) the step frequency*/
#if PREFERENCE == CLINT
  switch (stp) {                            //used by pushbutton to change frequency step
    case 1: stp = 2; fstep = 1000; break;   //frequency step change is 1 KHz
    case 2: stp = 3; fstep = 10000; break;  //frequency step change is 10 KHz
    case 3: stp = 4; fstep = 10; break;     //frequency step change is 10 Hz
    case 4: stp = 5; fstep = 100; break;    //frequency step change is 100 Hz
    case 5: stp = 1; fstep = 500; break;    //frequency step change is 500 KHz
  }
#else  
  switch (stp) {                            //used by pushbutton to change frequency step
    case 1: stp = 2; fstep = 1000; break;   //frequency step change is 1KHz
    case 2: stp = 3; fstep = 500; break;    //frequency step change is 500Hz
    case 3: stp = 4; fstep = 100; break;    //frequency step change is 100Hz
    case 4: stp = 5; fstep = 10; break;     //frequency step change is 10Hz
    case 5: stp = 1; fstep = 10000; break;  //frequency step change is 10 KHz
  }
#endif
  f_dchange = 1;
  STEPlast = LOW;                             //reset the STEP to low state
  delay(50);
}

//----------------------------code to handle band changes  --------------------
void band_change() {                         /*procedure to set (advance) the frequency band*/
  freqrecall();                              //determine what frequency should be
  #if BAND_TYPE == DIGITAL                   //Compiler directive using PCF8574
    bandLast=LOW;                            //reset band flag
  #endif
  delay(5);                                  //delay for reset
}
  
//----------------------------code to handle Scan functions--------------------
void scan() {                                //procedure to scan frequency
    freq = freq + 1000;                      //change frequency up 1kHz
    if (freq >= endofBand[count-1]) freq = begofBand[count-1]; //set frequency to beginning of band
    update_Display();                        //write changes to screen
}

//----------------------------code to handle Screen (Display) update -----------
void update_Display() {
    if (on_off_flag == HIGH) set_frequency();//send frequency to si5351
    Dial_frq = freq;                         //send frequency to dial 
    display_write();                        //write the band change to the display
    //delay(5);           
}

  
//----------------------------code to handle OPT functions--------------------
void opt_change() {                          //procedure for OPT button
    //put code here
    f_dchange = 1;                           //set display flag
    OPTlast = LOW;                           //reset the OPT flag to low state
    delay(50);                               //debounce
}
  
//------------------------code to handle memory band changes-------------------
void mem_change() {             //procedure for changing memories
  switch (count) {              //First switch makes available the memories tied to the band
    case 1: {switch (memory) {  //second switch determines which memory 1-6 will be used, First is band 1
              case 1: freqa[0] = freq;freq = freqa[1]; memory = 2;break; //save freq to memory A restore memory B
              case 2: freqa[1] = freq;freq = freqa[2]; memory = 3;break; //save freq to memory B restore memory C
              case 3: freqa[2] = freq;freq = freqa[3]; memory = 4;break; //save freq to memory C restore memory D
              case 4: freqa[3] = freq;freq = freqa[4]; memory = 5;break; //save freq to memory D restore memory E
              case 5: freqa[4] = freq;freq = freqa[0]; memory = 1;break; //save freq to memory E restore memory A
            }break;
    }
     case 2: {switch (memory) {                                          //second is band 2
              case 1: freqb[0] = freq;freq = freqb[1]; memory = 2;break; //save freq to memory A restore memory B
              case 2: freqb[1] = freq;freq = freqb[2]; memory = 3;break; //save freq to memory B restore memory C
              case 3: freqb[2] = freq;freq = freqb[3]; memory = 4;break; //save freq to memory C restore memory D
              case 4: freqb[3] = freq;freq = freqb[4]; memory = 5;break; //save freq to memory D restore memory E
              case 5: freqb[4] = freq;freq = freqb[0]; memory = 1;break; //save freq to memory E restore memory A
            }break;
     }
     case 3: {switch (memory) {                                          //third is band 3
              case 1: freqc[0] = freq;freq = freqc[1]; memory = 2;break; //save freq to memory A restore memory B
              case 2: freqc[1] = freq;freq = freqc[2]; memory = 3;break; //save freq to memory B restore memory C
              case 3: freqc[2] = freq;freq = freqc[3]; memory = 4;break; //save freq to memory C restore memory D
              case 4: freqc[3] = freq;freq = freqc[4]; memory = 5;break; //save freq to memory D restore memory E
              case 5: freqc[4] = freq;freq = freqc[0]; memory = 1;break; //save freq to memory E restore memory A
            }break;
     }
     case 4: {switch (memory) {                                          //fourth is band 4
              case 1: freqd[0] = freq;freq = freqd[1]; memory = 2;break; //save freq to memory A restore memory B
              case 2: freqd[1] = freq;freq = freqd[2]; memory = 3;break; //save freq to memory B restore memory C
              case 3: freqd[2] = freq;freq = freqd[3]; memory = 4;break; //save freq to memory C restore memory D
              case 4: freqd[3] = freq;freq = freqd[4]; memory = 5;break; //save freq to memory D restore memory E
              case 5: freqd[4] = freq;freq = freqd[0]; memory = 1;break; //save freq to memory E restore memory A
            }break;
     }
     case 5: {switch (memory) {                                          //fifth is band 5
              case 1: freqe[0] = freq;freq = freqe[1]; memory = 2;break; //save freq to memory A restore memory B
              case 2: freqe[1] = freq;freq = freqe[2]; memory = 3;break; //save freq to memory B restore memory C
              case 3: freqe[2] = freq;freq = freqe[3]; memory = 4;break; //save freq to memory C restore memory D
              case 4: freqe[3] = freq;freq = freqe[4]; memory = 5;break; //save freq to memory D restore memory E
              case 5: freqe[4] = freq;freq = freqe[0]; memory = 1;break; //save freq to memory E restore memory A
            }break;
     }
     f_dchange = 1;                              // flag to display screen upon exit
  }
  MEMlast = LOW;                                 //reset the MEM to low state
  update_Display();                              // update display now
  delay(50);
} //end mem change
  
//------------------------ code to handle frequency memory --------------------
void freqrecall() {                           //code for recalling the saved frequencies 
  switch (countLast) {                        //code for saving frequency of current band
    case 1: freqa[memory-1] = freq;break;     //Save frequency for 80m band
    case 2: freqb[memory-1] = freq;break;     //Save frequency for 40m band
    case 3: freqc[memory-1] = freq;break;     //Save frequency for 20m band
    case 4: freqd[memory-1] = freq;break;     //Save frequency for 15m band
    case 5: freqe[memory-1] = freq;break;     //Save frequency for 10m band
  }
  recall[countLast-1] = memory;               //set recall variable to last saved memory
  memory = recall[count-1];                   //set memory variable to next saved memory
  switch (count) {                            //code to recall saved frequency of next band
    case 1: freq = freqa[memory-1];break;     //Set frequency for 80m band
    case 2: freq = freqb[memory-1];break;     //Set frequency for 40m band
    case 3: freq = freqc[memory-1];break;     //Set frequency for 20m band
    case 4: freq = freqd[memory-1];break;     //Set frequency for 15m band
    case 5: freq = freqe[memory-1];break;     //Set frequency for 10m band    
  }
  update_Display();
  delay(5);                                   //time to write to screen
}
   
//----------------------------code to handle band changes ---------------------
void bandlist() {                //function for displaying the correct band frequency & offset of each band
  long interfreq = 0;
  if (count == 1){ interfreq =  ifFreq + offSet;  dispFreq = freq - 0;}    //count = 1 (80m) on the display screen
  if (count == 2){ interfreq =  ifFreq + offSet;  dispFreq = freq - 0;}    //count = 2 (40M) IF frequency for LSB (7274.97 subtract additional 30)
  if (count == 3){ interfreq = -ifFreq - offSet;  dispFreq = freq + 0;}    //count = 3 (20m) IF Frequency for USB starting with 20m (8605000)
  if (count == 4){ interfreq = -ifFreq - offSet;  dispFreq = freq + 0;}    //count = 4 (15) on the display screen
  if (count == 5){ interfreq = -ifFreq - offSet;  dispFreq = freq + 0;}    //count = 5 (10m) on the display screen
  frq = freq + interfreq;  //in this case for 20m or 14250000 - IF to calibrate radio (8605000)
  //if (freq == 14275000) frq = freq + interfreq -10;  //shift frequency around birdies
  //if (freq == 14326000) frq = freq + interfreq + 60; //shift frequency around birdies
  //TM8736 - Serial number of radio with birdies
}

//----------------------------code to handle LP Filters used ------------------
void set_mux() {                  //Routine for setting MUX switches for LP Filter by band number
#if BAND_TYPE == DIGITAL          // Compiler directive using PCF8574 for band change
  switch(count) {                 //Switch for LP Filter by band number
    case 1: {pcf8574.digitalWrite(5,LOW); pcf8574.digitalWrite(6,LOW); break;} //80m
    case 2: {pcf8574.digitalWrite(5,HIGH); pcf8574.digitalWrite(6,LOW); break;}//40m
    case 3: {pcf8574.digitalWrite(5,LOW); pcf8574.digitalWrite(6,LOW); break;} //20m
    case 4: {pcf8574.digitalWrite(5,HIGH); pcf8574.digitalWrite(6,LOW); break;}//15m
    case 5: {pcf8574.digitalWrite(5,LOW); pcf8574.digitalWrite(6,HIGH); break;}//10m
  }
#endif
}
  
//------------------------code to set output frequency of si5351  -------------
void set_frequency() {                      //procedure for setting output frequency
  bandlist();                               //determine correct frequency to display 
  #if SI5351_DRV == ETHERKIT    // Compiler directive for using ETHERKIT SI5351 drivers
    if (on_off_flag) si5351.set_freq((frq) * 100ULL, SI5351_CLK0);    //output frequency for CLK0
  #endif
  #if SI5351_DRV == MCU                     // Compiler directive for using MCU SI5351 drivers
    if (on_off_flag) si5351.setFreq(0,frq);  // Output frequency for CLK0
  #endif
}

//---------------------- code to handle External/Internal VFO -----------------
void setsiflag() {                            //External/internal VFO input routine
      bool i2c_found;                         //variable for determining if si5351 is on & communicating
      if ((!VFOState)&&(on_off_flag)) {       //if External VFO is in EXT OSC. pin
        Serial.println("si5351 is off");      //diagnostics
        on_off_flag = LOW;                    //Set Internal VFO flag off
        VFOStateLast = VFOState;              //flag for 1 time through the routine
        #if SI5351_DRV == ETHERKIT
          si5351.output_enable( SI5351_CLK0, 0);//Turn VFO CLK0 off
        #endif
        #if SI5351_DRV == MCU                 // Compiler directive for using MCU SI5351 drivers
          si5351.disable(0);                  //Turn VFO CLK0 off  
        #endif
        update_Display();                     //Update display now
      } else {                                //if Internal VFO is being used
        if((VFOState)&& (!VFOStateLast)) {    //check plug in ext socket and VFO wires jumpered to on
        #if SI5351_DRV == ETHERKIT
          si5351.output_enable( SI5351_CLK0, 1);//Restart the si5351
        #endif
        #if SI5351_DRV == MCU                // Compiler directive for using MCU SI5351 drivers
          si5351.enable(0);                  //Turn VFO CLK0 on
        #endif
        delay(200);                           //to give the si5351 chip time to initialize
        Serial.println("si5351 OK");          //diagnostics
        VFOStateLast = VFOState;              //reset VFO flag
        bandpresets();                        //first time setup after start up si5351
        Serial.println("resetting si5351");   //diagnostics
        on_off_flag = HIGH;                   //set si5351 flag high
        update_Display();
      }
  }
}

//------------ code to handle band presets during startup Only ----------
void bandpresets() {       /*procedure for first time setup after start up of si5351 */
  bandlist();              //Determine and set the proper band 
  set_mux();               //set multiplexer to correct band 
  set_frequency();         //Send freq to si5351
  delay(5);                //time for si5351 to process frequency
  stp = 1;                 //set frequency step
  setstep1();              //set frequency step to default
}
 
//-----------Rest of code to handle display------------------------------------
//----------------------------code to handle band display 2--------------------
void display_Band() {                         //code to display the current selected band
  switch(count) {                             //determine which position the band switch is located
    case 1: sprintf(band_str, " 80 M"); break; //set to 80M position USB
    case 2: sprintf(band_str, " 40 M"); break; //set to 40M position USB
    case 3: sprintf(band_str, " 20 M"); break; //set to 20M position USB
    case 4: sprintf(band_str, " 15 M"); break; //set to 15M position USB
    case 5: sprintf(band_str, " 10 M"); break; //set to 10M position USB
  } 
  sprites[flip].setFont(&fonts::Font4); // Set font
  sprites[flip].setTextSize(0.75f);     // Scale font size
  sprites[flip].setTextColor(CL_NUM);   // Set font color
  #if  DISP_SIZE == LARGE_DISP          // if large display
    sprites[flip].setCursor(10, T1_POS );   // place cursor here
    sprites[flip].print(band_str);      // send to sprite memory
  #endif
  #if DISP_SIZE == SMALL_DISP           // if small display
    sprites[flip].setTextSize(0.50f);   // Scale font size
    sprites[flip].setCursor(4, T1_POS );    // place cursor here
    sprites[flip].print(band_str);      // send to sprite memory
  #endif
  #if DISP_SIZE == CUSTOM_DISP          // if custom display
    #ifndef SHORT16_OK                  // Compiler directive if full screen
     sprites[flip].setCursor(10, T1_POS );  // place cursor here
    #endif
    #ifdef SHORT16_OK                   // Compiler directive reduce screen by 16 pixels
     sprites[flip].setCursor(10+DISP_L/2, T1_POS );  // place cursor here
    #endif
    sprites[flip].print(band_str);      // send to sprite memory
  #endif
}
//-----------------------------------------------------------------------------   
//----------------------------code to handle step display 2--------------------
void display_Step() {                    //code to handle large & custom display
  #if  DISP_SIZE == LARGE_DISP || DISP_SIZE == CUSTOM_DISP
  switch (fstep) {                               //determine step position
    case 10: sprintf(step_str,  "10  Hz");break;  //step frequency is 10Hz
    case 100: sprintf(step_str, "100  Hz");break;  //step frequency is 100Hz
    case 500: sprintf(step_str, "500  Hz");break;  //step frequency is 500Hz
    case 1000: sprintf(step_str,"1  KHz");break;  //step frequency is 1KHz
    case 10000: sprintf(step_str,"10  KHz");break;  //step frequency is 1KHz
  } 
  #endif
  #if DISP_SIZE == SMALL_DISP           //code to handle small display  
  switch (fstep) {                               //determine step position
    case 10: sprintf(step_str,  "10 Hz");break;  //step frequency is 10Hz
    case 100: sprintf(step_str, "100Hz");break;  //step frequency is 100Hz
    case 500: sprintf(step_str, "500Hz");break;  //step frequency is 500Hz
    case 1000: sprintf(step_str,"1 KHz");break;  //step frequency is 1KHz
    case 10000: sprintf(step_str,"10 KHz");break;  //step frequency is 1KHz
  } 
  #endif  
  sprites[flip].setFont(&fonts::Font4); // Set font
  sprites[flip].setTextSize(0.75f);     // Scale font size
  sprites[flip].setTextColor(CL_NUM);   // Set font color
  #if  DISP_SIZE == LARGE_DISP          // if large display
    #ifdef MEM_OK && #ifdef CLOCK_OK    // if using memory function & using clock
      sprites[flip].setCursor(90, T1_POS ); // place cursor here
    #endif
    #ifdef MEM_OK                       // if using memory functions
      #ifndef CLOCK_OK                  // if not using clock
        sprites[flip].setCursor(140, T1_POS );// place cursor here
      #endif
    #endif
    #ifndef MEM_OK && #ifndef CLOCK_OK  // Compiler directive if using clock
      sprites[flip].setCursor(260, T1_POS );// place cursor here
    #endif
    
    sprites[flip].print(step_str);      // send to sprite memory
  #endif
  #if DISP_SIZE == SMALL_DISP           // code for writing step frequency to small display
    sprites[flip].setTextSize(0.50f);   // set font scale
    #ifdef MEM_OK
      #ifdef CLOCK_OK    // if using memory function & using clock
        sprites[flip].setCursor(45, T1_POS ); // place cursor here 
      #endif
    #endif
    #ifndef CLOCK_OK 
      #ifndef  MEM_OK  // Compiler directive if not memory functions & clock
        sprites[flip].setCursor(120, T1_POS );// place cursor here
      #endif
    #endif
    #ifdef CLOCK_OK 
      #ifndef MEM_OK                    // Compiler directive if using memory functions or clock
        sprites[flip].setCursor(70, T1_POS ); // place cursor here
      #endif
    #endif
    #ifdef MEM_OK
      #ifndef CLOCK_OK
        sprites[flip].setCursor(70, T1_POS ); // place cursor here
      #endif
    #endif
    sprites[flip].print(step_str);      // Send sprite to memroy
  #endif
  #if DISP_SIZE == CUSTOM_DISP          // if custom display
    sprites[flip].setTextColor(CL_NUM_O); // Set font color
    #ifdef MEM_OK                       // if using memory functions
      #ifdef CLOCK_OK                   // if using clock functions
       sprites[flip].setCursor(90, T1_POS );// place cursor here
      #endif
    #endif
    #ifndef MEM_OK                   // if using clock functions
       sprites[flip].setCursor(140, T1_POS );// place cursor here
    #endif
    #ifndef CLOCK_OK || #ifndef MEM_OK  // Compiler directive if not using clock or not using memory functions
      sprites[flip].setCursor(140, T1_POS );// Place cursor here
    #endif
    #ifndef CLOCK_OK 
      #ifndef MEM_OK  // Compiler directive if not using clock & memory functions
        sprites[flip].setCursor(260, T1_POS );// Place cursor here
      #endif
    #endif
    sprites[flip].print(step_str);      // send to sprite memory
  #endif
}
//-----------------------------------------------------------------------------   
//----------------------------code to handle time display 2--------------------
void display_Time() {                   // Code to display the current UTC time on screen
  #ifdef CLOCK_OK                       // Compiler directive if using clock
   DateTime now = rtc.now();            // look up current time on clock module
   delay(5);                            // give module time to respond
   char buf2[] = "hh:mm ";              // write string to display time
   now.toString(buf2);                  // send string to buffer
   sprintf(time_str,buf2);              // send buffer to time string for display
   sprites[flip].setFont(&fonts::Font4);// Set font size
   sprites[flip].setTextSize(0.75f);    // Set font scale
   sprites[flip].setTextColor(CL_NUM);  // set font color
   #if  DISP_SIZE == LARGE_DISP         // If large display
    #ifndef MEM_OK                      // If using memory functions
      sprites[flip].setCursor(260, T1_POS );// Place cursor here
    #endif
    #ifndef SHORT16_OK                  // Compiler directive reduce screen by 16 pixels
     #ifdef MEM_OK                      // Compiler directive if using memory functions
      sprites[flip].setCursor(184, T1_POS );// Place cursor here
     #endif
    #endif
    #ifdef SHORT16_OK                    // Compiler directive if full screen
      #ifdef MEM_OK                      // Compiler directive if using memory functions
       sprites[flip].setCursor(192, T1_POS );// Place cursor here
      #endif
    #endif
    sprites[flip].print(time_str);       // Sent sprite to memory
   #endif
   #if DISP_SIZE == SMALL_DISP           // code for small display
     #ifdef CLOCK_OK
      sprites[flip].setTextSize(0.50f);    // Set font scale 
      #ifdef MEM_OK                       // If not using memory functions
        sprites[flip].setCursor(90, T1_POS );  // Place cursor here
      #else                       // If using memory functions
        sprites[flip].setCursor(128, T1_POS );  // Place cursor here
      #endif
      sprites[flip].print(time_str);       // Send srite to memroy
     #endif   
   #endif
   #if DISP_SIZE == CUSTOM_DISP          // If using custom display
    #ifndef MEM_OK                       // Compiler directive if not using memory functions
      sprites[flip].setCursor(260, T1_POS ); // Place cursor here
    #endif
    #ifndef SHORT16_OK                   // Compiler directive if full screen
      #ifdef MEM_OK                      // If using memory functions
       sprites[flip].setCursor(184, T1_POS );// Place cursor here
      #endif
    #endif
    #ifdef SHORT16_OK                    // Compiler directivereduce screen by 16 pixels
      #ifdef MEM_OK                      // If using memory functions
       sprites[flip].setCursor(192, T1_POS );// Place Cursor here
      #endif
    #endif
    sprites[flip].print(time_str);       // Send sprite to memory
   #endif  
  #endif
} // end of display time

//-----------------------------------------------------------------------------   
//----------------------------code to handle memory Display 2 -----------------
void display_Mem() {                  //code to display the current memory A, B, or C
 #if  DISP_SIZE == LARGE_DISP || DISP_SIZE == CUSTOM_DISP
  switch (memory) {                         //determine memory position
    case 1: sprintf(mem_str, "Mem:A");break;  //memory A
    case 2: sprintf(mem_str, "Mem:B");break;  //memory B
    case 3: sprintf(mem_str, "Mem:C");break;  //memory C
    case 4: sprintf(mem_str, "Mem:D");break;  //memory C
    case 5: sprintf(mem_str, "Mem:E");break;  //memory C
  } 
 #endif
 #if DISP_SIZE == SMALL_DISP
  switch (memory) {                         //determine memory position
    case 1: sprintf(mem_str, "M:A");break;  //memory A
    case 2: sprintf(mem_str, "M:B");break;  //memory B
    case 3: sprintf(mem_str, "M:C");break;  //memory C
    case 4: sprintf(mem_str, "M:D");break;  //memory C
    case 5: sprintf(mem_str, "M:E");break;  //memory C
  }
 #endif
  sprites[flip].setFont(&fonts::Font4);// Set font size
  sprites[flip].setTextSize(0.75f);    // Set font scale
  sprites[flip].setTextColor(CL_NUM);  // Set font color
  #if  DISP_SIZE == LARGE_DISP         // If large display
    sprites[flip].setCursor(254, T1_POS ); // Place cursor here
    sprites[flip].print(mem_str);      // Sent sprite to memory
  #endif
  #if DISP_SIZE == SMALL_DISP          // If small display
    #ifdef MEM_OK                     // if not using memory functions
     sprites[flip].setTextSize(0.50f);  // Set font scale
     sprites[flip].setCursor(133, T1_POS );// place cursor here
     sprites[flip].print(mem_str);     // Send sprite to memory
    #endif
  #endif
  #if DISP_SIZE == CUSTOM_DISP         // If custom display
    
    #ifndef SHORT16_OK                 // Compiler directive if display full size
     sprites[flip].setCursor(254, T1_POS );// Place cursor
    #endif
    #if PREFERENCE == CLINT
      sprites[flip].setCursor(248, T1_POS );// Place cursor
    #endif
    #ifdef SHORT16_OK                  // Compiler directive if display 16 pixels smaller
     sprites[flip].setCursor(254, T1_POS );// Place cursor here
    #endif
    sprites[flip].print(mem_str);      // Send sprites to memory
  #endif
}

//----------------------------code to handle frequency Display 2 --------------
void display_Freq() {            // Code to display digital frequency 
      char str[12], strl[24];    // strings to manipulate appearance of the digital frequency 
      sprintf(str, "%3d.%03d.%03d",Dial_frq/1000000, (Dial_frq%1000000)/1000, (Dial_frq%1000) );
        // sprintf converts numbers into strings
      int cc=0;                  // integer for manipulating the string
      if(str[0]=='0') {          // gets rid of leading 0
        cc++;                    // increments CC value by 1
        if(str[1]=='0') cc++;    // if 2 leading 0's increment cc vale by 1 again
      }
      for(int i=0; i<12; i++) {  // creates value of i and loops while i is less than 12
        strl[2*i]=str[i+cc];     // moves string from str to strl
        strl[2*i+1]=' ';         // add a space between each letter to stretch out string
      }
  #if  DISP_SIZE == LARGE_DISP               //If large display 240x320
      sprites[flip].drawRoundRect(0,F1_POS,320,40,5,CL_FREQ_BOX); // draw box (x1,y1,x2,y2,thick,color) (15,40)
      sprites[flip].drawRoundRect(1,F1_POS+1,318,38,5,CL_FREQ_BOX); // draw box (x1,y1,x2,y2,thick,color) (15,40)
      sprites[flip].setFont(&fonts::Font6);  // set font type and size to Font6
      sprites[flip].setTextSize(0.55, 0.81); // text size 0.55x, 0.81y 
      sprites[flip].setTextColor(CL_F_NUM);  // text color set in config
      sprites[flip].setCursor(44,F1_POS+6);        // put cursor to position x,y
      sprites[flip].print(strl);             // print string strl
      sprites[flip].setFont(&fonts::Font4);  // set font type and size to Font4
      sprites[flip].setTextSize(1.0f);       // text size 1.0
      sprites[flip].setTextColor(CL_F_NUM);  // text color set in config
      sprites[flip].setCursor(248,F1_POS+15);       // put cursor to position x,y
      if (on_off_flag) sprites[flip].print("MHz"); else sprites[flip].print("Off"); // print text MHz
      sprites[flip].setFont(&fonts::Font4);  // Set font size
      sprites[flip].setTextSize(0.70f);      // set font scale
      sprites[flip].setTextColor(CL_NUM);    // set font color
      sprites[flip].setCursor(5,F1_POS+5);         // put cursor to position x,y
      if (offSet == CWOFFSET) sprites[flip].print("CW"); // if in CW mode, display CW
      else 
      if (slideSwitch) sprites[flip].print("OPP"); else sprites[flip].print("Norm"); // print text OPP/NOR
  #endif
  #if DISP_SIZE == SMALL_DISP                // If small display 128x160
      sprites[flip].drawRoundRect(0,F1_POS,160,30,5,CL_FREQ_BOX); // draw box (x1,y1,x2,y2,thick,color)
      sprites[flip].setFont(&fonts::Font4);  // set font type and size to Font6
      sprites[flip].setTextSize(0.6, 1.0);   // text size 0.31x, 0.48y 
      sprites[flip].setTextColor(CL_F_NUM);  // text color set in config
      sprites[flip].setCursor(28,F1_POS+6);         // put cursor to position x,y
      sprites[flip].print(strl);             // print string strl
      sprites[flip].setFont(&fonts::Font4);  // set font type and size to Font4
      sprites[flip].setTextSize(0.5f);       // text size 1.0
      sprites[flip].setTextColor(CL_F_NUM);  // text color set in config
      sprites[flip].setCursor(132,F1_POS+14);       // put cursor to position x,y
      if (on_off_flag) sprites[flip].print("MHz"); else sprites[flip].print("Off"); // print text MHz
      sprites[flip].setFont(&fonts::Font0);  // set font type and size to Font4
      sprites[flip].setTextSize(1.0f);       // text size 1.0
      sprites[flip].setTextColor(CL_NUM);    // text color set in config
      sprites[flip].setCursor(3,F1_POS+4);          // put cursor to position x,y
      if (offSet == CWOFFSET) sprites[flip].print("CW"); // if in CW mode, display CW
      else 
      if (slideSwitch) sprites[flip].print("Opp"); else sprites[flip].print("Norm"); // print text OPP/NOR
  #endif
  #if DISP_SIZE == CUSTOM_DISP               //Compiler directive 170x320
      #ifdef SHORT16_OK                     // Compiler directive if display full size
       sprites[flip].drawRoundRect(DISP_L,F1_POS,320-DISP_L,40,5,CL_FREQ_BOX); // draw box (x1,y1,x2,y2,thick,color) (0,15,320,40,5,)
      #else                      // Compiler directive if display 16 pixels smaller
       #if PREFERENCE == CLINT
         sprites[flip].drawRoundRect(5,F1_POS,305,40,15,CL_FREQ_BOX); // draw box (x1,y1,x2,y2,thick,color) (0,15,320,40,15,)
       #else
         sprites[flip].drawRoundRect(0,F1_POS,320,40,5,CL_FREQ_BOX); // draw box (x1,y1,x2,y2,thick,color) (0,15,320,40,5,)
         sprites[flip].drawRoundRect(1,F1_POS+1,318,38,5,CL_FREQ_BOX);
       #endif
      #endif
      sprites[flip].setFont(&fonts::Font6);  // set font type and size to Font6
      sprites[flip].setTextSize(0.55, 0.81); // text size 0.55x, 0.81y 
      sprites[flip].setTextColor(CL_F_NUM);  // text color set in config
      #ifndef SHORT16_OK                     // Compiler directive if display full size
       sprites[flip].setCursor(44,F1_POS+5);       // put cursor to position x,y
      #endif
      #ifdef SHORT16_OK                      // Compiler directive if display 16 pixels smaller
      sprites[flip].setCursor(DISP_L+44,F1_POS+5);        // put cursor to position x,y
      #endif
      sprites[flip].print(strl);             // print string strl
      sprites[flip].setFont(&fonts::Font4);  // set font type and size to Font4
      sprites[flip].setTextSize(1.0f);       // text size 1.0
      sprites[flip].setTextColor(CL_F_NUM);  // text color set in config
      #ifndef SHORT16_OK                     // Compiler directive if display full size
        sprites[flip].setCursor(254,F1_POS+15);// put cursor to position x,y
      #endif
      #if PREFERENCE == CLINT
        sprites[flip].setCursor(248,F1_POS+15);// put cursor to position x,y
      #endif
      #ifdef SHORT16_OK                      // Compiler directive if display 16 pixels smaller
       sprites[flip].setCursor(DISP_L+254,F1_POS+15);      // put cursor to position x,y
      #endif
      if (on_off_flag) sprites[flip].print("MHz"); else sprites[flip].print("Off"); // print text MHz
      sprites[flip].setFont(&fonts::Font4);  // Set font size
      sprites[flip].setTextSize(0.70f);      // Set font scale
      sprites[flip].setTextColor(CL_NUM_NORM);    // Set font color
      #ifndef SHORT16_OK                     // Compiler directive if display full size
       sprites[flip].setCursor(10,F1_POS+13);        // put cursor to position x,y
      #endif
      #ifdef SHORT16_OK                      // Compiler directive if display 16 pixels smaller
       sprites[flip].setCursor(DISP_L+6,F1_POS+13);       // put cursor to position x,y
      #endif
      if (offSet == CWOFFSET) sprites[flip].print("CW"); // if in CW mode, display CW
        else
      if (slideSwitch) sprites[flip].print("OPP"); else sprites[flip].print("Norm"); // print text OPP/NOR
  #endif
  
}
  

//------------- Clear display and write Small Display ----------------

void display_write() {                           // procedure to write to the screen
  sprites[flip].clear(BGCol);                    // Clear screen
  //Serial.println(sprite_height);               // Diagnostics
   dial.draw(Dial_frq,0);                        // Send dial to screen
   #ifdef MODE0                                  // If model 0 
     display_Freq();                             // Send frequency to screen
     display_Band();                             // Send band to screen
     display_Step();                             // Send step to screen
     #ifdef CLOCK_OK                             //Compiler directive if using clock
       display_Time();                            // send time to screen
     #endif
     #ifdef MEM_OK                               // If using memory functions
       display_Mem();                             // Send memory to screen
     #endif
   #endif
   #ifdef SHORT16_OK                             // Compiler directive if display 16 pixels smaller
    sprites[flip].fillRect(0,0,DISP_L,180,CL_BLACK); // Blank left 16 pixels of screen
   #endif
   sprites[flip].pushSprite(&lcd, 0, 0);         // Send frequency and 1/2 of dial to screen
   flip = !flip;                                 // clear sprite flag
   dial.draw(Dial_frq,sprite_height);            // draw 2nd 1/2 of dial to screen
   #ifdef SHORT16_OK                             // Compiler directive if display 16 pixels smaller
    sprites[flip].fillRect(0,0,DISP_L,180,CL_BLACK); // Blank left 16 pixels of screen
   #endif
   sprites[flip].pushSprite(&lcd, 0, sprite_height); // Send 2nd half of dial to screen
   flip = !flip;                                 // clear sprite flag
} // End of Display process
