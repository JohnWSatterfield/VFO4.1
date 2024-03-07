 
#ifndef _CONFIG_H_			      // Prevent double inclusion
#define _CONFIG_H_

#define CLINT      1          // Preference definition CLINT
#define MARK       2          // Preference definition MARK
#define JOHN       3          // Preference definition JOHN

#define ETHERKIT   1          // SI5351 Etherkit Driver
#define MCU        2          // SI5351 MCU Driver

#define ATLAS_CO   1          // Radio using the original carrier oscilator (CO)
#define SI5351_CO  2          // Radio using the si5351 CLK1 for carrier oscilator (CO)

#define TESTING    1          // For testing the display
#define RUN        2          // For normal operation

#define	SMALL_DISP		1	      // 1.77inch - 128x160 display
#define	LARGE_DISP		2	      // 2.0 inch - 240x320 display
#define	CUSTOM_DISP		3	      // 1.9 inch - 172x320 display

#define OLDER 1               //5.5200 MHz crystal Filter for Atlas
#define NEWER 2               //5.6450 MHz crystal Filter for Atlas
#define DRAKE 3               //5.6436 MHz crystal Filter for Drake

#define DIGITAL 1             //PCF8574
#define ANALOG  2             //Resistor matrix

//----------From this point forward you must sed definitions to run properly
//          The Preference section on Line 305-387 should allow you to run

#define PREFERENCE JOHN      //PREFERENCE can be defined as CLINT MARK JOHN

#define BAND_TYPE DIGITAL     // BAND_TYPE is DIGITAL (PCF8574) or ANALOG (resistor matrix)

#define SI5351_DRV MCU   // SI5351 Driver can be ETHERKIT or MCU

//----------- The following are Feature Choices - Comment out if not using-------

//#define CLOCK_OK              //comment out if no clock
//#define LOCK_OK               //comment out if no Lock
//#define SHORT16_OK            //comment out if display normal length
#define MEM_OK                //comment out if not using memory features
//#define SS1PIN_OK           //DIGITAL but using SS1 Pin

//----------- The following are Micro controller choices
#define WROVER 2  //Esp32 Wrover separate serial SPI display
#define S3ZERO 3  //Esp32-S3 Zero separate serial SPI display
#define S3R8   4  //Esp32-S3R8 with LilyGo integrated T-display parallel SPI
#define S3MINI 5  //Esp32-S3 mini separate serial SPI display
#define R8N16  6  //Esp32-S3 R8N16 with generic integrated display serial SPI
#define T7S3   7  //ESP32-S3 LilyGo T7 Ver 1.1 plug in replacement for CLINT
#define S2MINI 8  //Esp32-S2 mini separate serial SPI display
#define RP2040 9  //Waveshare RP2040-zero

/*
 * There are 5 choices for the MC Microcontroller type
 * if MC_TYPE is set to WROVER then the Microcontroller is an ESP32-Wrover LilyGo T7 Ver 1.5
 * if MC_TYPE is set to S3ZERO then the Microcontroller is an ESP32-S3-Zero WaveShare
 * if MC_TYPE is set to S3R8 then the Microcontroller is an ESP32-S3R8 LilyGo W/Integrated Display
 * if MC_TYPE is set to S3MINI then the Microcontroller is an ESP32-S3-Mini Lolin
 * if MC_TYPE is set to R8N16 then the Microcontroller is an LVGL ESP32-R8N16 W/integrated Display
 * if MC_TYPE is set to T7S3 then the Microcontroller is an ESP32-S3 LilyGo T7 Ver 1.1
 * if MC_TYPE is set to S2MINI then the Microcontroller is an ESP32-S2-Mini Lolin
 * if MC_TYPE is set to RP2040 then the Microcntroller is a RP2040-zero
 * 
 */

#define MC_TYPE S3ZERO


//-----------------From this point forward you do not need to set anything
//                 I suggest you use the Preference section on Line 305-387 

/*
 *    Define the EncoderStep here
 */
 
#define EncoderStep  24   //  Encoder pulses needed to advance the encoder 1 step
#define BANDDIV  712      //  DIV routine for analog resistor matrix normally set to 712

