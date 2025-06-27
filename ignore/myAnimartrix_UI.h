#pragma once

#include "FastLED.h" 
#include "fl/ui.h" 

using namespace fl;

uint8_t BRIGHTNESS = 125;

bool displayOn = true;

// UI Elements *************************************************************************************

UITitle title("AnimARTrix Playground");
UIDescription description("Test of user-controlled inputs for selected Animartrix effects. @author of fx is StefanPetrick");
UISlider brightness("Brightness", BRIGHTNESS, 0, 255);
UINumberField fxIndex("Animartrix - index", 0, 0, 9); // NUM_ANIMATIONS - 1
UINumberField colorOrder("Color Order", 0, 0, 5);
UISlider timeSpeed("Speed", 1, .1, 10, -1); //multiplied

UISlider adjustRatiosBase("Ratios: Base", 0, -1, 1, -1);  // added/subtracted
UISlider adjustRatiosDiff("Ratios: Diff", 1, .1, 10, -1);  // multiplied

UISlider adjustOffsetsBase("Offsets: Base", 1, .1, 10, -1);  // multiplied
UISlider adjustOffsetsDiff("Offsets: Diff" , 1, .1, 10, -1);  //multiplied

UISlider adjustScale("Scale", 1, .3, 3, -1); //multiplied

UISlider adjustRadiusA("Radius", 0, -10, 30, 1);  // added/subtracted
UISlider adjustRadiusB("Radius", 0, -10, 10, 1);  // added/subtracted
UISlider adjustRadiusC("Radius", 0, -10, 30, 1);  // added/subtracted

UISlider adjustAngle("Angle", 1, .1, 10, -1); //multiplied

UISlider adjustZ("Magic Z", 1, .1, 2, -1); //multiplied

UISlider adjustRed("Red", 1, .1, 10, -1);  //multiplied 
UISlider adjustGreen("Green", 1, .1, 10, -1);   //multiplied
UISlider adjustBlue("Blue", 1, .1, 10, -1);  //multiplied