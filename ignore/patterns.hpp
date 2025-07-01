#pragma once

#include "bleControl.h"

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