/*--------------------------------------------------------
 *   Calibration SI5351  You should calibrate your si5351 chip
 *   The frequency change is inverse to the Correction 
 *   For the EtherKit a change of +116 will lower the frequency by 1 Hz
 *   For the MCU a change of +2.881 will lower the frequency by 1 Hz
 *   I tune on 20m band at 14.250 MHz. Remember the actual frequency is 5.546MHz lower for 20m, 15m, and 10m bands
 *   The actual output frequency of VFO with 5.546MHz CF is 8,605,000 Hz with the 5.546 MHz Carrier Oscillator. 
 *   The actual output frequency of VFO with 5.520MHz CF is 8,730,000 Hz with the 5.546 MHz Carrier Oscillator. 
 *   See the Calibration Target Frequency below in Frequency Settings for each crystal filter
 *   Remember, you are calibrating the Si5351 module, not the radio.  
 *   CORRECTION_MCU = CORRECTION_MCU + (ACTUAL_FREQ-DESIRED_FREQ)*2.881)  FOR THE MCU LIBRARY
 *   CORRECTION = CORRECTION + (ACTUAL_FREQ-DESIRED_FREQ)*116) FOR THE ETHERKIT LIBRARY
----------------------------------------------------------*/

#define CORRECTION      0ULL      //tuned on 20m at 14.250 - 8605000 typically 116 x freq = correction change
#define CORRECTION_MCU    0       //tuned on 20m at 14.250 - 8605000 typically 2.881 x freq = correction change
#define CRYSTAL      25000000     // use 27000000 for QRP Labs (27MHz crystal) or 0 for generic module (25 MHz crystal)
#define CRYSTAL_MCU  25000000     // use 27000000 for QRP Labs or 25000000 for generic module

//Calculated how low the output frequency is / go above frequency tunes better
//Radio    Etherkit   MCU
//TK7490 - 40600ULL    504  5.52 MHz C.F. Atlas C.O.
//Test3  -152500ULL   3124
//Test4  - 74200ULL   1217  GUESS
//Test5  - 74300ULL   1155/S2  1852/S3 
//Test6  - 59660ULL    938
//Test7  -157860ULL   3945
//TM8736 - 69200ULL   1127  5.645 MHz C.F. Si5351 C.O.  T2 
//TM???? -  3300ULL   -450         moved Test1 to this radio
//TM???  - 60403ULL    964  GUESS  for radio with Drake Crystal Filter
//Feel free to add your serial number and calibration constant here

#define CF NEWER                  //OLDER for 5520 CO, NEWER for 5645 CO and DRAKE for Drake CF

/*-------------------------------------------------------
   Frequency settings
--------------------------------------------------------*/
#if CF == OLDER
#define IF       5520000      //Std. Crystal Filter 5520000;
#define OPPFREQ  5523300      //Std. Crystal Filter 5523300; 
#define CWFREQ   5521200      //Std. Crystal Filter 5521200; 
#define RFFREQ   8730000      //Std. Crystal Filter 8730000; Calibration Target Frequency
#define DFREQ   14250000      //Default startup frequency using 20m band 
#endif

#if CF == NEWER
#define IF       5645000      //Std. Crystal Filter 5645000;
#define OPPFREQ  5648300      //Std. Crystal Filter 5648300;
#define CWFREQ   5646200      //Std. Crystal Filter 5646200;
#define RFFREQ   8605000      //Std. Crystal Filter 8605000;  Calibration Target frequency
#define DFREQ   14250000      //Default startup frequency using 20m band 
#endif

#if CF == DRAKE
#define IF       5643600      //Std. Crystal Filter 5643600; 
#define OPPFREQ  5646400      //Std. Crystal Filter 5646400; 
#define CWFREQ   5644800      //Std. Crystal Filter 5644800; 
#define RFFREQ   8606400      //Std. Crystal Filter 8606400;  Calibration Target frequency 
#define DFREQ   14250000      //Default startup frequency using 20m band 
#endif

#define  OFFSET 3300                //off set for OPP SSB Mode Std. Crystal Filter 3300; Drake Crystal Filter 2800
#define  CWOFFSET 1200              //off set for CW Mode

/*
 *  There are three choices for the size of the display being used ("DISP_SIZE"):
 *
 *    If "DISP_SIZE" is set to "SMALL_DISP", then we assume a 128x160 pixel display.
 *    If "DISP_SIZE" is set to "LARGE_DISP", then we assume a 240x320 pixel display.
 *    If "DISP_SIZE" is set to "CUSTOM_DISP", the user can create a 'logical' (large)
 *    display size as I have below for my Atlas 210X
 *
 *  Many of the locations of things on the screen are conditionalized based on the
 *  screen size. The fonts used on the splash screen also vary with the screen size.
 */

