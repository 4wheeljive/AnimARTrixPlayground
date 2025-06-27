/// @file    Animartrix.ino
/// @brief   Demo of the Animatrix effects
/// @example Animartrix.ino
///
/// This sketch is fully compatible with the FastLED web compiler. To use it do the following:
/// 1. Install Fastled: `pip install fastled`
/// 2. cd into this examples page.
/// 3. Run the FastLED web compiler at root: `fastled`
/// 4. When the compiler is done a web page will open.
///
/// @author  Stefan Petrick
/// @author  Zach Vorhies (FastLED adaptation)
///

/*
This demo is best viewed using the FastLED compiler.

Windows/MacOS binaries: https://github.com/FastLED/FastLED/releases

Python

Install: pip install fastled
Run: fastled <this sketch directory>
This will compile and preview the sketch in the browser, and enable
all the UI elements you see below.



OVERVIEW:
This is the famouse Animartrix demo by Stefan Petrick. The effects are generated
using polor polar coordinates. The effects are very complex and powerful.
*/




#include <Arduino.h>
#include <stdio.h>
#include <string>

#define FL_ANIMARTRIX_USES_FAST_MATH 1

#include <FastLED.h>
#include "fl/json.h"
#include "fl/slice.h"
#include "fx/fx_engine.h"

#include "myAnimartrix.hpp"
#include "fl/ui.h"

#define BIG_BOARD
//#define SCREEN_TEST

#define FIRST_ANIMATION RINGS
#define SECONDS_PER_ANIMATION 10

#ifndef SCREEN_TEST
    
    #ifdef BIG_BOARD
        #include <matrixMap_32x48_3pin.h>
    #else
        #include <matrixMap_22x22.h>
    #endif

#else

    #include "bleControl.h"

    // This is purely use for the web compiler to display the animartrix effects.
    // This small led was chosen because otherwise the bloom effect is too strong.
    #define LED_DIAMETER 0.15

#endif

#define DATA_PIN_1 2

#ifdef BIG_BOARD
    #define DATA_PIN_2 3
    #define DATA_PIN_3 4
#endif

#ifdef BIG_BOARD
    #define HEIGHT 32 
    #define WIDTH 48
    #define NUM_SEGMENTS 3
    #define NUM_LEDS_PER_SEGMENT 512
#else
    #define HEIGHT 32 
    #define WIDTH 32
    #define NUM_SEGMENTS 1
    #define NUM_LEDS_PER_SEGMENT 1024
#endif

#define NUM_LEDS ( WIDTH * HEIGHT )

CRGB leds[NUM_LEDS];

using namespace fl;

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

/*
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
*/

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

    /*
    colorOrder.onChanged([](int value) {
        switch(value) {
            case 0: value = RGB; break;
            case 1: value = RBG; break;
            case 2: value = GRB; break;
            case 3: value = GBR; break;
            case 4: value = BRG; break;
            case 5: value = BGR; break;
        }
        myAnimartrix.setColorOrder(static_cast<EOrder>(value));
    });
    */

    //myAnimartrix.fxSet(5);

    #ifndef SCREEN_TEST
        bleSetup();
    #endif

 }

//************************************************************************************************************

void loop() {

    /*
    #ifndef SCREEN_TEST
        server.handleClient();
    #endif
    */
    
    if (!displayOn){
      FastLED.clear();
    }
    else {

        FastLED.setBrightness(brightness);
        fxEngine.setSpeed(timeSpeed);
        //myAnimartrix.fxSet(fxIndex);

        static auto lastFxIndex = -1;
        if (fxIndex != lastFxIndex) {
            lastFxIndex = fxIndex;
            myAnimartrix.fxSet(fxIndex);
        }
        
        fxEngine.draw(millis(), leds);
        
        
        if (rotateAnimations) {
            EVERY_N_SECONDS (SECONDS_PER_ANIMATION) { 
                if (nextFxIndexRandom) {fxIndex = random(0, NUM_ANIMATIONS - 1);}
                else {fxIndex += 1 % (NUM_ANIMATIONS - 1);}
            }
        }
        
    }
    
    FastLED.show();

}

//************************************************************************************************************
