#pragma once

#include "FastLED.h" 
#include "fl/ui.h"
#include <ArduinoJson.h> 

/* Be sure to set numHandles = 60 in this file:
C:\Users\...\.platformio\packages\framework-arduinoespressif32\libraries\BLE\src\BLEServer.h
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>

uint8_t BRIGHTNESS = 125;

bool displayOn = true;

bool debug = true;

bool nextFxIndexRandom = false;
bool rotateAnimations = true;
bool colorOrderChanged = false;
double initialFxIndex = 4;   // this should really be changed to uint8_t, but UINumberField requires that it be a double


// UI Elements *************************************************************************************

#ifdef SCREEN_TEST

   using namespace fl;

   UITitle title("AnimARTrix Playground");
   UIDescription description("Test of user-controlled inputs for selected Animartrix effects. @author of fx is StefanPetrick");
   UISlider brightness("Brightness", BRIGHTNESS, 0, 255);
   UINumberField fxIndex("Animartrix - index", initialFxIndex, 0, 9); // NUM_ANIMATIONS - 1
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

#else

   using namespace ArduinoJson;

   uint8_t brightness = BRIGHTNESS;
   double fxIndex = initialFxIndex;        // this should really be changed to uint8_t, but UINumberField requires that it be a double
   double colorOrder = 0;                   // this should really be changed to uint8_t, but UINumberField requires that it be a double

   float timeSpeed = 1.f; //("Speed", 1, .1, 10, -1) multiplied

   float adjustRatiosBase = 0.0f; //("Ratios: Base", 0, -1, 1, -1);  // added/subtracted
   float adjustRatiosDiff= 1.f; // ("Ratios: Diff", 1, .1, 10, -1);  // multiplied

   float adjustOffsetsBase = 1.f; //("Offsets: Base", 1, .1, 10, -1);  // multiplied
   float adjustOffsetsDiff = 1.f; //("Offsets: Diff" , 1, .1, 10, -1);  //multiplied

   float adjustScale = 1.f; // ("Scale", 1, .3, 3, -1); //multiplied

   float adjustRadiusA = 0.0f; // ("Radius", 0, -10, 30, 1);  // added/subtracted
   float adjustRadiusB = 0.0f; // ("Radius", 0, -10, 10, 1);  // added/subtracted
   float adjustRadiusC = 0.0f; // ("Radius", 0, -10, 30, 1);  // added/subtracted

   float adjustAngle = 1.f; // ("Angle", 1, .1, 10, -1); //multiplied

   float adjustZ = 1.f; // ("Magic Z", 1, .1, 2, -1); //multiplied

   float adjustRed = 1.f; // ("Red", 1, .1, 10, -1);  //multiplied 
   float adjustGreen = 1.f; // ("Green", 1, .1, 10, -1);   //multiplied
   float adjustBlue = 1.f; // ("Blue", 1, .1, 10, -1);  //multiplied

   ArduinoJson::JsonDocument sendDoc;
   ArduinoJson::JsonDocument confirmDoc;

#endif

//BLE configuration *************************************************************

BLEServer* pServer = NULL;
BLECharacteristic* pAnimationCharacteristic = NULL;
BLECharacteristic* pColorCharacteristic = NULL;
BLECharacteristic* pSpeedCharacteristic = NULL;
BLECharacteristic* pScaleCharacteristic = NULL;
BLECharacteristic* pControlCharacteristic = NULL;
bool deviceConnected = false;
bool wasConnected = false;

#define SERVICE_UUID                  	"19b10000-e8f2-537e-4f6c-d104768a1214"
#define ANIMATION_CHARACTERISTIC_UUID  "19b10001-e8f2-537e-4f6c-d104768a1214"
#define COLOR_CHARACTERISTIC_UUID   	"19b10002-e8f2-537e-4f6c-d104768a1214"
#define SPEED_CHARACTERISTIC_UUID      "19b10003-e8f2-537e-4f6c-d104768a1214"
#define SCALE_CHARACTERISTIC_UUID 		"19b10004-e8f2-537e-4f6c-d104768a1214"
#define CONTROL_CHARACTERISTIC_UUID    "19b10005-e8f2-537e-4f6c-d104768a1214"

/*
BLEDescriptor pAnimationDescriptor(BLEUUID((uint16_t)0x2901));
BLEDescriptor pColorDescriptor(BLEUUID((uint16_t)0x2902));
BLEDescriptor pSpeedDescriptor(BLEUUID((uint16_t)0x2903));
BLEDescriptor pScaleDescriptor(BLEUUID((uint16_t)0x2904));
BLEDescriptor pControlDescriptor(BLEUUID((uint16_t)0x2905));
*/