#define DISP_SIZE CUSTOM_DISP      // SMALL_DISP  CUSTOM_DISP  LARGE_DISP


/*
 * There are three choices for the crystal filter for the Atlas Radio
 * The first filter used was 5.520 MHz that was later replaced by the
 * 5.6450 MHz filter.  The third is for the Drake Transceiver at 5.6450 
 * MHz but is is different in that The Atlas Radio filters on the edge
 * of 5.6450 MHz while the center of the filter of the Drake radio is
 * 5.6450 MHz. If "CO" is set to OLDER the IF frequency is 5.520 MHz
 * IF "CO" is set to NEWER the IF frequency is 5.6450 MHz and if "CO"
 * is set to DRAKE the IF frequency is 5.6436 MHz.
 * 
 */
 

/*--------------------------------------------------------
   Splash Screen Startup intro - Version Number
----------------------------------------------------------*/
#define NAME "VFO System"     //Start-up Intro with version number
#define VERSIONID "VFO4.1 - Ver. 9.62" //version ID number
#define ID "by KI5IDZ"        //ID writer of program code



#define	BIT_RATE 115200

/*
 * There are two choices for the STATUS  TESTING and RUN
 * IF "STATUS" is set to TESTING then additional diagnostics is activated. The display
 * will operate normally, External input can be tested. Controller is held in Norm,
 * with the on_off_flag held high without VFO input and encoder will function
 * IF "STATUS" is set to RUN then the radio will operate normally
 */

#define P_STATUS TESTING              // TESTING   RUN

/*
 * There are two choices for the carrier oscilator for the Radio
 * IF "CO" is set to ATLAS_CO then the radio is using the original internal CO
 * IF "CO" is set to SI5351_CO then the radio is using the si5351 CLK1 for the CO
 */

#define CO ATLAS_CO             //Internal CO ATLAS_CO, si5351 CO SI5351_CO



#if DISP_SIZE == SMALL_DISP       // Define things for the ST7785 small display (128x160)

  #define DISP_W       160        // Display width (in landscape mode)
  #define DISP_H       128        // Display height
  #define DISP_L         0        // Dial Display begins to center dial
  #define DISP_TM        0        // Top Margin moves Dial up and down
	
  #define  D_R         200        // Dial radius (if 45000, Linear scale)
  #define  DIAL_FONT   0.4        // Font -  0, 1, or 2 (Defaults to '0' in "dial.cpp")
  #define  DIAL_SPACE   30        // Number of pixels between the main and sub arcs

  #define  DP_WIDTH      1        // Width of Dial ponter
  #define  DP_LEN       60        // Normal length of Dial pointer
  #define DP_POS         0        // Position of Dial pointer

  #define F1_POS        10        // Vertical Position of the frequency box
  #define T1_POS        44        // Align the secondary text information on this line
   
	
#endif

#if DISP_SIZE == LARGE_DISP       // Define things for the large display (240x320)

  #define DISP_W        320       // Display width (in landscape mode)
  #define DISP_H        240       // Display height
  #define DISP_L          0       // Dial Display begins to center dial
  #define DISP_TM        60       // Top Margin moves Dial up and down
	
  #define D_R           250      // Dial radius (if 45000, Linear scale)
  #define DIAL_FONT    0.65      // Font -  0, 1, or 2 (Defaults to '0' in "dial.cpp")
  #define DIAL_SPACE     45      // Number of pixels between the main and sub arcs

  #define DP_WIDTH        1      // Width of Dial pointer 0,1 OR 2
  #define DP_LEN        180      // Length of Dial pointer
  #define DP_POS         10      // Length Dial pointer extends above dial

  #define F1_POS         15      // Vertical Position of the frequency box
  #define T1_POS         60      // Align the secondary text information on this line

#endif

#if DISP_SIZE == CUSTOM_DISP     // Define things John's display (1.9" display 170x320)

  #define DISP_W         320     // Display width in landscape mode  260 280 300 320
  #define DISP_H         170     // Display height full height
  #define DISP_L          16     // Dial Display offset from center Normally set from 0 set to 16 for short 16 pixels
  #define DISP_TM         30     // Top Margin moves Dial up and down
	
  #define D_R            250     // Dial radius (if 45000, Linear scale)
  #define DIAL_FONT     0.65     // Dial Font width is multiplied by DIAL_FONT height is proportional to width
  #define DIAL_SPACE      45     // Number of pixels between the main and sub arcs

  #define DP_WIDTH         1     // Width of Dial pointer
  #define DP_LEN         100     // Length of Dial pointer
  #define DP_POS          10     // Length Dial pointer extends above dial
	
  #define F1_POS          24     // Vertical Position of the frequency box 14 60    32,14
  #define T1_POS          70     // Align the secondary text information on this line

