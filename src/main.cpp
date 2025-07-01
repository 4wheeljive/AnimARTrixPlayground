
#include <Arduino.h>
#include <stdio.h>
#include <string>

#include <FastLED.h>

//*********************************************

//#define BIG_BOARD
#undef BIG_BOARD

//#define SCREEN_TEST
#undef SCREEN_TEST

#define DATA_PIN_1 2

//*********************************************

#ifdef BIG_BOARD 
    #define DATA_PIN_2 3
    #define DATA_PIN_3 4
    #define HEIGHT 32 
    #define WIDTH 48
    #define NUM_SEGMENTS 3
    #define NUM_LEDS_PER_SEGMENT 512
#else 
    #define HEIGHT 22 
    #define WIDTH 22
    #define NUM_SEGMENTS 1
    #define NUM_LEDS_PER_SEGMENT 484

#endif

//*********************************************

#ifndef SCREEN_TEST

    #include "bleControl.h"

    #ifndef BIG_BOARD
        #include <matrixMap_22x22.h>    
    #else
        #include <matrixMap_32x48_3pin.h>    
    #endif

#endif

//*********************************************

#define NUM_LEDS ( WIDTH * HEIGHT )

CRGB leds[NUM_LEDS];

using namespace fl;

//bool displayOn = true;
//uint8_t BRIGHTNESS = 50;


// MAPPINGS **********************************************************************************

#ifndef SCREEN_TEST
    
    // Custom XYMap mappings

        uint16_t ledNum = 0;

        extern const uint16_t loc2indSerpByRow[HEIGHT][WIDTH] PROGMEM;
        extern const uint16_t loc2indProgByRow[HEIGHT][WIDTH] PROGMEM;
        extern const uint16_t loc2indSerp[NUM_LEDS] PROGMEM;
        extern const uint16_t loc2indProg[NUM_LEDS] PROGMEM;
        extern const uint16_t loc2indProgByColBottomUp[WIDTH][HEIGHT] PROGMEM;

        /*
        uint16_t XY(uint8_t x, uint8_t y) {
            ledNum = loc2indProgByColBottomUp[x][y];
            return ledNum;
        }
        */

        uint16_t myXYFunction(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {
            width = WIDTH;
            height = HEIGHT;
            if (x >= width || y >= height) return 0;
            ledNum = loc2indProgByRow[y][x];
            return ledNum;
        }

        uint16_t myXYFunction(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

        XYMap myXYmap = XYMap::constructWithUserFunction(WIDTH, HEIGHT, myXYFunction);
        //XYMap myXYmap(WIDTH, HEIGHT, true); // use this XYMap instead of one immediately above to bypass custom mapping 

#else

    // Standard XYMap mappings
        
        #define IS_SERPINTINE true

        XYMap myXYmap = XYMap::constructRectangularGrid(WIDTH, HEIGHT);
        //XYMap myXYmap(WIDTH, HEIGHT, IS_SERPINTINE);

#endif

//************************************************************************************************************

void colorChase() {
    for ( uint8_t y = 0 ; y < HEIGHT ; y++ ) {
        for ( uint8_t x = 0 ; x < WIDTH ; x++ ) {
            ledNum = loc2indProgByRow[y][x];
            leds[ledNum] = CHSV(hue,255,255);
            //FastLED.delay(5);
        }
        FastLED.show();
   }
}
//**********************************************************************************************

//**********************************************************************************************

void setup() {

    Serial.begin(115200);

    #ifdef SCREEN_TEST
        auto screen_map = myXYmap.toScreenMap();
        screen_map.setDiameter(LED_DIAMETER);
    #endif

    FastLED.addLeds<WS2812B, DATA_PIN_1, GRB>(leds, 0, NUM_LEDS_PER_SEGMENT)
        #ifdef SCREEN_TEST
            .setScreenMap(screen_map)
        #endif
        .setCorrection(TypicalLEDStrip);

    #ifdef DATA_PIN_2
        FastLED.addLeds<WS2812B, DATA_PIN_2, GRB>(leds, NUM_LEDS_PER_SEGMENT, NUM_LEDS_PER_SEGMENT)
        .setCorrection(TypicalLEDStrip);
    #endif
    
    #ifdef DATA_PIN_3
    FastLED.addLeds<WS2812B, DATA_PIN_3, GRB>(leds, NUM_LEDS_PER_SEGMENT * 2, NUM_LEDS_PER_SEGMENT)
        .setCorrection(TypicalLEDStrip);
    #endif

    #ifndef SCREEN_TEST
        bleSetup();
    #endif

 }

//************************************************************************************************************





void loop() {
    
    if (!displayOn) {
      FastLED.clear();
    }
    else {
      FastLED.setBrightness(BRIGHTNESS);
      colorChase();
    }
        
    //FastLED.show();
    
    /*
    // BLE maintenance
    if (!deviceConnected && wasConnected) {
    if (debug) {Serial.println("Device disconnected.");}
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising();
    if (debug) {Serial.println("Start advertising");}
    wasConnected = false;
    }
    */
}

//************************************************************************************************************
