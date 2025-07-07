#pragma once

#include "FastLED.h" 
#include <ArduinoJson.h> 

/* Be sure to set numHandles = 60 in this file:
C:\Users\...\.platformio\packages\framework-arduinoespressif32\libraries\BLE\src\BLEServer.h
*/
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>

uint8_t BRIGHTNESS = 50;

bool displayOn = true;

bool debug = true;

bool rotateAnimations = true;

void colorChase();

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

   uint8_t hue = 0;
   uint8_t brightness = BRIGHTNESS;
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
//BLECharacteristic* pButtonCharacteristic = NULL;
//BLECharacteristic* pCheckboxCharacteristic = NULL;
BLECharacteristic* pNumberCharacteristic = NULL;
//BLECharacteristic* pSliderCharacteristic = NULL;
bool deviceConnected = false;
bool wasConnected = false;

#define SERVICE_UUID                  	"19b10000-e8f2-537e-4f6c-d104768a1214"
//#define BUTTON_CHARACTERISTIC_UUID     "19b10001-e8f2-537e-4f6c-d104768a1214"
//#define CHECKBOX_CHARACTERISTIC_UUID   "19b10002-e8f2-537e-4f6c-d104768a1214"
#define NUMBER_CHARACTERISTIC_UUID     "19b10003-e8f2-537e-4f6c-d104768a1214"
//#define SLIDER_CHARACTERISTIC_UUID     "19b10004-e8f2-537e-4f6c-d104768a1214"

String elementID;
uint8_t elementValue;
String numberValue;

// CONTROL FUNCTIONS ***************************************************************


void processNumber(String receivedID, uint8_t receivedValue ) {
   if (receivedID == "hueInput" ) {
      hue = receivedValue;
   }
}


/*void animationAdjust(double newAnimation) {
   fxIndex = newAnimation;
   if (debug) {
      Serial.print("Animation: ");
      Serial.println(newAnimation);
   }   
   pAnimationCharacteristic->setValue(String(newAnimation).c_str());
   pAnimationCharacteristic->notify();
}


void sliderAdjust(String jsonReceived) {

   deserializeJson(sendDoc, jsonReceived);
   uint8_t newHue = sendDoc["scale"];
   adjustScale = newScale;

   if (debug) {
      Serial.print("Scale: ");
      Serial.println(newScale);
   }

   pScaleCharacteristic->setValue(String(adjustScale).c_str());
   pScaleCharacteristic->notify();
}

//   confirmDoc["scale"] = adjustScale;
//   String confirmString;
//   ArduinoJson::serializeJson(confirmDoc, confirmString);
//   pScaleCharacteristic->setValue(confirmString);

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

/*
class ButtonCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
       uint8_t receivedValue = value[0];
       if (debug) {
         Serial.print("Button: ");
         Serial.println(receivedValue);
       }
       
       if (receivedValue != 99) {
       
          if (receivedValue == 1) { }
         
          displayOn = true;
              
       }

          if (receivedValue == 99) { //off
             displayOn = false;
         }
	
      }
   }
};

class CheckboxCharacteristicCallbacks : public BLECharacteristicCallbacks {
 void onWrite(BLECharacteristic *characteristic) {
    String value = characteristic->getValue();
    if (value.length() > 0) {
      uint8_t receivedValue = value[0];
      if (debug) {
         Serial.print("Checkbox: ");
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


class SliderCharacteristicCallbacks : public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *characteristic) {
      String receivedString = characteristic->getValue();
      if (receivedString.length() > 0) {
         if (debug) {
            Serial.print("Received String: ");
            Serial.println(receivedString);
         }
         ArduinoJson::deserializeJson(sendDoc, receivedString);
         String receivedID = sendDoc["id"] ;
         uint8_t receivedValue = sendDoc["value"];
         processSlider(receivedID, receivedValue);
      }
   }
};
*/


class NumberCharacteristicCallbacks : public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *characteristic) {
      String receivedString = characteristic->getValue();
      if (receivedString.length() > 0) {
         if (debug) {
            Serial.print("Received String: ");
            Serial.println(receivedString);
         }
         ArduinoJson::deserializeJson(sendDoc, receivedString);
         String receivedID = sendDoc["id"] ;
         uint8_t receivedValue = sendDoc["value"];
         processNumber(receivedID, receivedValue);
      }
   }
};




//*******************************************************************************************

void bleSetup() {

   BLEDevice::init("json Playground");

   pServer = BLEDevice::createServer();
   pServer->setCallbacks(new MyServerCallbacks());

   BLEService *pService = pServer->createService(SERVICE_UUID);


  /* pButtonCharacteristic = pService->createCharacteristic(
                        BUTTON_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                     );
      pButtonCharacteristic->setCallbacks(new ButtonCharacteristicCallbacks());
      pButtonCharacteristic->setValue(String(fxIndex).c_str()); 
   
   
   pCheckboxCharacteristic = pService->createCharacteristic(
                        CHECKBOX_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                     );
      pCheckboxCharacteristic->setCallbacks(new CheckboxCharacteristicCallbacks());
 
   
 
      pSliderCharacteristic = pService->createCharacteristic(
                        SLIDER_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                     );
      pSliderCharacteristic->setCallbacks(new SliderCharacteristicCallbacks());
      pSliderCharacteristic->setValue(String(adjustScale).c_str());
      */

      pNumberCharacteristic = pService->createCharacteristic(
                        NUMBER_CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_WRITE |
                        BLECharacteristic::PROPERTY_READ |
                        BLECharacteristic::PROPERTY_NOTIFY
                     );
      pNumberCharacteristic->setCallbacks(new NumberCharacteristicCallbacks());
      pNumberCharacteristic->setValue(String(numberValue).c_str());

   //**********************************************************

   pService->start();

   BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
   pAdvertising->addServiceUUID(SERVICE_UUID);
   pAdvertising->setScanResponse(false);
   pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
   BLEDevice::startAdvertising();
   if (debug) {Serial.println("Waiting a client connection to notify...");}

}