#endif

/*
 *	The following are the things that the user can change to modify the appearance
 *	and/or behavior of the dial itself. They were moved from the original "dial_prm.h"
 *	and "display.h"	files and converted from variables to definitions.
 */

#define   F_REV            1       // Frequency increases CW if '1'; CCW (ACW) if '0'
#define   F_MAIN_OUTSIDE   0       // 0 - Main dial is inside;  1 - Main dial is outside

/*
 *	Changing this to anything other than "10000" does weird things with the
 *	displayed numerical values on both dial scales. Still needs to be investigated.
 */

#define   FREQ_TICK_MAIN   10000   // Tick labels on main dial (if 10000: 10kHz / else： 100kHz)
#define   FREQ_TICK_SUB     1000   // Tick labels on sub dial
#define   TICK_PITCH_MAIN    9.0   // Main Tick Pitch (note small changes make a big difference)
#define   TICK_PITCH_SUB     9.5   // Sub Tick Pitch  (try not to go below 4.0)

#define   TICK_WIDTH1        3.0    // Width of Tick
#define   TICK_WIDTH2        4.0    // Width of Tick
#define   TICK_WIDTH3        4.5    // Width of Tick

#define   TICK_MAIN1           5    // Length of Main Tick(1)
#define   TICK_MAIN5          14    // Length of Main Tick(5)
#define   TICK_MAIN10         18    // Length of Main Tick(10)

#define   TICK_SUB1            5    // Length of Sub Tick(1)
#define   TICK_SUB5           14    // Length of Sub Tick(5)
#define   TICK_SUB10          18    // Length of Sub Tick(10)

#define   TNCL_MAIN           22    // Space between Number and Tick (Main)
#define   TNCL_SUB            22    // Space between Number and Tick (Sub)


/*
 *	Let's define some colors that are used to draw things. Feel free to add to the
 *	list. The following link is a handy tool for selecting colors and getting the
 *	numerical color codes in various formats:
 *
 *						https://www.tydac.ch/color/
 *
 *	These definitions are the 24 bit color codes in red, green, blue format.
 */

#define   CL_GREY     0xA0A0A0UL		// 
#define   CL_BLACK    0x000000UL		// 
#define   CL_WHITE    0xFFFFFFUL		// 
#define   CL_RED      0xFF0000UL		// 
#define   CL_GREEN    0x00FF00UL		// 
#define   CL_LT_BLUE  0x00FFFFUL		// 
#define   CL_ORANGE   0xFFD080UL		// 
#define   CL_CYAN     0x00FFFFUL
#define   CL_BLUE     0x0000FFUL
#define   CL_YELLOW   0xFFFF00UL
#define   CL_SKYBLUE  0x87CEEBUL
#define   CL_AQUA     0x1B73A1UL
#define   CL_TOMATO   0xCA261CUL
#define   CL_LBLUE    0X0048A4UL

/*
 *	The following list associates the various components of the display with the
 *	colors we want them to be display as.
 */

