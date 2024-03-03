# VFO4.1
VFO program for Atlas 210x Ham Radio



This project is a digital VFO for an Atlas 210X Amateur Radio by John Satterfield-KI5IDZ and Clint Chron-W7KEC
It was inspired by software Author: JF3HZB / T.UEBO - https://github.com/tjlab-jf3hzb/Digital_VFO_with_analog_dial_V2

This file is readable by the Arduino IDE and uses ESP32 microcontrollers that can be selected in the config.h file
if MC_TYPE is set to WROVER then the Microcontroller is an ESP32-Wrover LilyGo T7 Ver 1.5

    if MC_TYPE is set to S3ZERO then the Microcontroller is an ESP32-S3-Zero WaveShare
    if MC_TYPE is set to S3R8 then the Microcontroller is an ESP32-S3R8 LilyGo W/Integrated Display
    if MC_TYPE is set to S3MINI then the Microcontroller is an ESP32-S3-Mini Lolin
    if MC_TYPE is set to R8N16 then the Microcontroller is an LVGL ESP32-R8N16 W/integrated Display
    if MC_TYPE is set to T7S3 then the Microcontroller is an ESP32-S3 LilyGo T7 Ver 1.1
    if MC_TYPE is set to S2MINI then the Microcontroller is an ESP32-S2-Mini Lolin

Different parameters can be selected in the config.h file by commenting out or changing the values, colors, etc
Each user can create a PREFERENCE that assigned values to their liking
The band changes in the radio are input via an analog resistor matrix that puts voltage across an analog input pin or the user can select using PCF8574 expand-er module. The analog matrix can be fine tuned by means of a constant BANDDIV
Two different libraries for for controlling the SI5351 module: ETHERKIT or MCU
Different features can be turned on or off by commenting out the #define Feature
The feel of the encoder can be modified by the #define EncoderStep 24 with the number of pulses per encoder step change
The SI5351 module can be calibrated by use of a calibration constant for each of the 2 libraries
The SI5351 module frequency can e changed by a CRYSTAL constant for each of the 2 libraries
The Atlas radio had 2 different crystal filter frequencies over the life of the radio. Either can be selected by a defined constant or the Drake T7 crystal can be selected for those who have had to change out their crystal filter for a Drake.
Three different displays can be chosen via a defined constant:

    If "DISP_SIZE" is set to "SMALL_DISP", then we assume a 128X160 pixel display ST7735
    If "DISP_SIZE" is set to "LARGE_DISP", then we assume a 240x320 pixel display ST7789
    If "DISP_SIZE" is set to "CUSTOM_DISP", then we assume a 170x320 pixel display ST7789
    We prefer the CUSTOM_DISP because the 1.9" display fits perfectly in the Atlas radio
    Constants are provided to manipulate the Analog Dial and the Text and digital frequency display

You will have to load the LovyanGFX library through the Arduino Library Manager. https://github.com/lovyan03/LovyanGFX/tree/master

To download program click on SRC subdirectory and download the code
