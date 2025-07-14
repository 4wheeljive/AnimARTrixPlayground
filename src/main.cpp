/*
My personal learning playground for working with:
    FastLED Adapter for the animartrix fx library.
    Copyright Stefen Petrick 2023.
    Adapted to C++ by Netmindz 2023.
    Adapted to FastLED by Zach Vorhies 2024.
    For details on the animartrix library and licensing information, 
    see https://github.com/FastLED/FastLED/blob/master/src/fx/2d/animartrix_detail.hpp

This playground operates in one of two modes:

    Using the FastLED web compiler and browser UI controls:

        1.  Ensure you are set up to use FastLED compiler:
            https://github.com/zackees/fastled-wasm/blob/main/README.md
    
        2.  Comment/uncomment the SCREEN_TEST defs below as follows:

            #define SCREEN_TEST
            //#undef SCREEN_TEST

        3.  Run 'fastled' (without quotes) from a command line in the directory where your sketch is saved. 

    Driving an actual LED panel from an MCU, which this sketch sets up as a BLE server to enable web control:
    
        1.  Ensure your MCU is BLE capable and enabled. 

        2.  The default number of handles (15) in BLEServer.h is too low to support the number of characteristics used.
            Edit the following file to set numHandles = 60 :
            C:\Users\...\.platformio\packages\framework-arduinoespressif32\libraries\BLE\src\BLEServer.h

        3.  Comment/uncomment the SCREEN_TEST defs below as follows:

            //#define SCREEN_TEST
            #undef SCREEN_TEST

        4.  Load https://4wheeljive.github.io/AnimARTrixPlayground/ from a Web BLE-capable browser.

            NOTE:   On iOS devices, standard browsers (e.g., Safari, Chrome) do not currently support Web BLE.
                    I've had success using the Bluefy browser:
                    https://apps.apple.com/us/app/bluefy-web-ble-browser/id1492822055
                    If Bluefy doesn't connect to your device on its own, the nRF Connect app might help:
                    https://apps.apple.com/us/app/nrf-connect-for-mobile/id1054362403

When running in the actual LED panel mode, this sketch can accommodate two different setups, 
selected by defining/undefining BIG_BOARD below.

When BIG_BOARD is defined:
- Separate FastLED controllers are enabled for multiple pins to drive multiple panels/strips
- LED mapping will likely need to be done by custom XYMap functions and arrays, which are enabled by default

When BIG_BOARD is undefined:
- One FastLED controller is used for single pin/panel/strip
- Custom LED mapping may be unnecessary
*/

#include <Arduino.h>
#include <stdio.h>
#include <string>

#define FL_ANIMARTRIX_USES_FAST_MATH 1

#include <FastLED.h>
#include "fl/slice.h"
#include "fx/fx_engine.h"

#include "myAnimartrix.hpp"
#include "fl/ui.h"

//*********************************************

#define BIG_BOARD
//#undef BIG_BOARD

//#define SCREEN_TEST
#undef SCREEN_TEST

#define FIRST_ANIMATION RINGS
#define SECONDS_PER_ANIMATION 10

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
    #define HEIGHT 16 
    #define WIDTH 16
    #define NUM_SEGMENTS 1
    #define NUM_LEDS_PER_SEGMENT 256
#endif

//*********************************************

#ifndef SCREEN_TEST

    #include "bleControl.h"

    #ifndef BIG_BOARD
        #include <matrixMap_22x22.h>
        //#include <matrixMap_14x19.h>    
    #else
        #include <matrixMap_32x48_3pin.h>    
    #endif

#else

    #include "fl/json.h"

    // This is purely use for the web compiler to display the animartrix effects.
    // This small led was chosen because otherwise the bloom effect is too strong.
    #define LED_DIAMETER 0.15

#endif

//*********************************************

#define NUM_LEDS ( WIDTH * HEIGHT )

CRGB leds[NUM_LEDS];

using namespace fl;

// MAPPINGS **********************************************************************************

#ifndef SCREEN_TEST
    
    // Custom XYMap mappings

        uint16_t ledNum = 0;
  /*
        extern const uint16_t loc2indSerpByRow[HEIGHT][WIDTH] PROGMEM;
        extern const uint16_t loc2indProgByRow[HEIGHT][WIDTH] PROGMEM;
        extern const uint16_t loc2indSerp[NUM_LEDS] PROGMEM;
        extern const uint16_t loc2indProg[NUM_LEDS] PROGMEM;
        extern const uint16_t loc2indProgByColBottomUp[WIDTH][HEIGHT] PROGMEM;

      
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

Animartrix myAnimartrix(myXYmap, FIRST_ANIMATION);
FxEngine fxEngine(NUM_LEDS);

//**********************************************************************************************

void setColorOrder(int value) {
    switch(value) {
        case 0: value = RGB; break;
        case 1: value = RBG; break;
        case 2: value = GRB; break;
        case 3: value = GBR; break;
        case 4: value = BRG; break;
        case 5: value = BGR; break;
    }
    myAnimartrix.setColorOrder(static_cast<EOrder>(value));
}

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

    fxEngine.addFx(myAnimartrix);

    #ifndef SCREEN_TEST
        bleSetup();
    #endif
    
 }

//************************************************************************************************************

void loop() {
    
    if (!displayOn){
      FastLED.clear();
    }
    else {

        FastLED.setBrightness(adjustBrightness);
        fxEngine.setSpeed(1);
        //fxEngine.setSpeed(timeSpeed);


        static auto lastColorOrder = -1;
        if (adjustColorOrder != lastColorOrder) {
            setColorOrder(adjustColorOrder);
            lastColorOrder = adjustColorOrder;
        } 

        static auto lastFxIndex = -1;
        if (fxIndex != lastFxIndex) {
            lastFxIndex = fxIndex;
            myAnimartrix.fxSet(fxIndex);
        }
        
        fxEngine.draw(millis(), leds);
        
        /*
        if (rotateAnimations) {
            EVERY_N_SECONDS (SECONDS_PER_ANIMATION) { 
                fxIndex += 1 % (NUM_ANIMATIONS - 1);
                animationAdjust(fxIndex);
            }
        }
        */
    }
    
    FastLED.show();

    // BLE CONTROL....

      // while connected
      /*if (deviceConnected) {

      }*/

      // upon disconnect
      if (!deviceConnected && wasConnected) {
        if (debug) {Serial.println("Device disconnected.");}
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising();
        if (debug) {Serial.println("Start advertising");}
        wasConnected = false;
      }

    // ..................

}

//************************************************************************************************************