#if PREFERENCE == CLINT
#define   CL_BG          CL_BLACK    // Display background (Black)
#define   CL_POINTER     CL_RED      // Dial pointer (Red)
#define   CL_TICK_MAIN   CL_GREEN    // Main Ticks (Lime green)
#define   CL_NUM_MAIN    CL_WHITE    // Main dial numbers (White)
#define   CL_TICK_SUB    CL_SKYBLUE  // Sub Ticks (Light blue)
#define   CL_NUM_SUB     CL_WHITE    // Sub Numbers (White)
#define   CL_DIAL_BG     CL_BLACK    // Dial background (Black)
#define   CL_SPLASH      CL_LT_BLUE  // Splash screen text
#define   CL_FREQ_BOX    CL_CYAN     // Numerical frequency box
#define   CL_F_NUM       CL_ORANGE   // Numerical frequency
#define   CL_NUM         CL_YELLOW   // Numerical small numbers
#define   CL_NUM_O       CL_RED      // Step color in CUSTOM_DISP
#define   CL_NUM_NORM    CL_WHITE    // Normal Text inside box
#define   DP_POS           0         // Length Dial pointer extends above dial
#define   DIAL_SPACE      40         // Number of pixels between the main and sub arcs
#define   TICK_SUB1        8         // Length of Sub Tick(1)
#define   TICK_SUB5       14         // Length of Sub Tick(5)
#define   TICK_SUB10      18         // Length of Sub Tick(10)
#define   TICK_MAIN1       4         // Length of Main Tick(1)
#define   TICK_MAIN5      14         // Length of Main Tick(5)
#define   TICK_MAIN10     18         // Length of Main Tick(10)
#define   TNCL_MAIN       18         // Space between Number and Tick (Main)
#define   TNCL_SUB        18         // Space between Number and Tick (Sub)
#define TICK_PITCH_MAIN 10.5         // Main Tick Pitch (note small changes make a big difference)
#define TICK_PITCH_SUB   9.8         // Sub Tick Pitch  (try not to go below 4.0)
#define CORRECTION       0ULL
#define CORRECTION_MCU   0
#define EncoderStep     12
#define CO ATLAS_CO
#define LOCK_OK
#undef  CLOCK_OK
#undef  SHORT16_OK
#endif

#if PREFERENCE == JOHN
#define   CL_BG         CL_BLACK      // Display background (Black)
#define   CL_POINTER    CL_RED        // Dial pointer (Red)
#define   CL_TICK_MAIN  CL_SKYBLUE    // Main Ticks (Lime green)
#define   CL_NUM_MAIN   CL_BLUE       // Main dial numbers (White)
#define   CL_TICK_SUB   CL_SKYBLUE    // Sub Ticks (Light blue)
#define   CL_NUM_SUB    CL_YELLOW     // Sub Numbers (White)
#define   CL_DIAL_BG    CL_BLACK      // Dial background (Black)
#define   CL_SPLASH     CL_LT_BLUE    // Splash screen text
#define   CL_FREQ_BOX   CL_YELLOW     // Numerical frequency box
#define   CL_F_NUM      CL_CYAN       // Numerical frequency
#define   CL_NUM        CL_YELLOW     // Numerical small numbers
#define   CL_NUM_O      CL_YELLOW     // Step color in CUSTOM_DISP
#define   CL_NUM_NORM   CL_YELLOW     // Normal Text inside box
#define BAND_TYPE DIGITAL 
#define EncoderStep  24
#undef  LOCK_OK
#endif

#if PREFERENCE == MARK
#define   CL_BG         CL_BLACK      // Display background (Black)
#define   CL_POINTER    CL_RED        // Dial pointer (Red)
#define   CL_TICK_MAIN  CL_SKYBLUE    // Main Ticks (Lime green)
#define   CL_NUM_MAIN   CL_BLUE       // Main dial numbers (White)
#define   CL_TICK_SUB   CL_SKYBLUE    // Sub Ticks (Light blue)
#define   CL_NUM_SUB    CL_YELLOW     // Sub Numbers (White)
#define   CL_DIAL_BG    CL_BLACK      // Dial background (Black)
#define   CL_SPLASH     CL_LT_BLUE    // Splash screen text
#define   CL_FREQ_BOX   CL_YELLOW     // Numerical frequency box
#define   CL_F_NUM      CL_CYAN       // Numerical frequency
#define   CL_NUM        CL_YELLOW     // Numerical small numbers
#define   CL_NUM_O      CL_YELLOW     // Step color in CUSTOM_DISP
#define   CL_NUM_NORM   CL_YELLOW     // Normal Text inside box
#define DISP_SIZE SMALL_DISP
#define BAND_TYPE DIGITAL 
#define EncoderStep  24
#undef  MEM_OK
#undef  CLOCK_OK
#undef  LOCK_OK
#endif






//------------Defined MC_TYPE---------------------------