// CONTROL FUNCTIONS ***************************************************************

void animationAdjust(double newAnimation) {
   fxIndex = newAnimation;
   pAnimationCharacteristic->setValue(String(newAnimation).c_str());
   pAnimationCharacteristic->notify();
   if (debug) {
      Serial.print("Animation: ");
      Serial.println(newAnimation);
   }
}

void colorOrderAdjust(double newColorOrder) {
   colorOrder = newColorOrder;
   colorOrderChanged = true;
   pColorCharacteristic->setValue(String(colorOrder).c_str());
   pColorCharacteristic->notify();
   if (debug) {
      Serial.print("Color order: ");
      Serial.println(colorOrder);
   }
}

void speedAdjust(float newSpeed) {
   timeSpeed = newSpeed;
   pSpeedCharacteristic->setValue(String(timeSpeed).c_str());
   pSpeedCharacteristic->notify();
   if (debug) {
      Serial.print("Speed: ");
      Serial.println(timeSpeed);
   }
}

void scaleAdjust(String jsonVal) {
   deserializeJson(sendDoc, jsonVal);
   float newScale = sendDoc["scale"];
   adjustScale = newScale;
   if (debug) {
      Serial.print("Scale: ");
      Serial.println(newScale);
   }
   confirmDoc["scale"] = adjustScale;
   String jsonVal;
   ArduinoJson::serializeJson(confirmDoc, jsonVal);
   pScaleCharacteristic->setValue(String(jsonVal).c_str());
   pScaleCharacteristic->notify();
}

/*
void brightnessAdjust(uint8_t newBrightness) {
   BRIGHTNESS = newBrightness;
   //brightnessChanged = true;
   FastLED.setBrightness(BRIGHTNESS);
   pBrightnessCharacteristic->setValue(String(BRIGHTNESS).c_str());
   pBrightnessCharacteristic->notify();
   if (debug) {
      Serial.print("Brightness: ");
      Serial.println(BRIGHTNESS);
   }
}
*/

/*
void startWaves() {
   if (rotateWaves) { gCurrentPaletteNumber = random(0,gGradientPaletteCount); }
   CRGBPalette16 gCurrentPalette( gGradientPalettes[gCurrentPaletteNumber] );
   pPaletteCharacteristic->setValue(String(gCurrentPaletteNumber).c_str());
   pPaletteCharacteristic->notify();
   if (debug) {
      Serial.print("Color palette: ");
      Serial.println(gCurrentPaletteNumber);
   }
   gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
   gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
}
*/


// CALLBACKS ****************************************************************************

class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
    wasConnected = true;
    if (debug) {Serial.println("Device Connected");}
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
    wasConnected = true;
  }
};

class AnimationCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       uint8_t receivedValue = value[0];
       if (debug) {
         Serial.print("Animation: ");
         Serial.println(receivedValue);
       }
       
       if (receivedValue != 99) {
       
         if (receivedValue == 1) { //polar waves
            fxIndex = 0;
         }
         if (receivedValue == 2) { // spiralus
            fxIndex = 1;
         }
         if (receivedValue == 3) { // caleido1
            fxIndex = 2;
         }
         if (receivedValue == 4) { // waves
            fxIndex = 3;
         }
         if (receivedValue == 5) { // chasing spirals
            fxIndex = 4;
         }
         if (receivedValue == 6) { // rings
            fxIndex = 5;
         }
         if (receivedValue == 7) { // complex kaleido 6 
            fxIndex = 6;
         }
         if (receivedValue == 8) { // experiment 10
            fxIndex = 7;
         }
         if (receivedValue == 9) { // experiment sm1
            fxIndex = 8;
         }
         
         displayOn = true;
         animationAdjust(fxIndex);
       
       }

       if (receivedValue == 99) { //off
          displayOn = false;
       }
	
      }
   }
};

class ColorCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       int receivedValue = value[0]; 
       if (debug) {
         Serial.print("Color order: ");
         Serial.println(receivedValue);
       }
       colorOrder = receivedValue;
       colorOrderAdjust(colorOrder);
	}
 }
};

class SpeedCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       double receivedValue = value[0]; 
       if (debug) {
         Serial.print("Speed adjust: ");
         Serial.println(receivedValue);
       }
       timeSpeed = receivedValue;
       speedAdjust(timeSpeed);
    }
 }
};

class ScaleCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       float receivedValue = value[0]; 
       if (debug) {
         Serial.print("Scale: ");
         Serial.println(receivedValue);
       }
      sendDoc["scale"] = receivedValue;
      String jsonVal;
      ArduinoJson::serializeJson(sendDoc, jsonVal);
      scaleAdjust(jsonVal);
    }
 }
};

class ControlCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
      uint8_t receivedValue = value[0];
      if (debug) {
         Serial.print("Control: ");
         Serial.println(receivedValue);
       } 
      if (receivedValue == 100) {
         rotateAnimations = true;
      }
      if (receivedValue == 101) {
         rotateAnimations = false;
	  }
    }
 }
};


/*
class BrightnessCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       uint8_t receivedValue = value[0]; 
       if (debug) {
         Serial.print("Brightness adjust: ");
         Serial.println(receivedValue);
       }
       if (receivedValue == 1) {
          uint8_t newBrightness = min(BRIGHTNESS + brightnessInc,255);
          brightnessAdjust(newBrightness);
       }
       if (receivedValue == 2) {
          uint8_t newBrightness = max(BRIGHTNESS - brightnessInc,0);
          brightnessAdjust(newBrightness);
       }
    }
 }
};
*/

/*
class SpeedCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       uint8_t receivedValue = value[0]; 
       if (debug) {
         Serial.print("Speed adjust: ");
         Serial.println(receivedValue);
       }
       if (receivedValue == 1) {
          uint8_t newSpeed = min(SPEED+1,10);
          speedAdjust(newSpeed);
       }
       if (receivedValue == 2) {
          uint8_t newSpeed = max(SPEED-1,0);
          speedAdjust(newSpeed);
       }
    }
 }
};
*/

//*******************************************************************************************

void bleSetup() {

   BLEDevice::init("AnimARTrix Playground");

   pServer = BLEDevice::createServer();
   pServer->setCallbacks(new MyServerCallbacks());

   BLEService *pService = pServer->createService(SERVICE_UUID);

   pAnimationCharacteristic = pService->createCharacteristic(
                        ANIMATION_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                     );
   pAnimationCharacteristic->setCallbacks(new AnimationCharacteristicCallbacks());
   pAnimationCharacteristic->setValue(String(fxIndex).c_str()); 
   pAnimationCharacteristic->addDescriptor(new BLE2902());

   pColorCharacteristic = pService->createCharacteristic(
                        COLOR_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                     );
   pColorCharacteristic->setValue(String(colorOrder).c_str()); 
   pColorCharacteristic->setCallbacks(new ColorCharacteristicCallbacks());
   pColorCharacteristic->addDescriptor(new BLE2902());

   pSpeedCharacteristic = pService->createCharacteristic(
                        SPEED_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                     );
   pSpeedCharacteristic->setCallbacks(new SpeedCharacteristicCallbacks());
   pSpeedCharacteristic->setValue(String(timeSpeed).c_str());
   pSpeedCharacteristic->addDescriptor(new BLE2902());
   //pSpeedDescriptor.setValue("Speed"); 


   pScaleCharacteristic = pService->createCharacteristic(
                        SCALE_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_NOTIFY
                     );
   pScaleCharacteristic->setCallbacks(new ScaleCharacteristicCallbacks());
   pScaleCharacteristic->setValue(String(adjustScale).c_str());
   //pScaleCharacteristic->addDescriptor(new BLE2902());
   //pScaleDescriptor.setValue("Scale"); 
   
   pControlCharacteristic = pService->createCharacteristic(
                        CONTROL_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                     );
   pControlCharacteristic->setCallbacks(new ControlCharacteristicCallbacks());
   pControlCharacteristic->addDescriptor(new BLE2902());

   //**********************************************************

   pService->start();

   BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
   pAdvertising->addServiceUUID(SERVICE_UUID);
   pAdvertising->setScanResponse(false);
   pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
   BLEDevice::startAdvertising();
   if (debug) {Serial.println("Waiting a client connection to notify...");}

}