#if MC_TYPE == WROVER         //Compiler directive for Lilygo T7 Ver 1.5 
#define INTERRUPTED_PIN 16    // user defined connection to PCF8574 interrupt pin
#define cw        2           //mode slector in cw position
#define DT       25           // DT Encoder A pin, connected to ESP32 pin 4 (GPIO4)
#define CLK       4           // CLK Encoder B pin, connected to ESP32 pin 25 (GPIO25)
#define MEM      33           //memory
#define STEP     34           //step
#define SCAN     35           //scan (not in use) 
#define TSDA     21           //Default pins defined in si5351.cpp  
#define TSCL     22           //Default pins defined in si5351.cpp
#define VFO      26           //Turn internal VFO on_off uses uses jumper on 9 pin plug
#define SS1      19           //Norm/opp side band position (pin 13 is inside and pin 12 is outside)
#define BAND     27           //Analog input from resistor network 80m 3243, 40m 2431, 20m 1622, 15m 813, 10m 0
//#define TX1                 //Transmit sig input to bypass LP Filter (not in use) 
#define OPT      14           //Free for opt_change function
#define TFT_MISO -1           //     13 or 37 Not connected -1
#define TFT_CS    5           //     10 or 34 or 5
#define TFT_MOSI 23           //     11 or 36 or 23
#define TFT_SCLK 18           //     12 or 38 0r 18
#define TFT_DC   15           //     14 or 35 or 15
#define TFT_RST  -1           //     14 or 44 or -1 
#define LOCK     32           //Lock - To lock the frequency
#endif

#if MC_TYPE == S3ZERO
#define INTERRUPTED_PIN 1       //defined in PCF8574inputpins interrupt pin
#define   cw       2            //mode slector in cw position
#define   CLK      4            //Defined in encodersetup.h A-CLK
#define   DT       6            //Defined in encodersetup.h B-DT
#define   MEM      3            //memory
#define   STEP     5            //step
#define   SCAN    44            //scan 
#define   TSDA     8            //Default pins for esp32-s2 used by wire.h
#define   TSCL     9            //Default pins for esp32-s2 used by wire.h
//#define   TX1                 //Transmit sig input to bypass LP Filter (not in use)  
#define LOCK -1
#define   OPT     43            //Free for opt_change function
#define   VFO      7            //Turn internal VFO on_off uses uses jumper on 9 pin plug
//#define TFT_BL   15   // LED back-light
#define TFT_MISO   -1   //     13 or 37 Not connected -1
#define TFT_CS     10   //     10 or 34
#define TFT_MOSI   11   //     11 or 36
#define TFT_SCLK   12   //     12 or 38
#define TFT_DC     13   //     14 or 35
#define TFT_RST    -1   //     14 or 44 Not connected -1
#endif   

#if MC_TYPE == RP2040
#define INTERRUPTED_PIN 29       //defined in PCF8574inputpins interrupt pin
#define   cw      28            //mode slector in cw position
#define   CLK     15            //Defined in encodersetup.h A-CLK
#define   DT      14            //Defined in encodersetup.h B-DT
#define   MEM     12            //memory
#define   STEP    13            //step
#define   SCAN     0            //scan 
#define   TSDA     8            //Default pins for esp32-s2 used by wire.h
#define   TSCL     9            //Default pins for esp32-s2 used by wire.h
//#define   TX1                 //Transmit sig input to bypass LP Filter (not in use)  
#define LOCK -1
#define   OPT      1            //Free for opt_change function
#define   VFO      7            //Turn internal VFO on_off uses uses jumper on 9 pin plug
//--------------------------Following pins defined in pcf8574inputpins.h--------------
//pcf8574.pinMode(P0, INPUT_PULLUP);// Input for 80m band
//pcf8574.pinMode(P1, INPUT_PULLUP);// Input for 40m band
//pcf8574.pinMode(P2, INPUT_PULLUP);// Input for 20m band
//pcf8574.pinMode(P3, INPUT_PULLUP);// Input for 15m band
//pcf8574.pinMode(P4, INPUT_PULLUP);// Input for 10m band
//pcf8574.pinMode(P5, OUTPUT, LOW); // Output for MUX0
//pcf8574.pinMode(P6, OUTPUT, LOW); // Output for MUX1
//pcf8574.pinMode(P7, INPUT_PULLUP);// Input for SS1 select button
//#define TFT_BL   15   // LED back-light
#define TFT_MISO   -1   //     13 or 37 Not connected -1
#define TFT_CS      5   //     10 or 34
#define TFT_MOSI    3   //     11 or 36
#define TFT_SCLK    2   //     12 or 38
#define TFT_DC      4   //     14 or 35
#define TFT_RST    -1   //     14 or 44 Not connected -1
#endif   


#if MC_TYPE == S3R8
#define PIN_POWER_ON 15  // LCD and battery Power Enable
#define INTERRUPTED_PIN 18      //PCF8574inputpins interrupt pin
#define   cw       2            //mode slector in cw position
#define   CLK     17            //Defined in encodersetup.h A-CLK
#define   DT      21            //Defined in encodersetup.h B-DT
#define   MEM      3            //memory
#define   STEP    10            //step
#define   SCAN    11            //scan 
#define   TSDA     43            //Default pins for esp32-s2 used by wire.h
#define   TSCL     44            //Default pins for esp32-s2 used by wire.h
#define   LOCK    -1
//#define   TX1                 //Transmit sig input to bypass LP Filter (not in use)  
#define   OPT     13             //OPT for opt_change function
#define   VFO      1            //Turn internal VFO on_off uses uses jumper on 9 pin plug
//--------------------------Following pins defined in pcf8574inputpins.h--------------
//pcf8574.pinMode(P0, INPUT_PULLUP);// Input for 80m band
//pcf8574.pinMode(P1, INPUT_PULLUP);// Input for 40m band
//pcf8574.pinMode(P2, INPUT_PULLUP);// Input for 20m band
//pcf8574.pinMode(P3, INPUT_PULLUP);// Input for 15m band
//pcf8574.pinMode(P4, INPUT_PULLUP);// Input for 10m band
//pcf8574.pinMode(P5, OUTPUT, LOW); // Output for MUX0
//pcf8574.pinMode(P6, OUTPUT, LOW); // Output for MUX1
//pcf8574.pinMode(P7, INPUT_PULLUP);// Input for SS1 select button
#define TFT_D0 39
#define TFT_D1 40
#define TFT_D2 41
#define TFT_D3 42
#define TFT_D4 45
#define TFT_D5 46
#define TFT_D6 47
#define TFT_D7 48
#define TFT_CS  6
#define TFT_DC  7
#define TFT_WR  8
#define TFT_RD  9
#define TFT_RST 5
#define TFT_BL 38

#endif

#if MC_TYPE == S3MINI
#define INTERRUPTED_PIN 16      // user defined connection to PCF8574 interrupt pin
#define   cw       3            //mode slector in cw position
#define   DT       6            // CLK pin, connected to ESP32 pin A
#define   CLK      5            // DT pin, connected to ESP32 pin B
#define   MEM      2            //memory
#define   STEP     4            //step
#define   SCAN    12            //scan 
#define   TSDA     7       
#define   TSCL    13           
#define   TX1      8            //Transmit sig input to bypass LP Filter (not in use)  
#define   OPT      9            //OPT for opt_change function
#define   VFO     10            //Turn internal VFO on_off uses uses jumper on 9 pin plug
//--------------------------Following pins defined in pcf8574inputpins.h--------------
//pcf8574.pinMode(P0, INPUT_PULLUP);// Input for 80m band
//pcf8574.pinMode(P1, INPUT_PULLUP);// Input for 40m band
//pcf8574.pinMode(P2, INPUT_PULLUP);// Input for 20m band
//pcf8574.pinMode(P3, INPUT_PULLUP);// Input for 15m band
//pcf8574.pinMode(P4, INPUT_PULLUP);// Input for 10m band
//pcf8574.pinMode(P5, OUTPUT, LOW); // Output for MUX0
//pcf8574.pinMode(P6, OUTPUT, LOW); // Output for MUX1
//pcf8574.pinMode(P7, INPUT_PULLUP);// Input for SS1 select button
#define TFT_CS   34 //     10 or 34 34
#define TFT_MOSI 36 //     11 or 36 35
#define TFT_SCLK 38 //     12 or 38 36
#define TFT_DC   35 //     14 or 35 33
#define TFT_RST  44 //     14 or 44 37
#endif



#if MC_TYPE == T7S3
#define INTERRUPTED_PIN 15      // user defined connection to PCF8574 interrupt pin
#define   cw       2            //mode slector in cw position
#define   DT       4            // CLK pin, connected to ESP32 pin A
#define   CLK      12           // DT pin, connected to ESP32 pin B
#define   MEM      46           //memory
#define   STEP     45           //step
#define   SCAN     47           //scan 
#define   TSDA     13           //SDA
#define   TSCL     14           //SCL
//#define   TX1                   //Transmit sig input to bypass LP Filter (not in use)  
#define   OPT      14           //OPT for opt_change function
#define   VFO      16           //Turn internal VFO on_off uses uses jumper on 9 pin plug
#define   SS1      17           //Norm/opp side band position
#define   BAND      3           //Analog input from resistor network
#define   LOCK     38           //Lock - To lock the frequency
//following pins are for the display

#define TFT_CS   34 //     10 or 34
#define TFT_MOSI 36 //     11 or 36
#define TFT_SCLK 38 //     12 or 38
#define TFT_DC   35 //     14 or 35
#define TFT_RST  44 //     14 or 44
#endif

#if MC_TYPE == R8N16  //if MC_TYPE is set to R8N16 then it is an ESP32-R8N16 W/integrated Display
#define INTERRUPTED_PIN 42      // user defined connection to PCF8574 interrupt pin
#define   DT       4            // CLK pin, connected to ESP32 pin 6 (GPIO4) A
#define   CLK      6            // DT pin, connected to ESP32 pin 4 (GPIO25) B           
#define   cw       5            //mode slector in cw position
#define   VFO      7            //Turn internal VFO on_off uses uses jumper on 9 pin plug
#define   MEM     17            //memory
#define   STEP    16            //step
#define   SCAN    15            //scan 
#define   OPT     19            //Optional input pin
#define   TSDA     40            //Default pins for esp32-s2 used by wire.h
#define   TSCL     41            //Default pins for esp32-s2 used by wire.h
//#define   TX1                 //Transmit sig input to bypass LP Filter (not in use)  
//#define TFT_BACKLIGHT_ON 1
//#define TFT_INVERSION_ON
//----------------------Following pins defined in pcf8574inputpins.h--------------
//pcf8574.pinMode(P0, INPUT_PULLUP);// Input for 80m band
//pcf8574.pinMode(P1, INPUT_PULLUP);// Input for 40m band
//pcf8574.pinMode(P2, INPUT_PULLUP);// Input for 20m band
//pcf8574.pinMode(P3, INPUT_PULLUP);// Input for 15m band
//pcf8574.pinMode(P4, INPUT_PULLUP);// Input for 10m band
//pcf8574.pinMode(P5, OUTPUT, LOW); // Output for MUX0
//pcf8574.pinMode(P6, OUTPUT, LOW); // Output for MUX1
//pcf8574.pinMode(P7, INPUT_PULLUP);// Input for SS1 select button
//following pins are for the display
#define TFT_BL   14 //     Back light
#define TFT_MISO -1 //     Normally set to -1 if not using
#define TFT_CS   10 //     10 or 34
#define TFT_MOSI 13 //     11 or 36
#define TFT_SCLK 12 //     12 or 38
#define TFT_DC   11 //     14 or 35
#define TFT_RST   1 //     14 or 44 
#endif

#if MC_TYPE == S2MINI
#define INTERRUPTED_PIN 16      // user defined connection to PCF8574 interrupt pin
#define   cw       2            //mode slector in cw position
#define   DT       6            // CLK pin, connected to ESP32 pin A
#define   CLK      4            // DT pin, connected to ESP32 pin B
#define   MEM      3            //memory
#define   STEP     5            //step
#define   SCAN     7            //scan 
#define   TSDA     8       
#define   TSCL     9           
#define   TX1     10            //Transmit sig input to bypass LP Filter (not in use)  
#define   OPT     13            //OPT for opt_change function
#define   VFO     12            //Turn internal VFO on_off uses uses jumper on 9 pin plug
//--------------------------Following pins defined in pcf8574inputpins.h--------------
//pcf8574.pinMode(P0, INPUT_PULLUP);// Input for 80m band
//pcf8574.pinMode(P1, INPUT_PULLUP);// Input for 40m band
//pcf8574.pinMode(P2, INPUT_PULLUP);// Input for 20m band
//pcf8574.pinMode(P3, INPUT_PULLUP);// Input for 15m band
//pcf8574.pinMode(P4, INPUT_PULLUP);// Input for 10m band
//pcf8574.pinMode(P5, OUTPUT, LOW); // Output for MUX0
//pcf8574.pinMode(P6, OUTPUT, LOW); // Output for MUX1
//pcf8574.pinMode(P7, INPUT_PULLUP);// Input for SS1 select button
#define TFT_MISO -1 //
#define TFT_CS   34 //     10 or 34 or 34
#define TFT_MOSI 35 //     11 or 36 or 35
#define TFT_SCLK 36 //     12 or 38 or 36
#define TFT_DC   33 //     14 or 35 or 33
#define TFT_RST  37 //     14 or 44 or 37
#endif


#endif    //_CONFIG_H_
