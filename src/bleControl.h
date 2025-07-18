#pragma once

#include "FastLED.h"
#include <ArduinoJson.h>

/* If you use more than ~4 characteristics, you need to increase numHandles in this file:
C:\Users\...\.platformio\packages\framework-arduinoespressif32\libraries\BLE\src\BLEServer.h
Setting numHandles = 60 has worked for 7 characteristics.  
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>

#include <FS.h>
#include "LittleFS.h"
#define FORMAT_LITTLEFS_IF_FAILED true 

bool displayOn = true;
bool debug = true;
uint16_t debugDelay = 500;

uint8_t initialFxIndex = 0;

uint8_t dummy = 1;

// UI Elements *************************************************************************************

#ifdef SCREEN_TEST

   //I have not used the Screen Test/ WASM functionality in some time. Certain variables/UI elements may be broken/missing/disconnected

   using namespace fl;

   UITitle title("AnimARTrix Playground");
   UIDescription description("Test of user-controlled inputs for selected Animartrix effects. @author of fx is StefanPetrick");
   UISlider brightness("Brightness", BRIGHTNESS, 0, 255);
   UINumberField cFxIndex("Animartrix - index", initialFxIndex, 0, 9); // NUM_ANIMATIONS - 1 // currently creates a float; rest of program assumes uint8_t; not sure it will work
   UINumberField colorOrder("Color Order", 0, 0, 5); // currently creates a float; rest of program assumes uint8_t; not sure it will work
   UISlider timeSpeed("Speed", 1, .1, 10, -1); //multiplied

   UISlider cRatBase("Ratios: Base", 0, -1, 1, -1);  // added/subtracted
   UISlider cRatDiff("Ratios: Diff", 1, .1, 10, -1);  // multiplied

   UISlider cOffBase("Offsets: Base", 1, .1, 10, -1);  // multiplied
   UISlider cOffDiff("Offsets: Diff" , 1, .1, 10, -1);  //multiplied

   UISlider cScale("Scale", 1, .3, 3, -1); //multiplied

   UISlider cRadiusA("Radius", 0, -10, 30, 1);  // added/subtracted
   UISlider cRadiusB("Radius", 0, -10, 10, 1);  // added/subtracted

   UISlider cAngle("Angle", 1, .1, 10, -1); //multiplied
   //UISlider cZoom("Zoom", 1, .1, 10, -1); //multiplied
   UISlider cZ("Magic Z", 1, .1, 2, -1); //multiplied

   UISlider cRed("Red", 1, .1, 10, -1);  //multiplied 
   UISlider cGreen("Green", 1, .1, 10, -1);   //multiplied
   UISlider cBlue("Blue", 1, .1, 10, -1);  //multiplied

#else

   //#include <map>

   using namespace ArduinoJson;

   bool rotateAnimations = false;
   
   String cPresetName;

   uint8_t cBright = 75;
   uint8_t cColOrd = 0;                  
   uint8_t cFxIndex = initialFxIndex;

   //float timeSpeed = 1.f; 
   float cSpeed = 1.f;

   float cRatBase = 0.0f; 
   float cRatDiff= 1.f; 

   float cOffBase = 1.f; 
   float cOffDiff = 1.f; 

   float cScale = 1.f; 
   float cTwist = 1.f;

   float cRadius = 1.0f; 
   float cEdge = 1.0f;

   float cZoom = 1.f;

   float cAngle = 1.f; 

   float cZ = 1.f; 

   float cRed = 1.f; 
   float cGreen = 1.f; 
   float cBlue = 1.f; 

   bool Layer1 = true;
   bool Layer2 = true;
   bool Layer3 = true;
   bool Layer4 = true;
   bool Layer5 = true;

   ArduinoJson::JsonDocument sendDoc;
   ArduinoJson::JsonDocument receivedJSON;

#endif

//*******************************************************************************
//BLE CONFIGURATION *************************************************************

BLEServer* pServer = NULL;
BLECharacteristic* pButtonCharacteristic = NULL;
BLECharacteristic* pCheckboxCharacteristic = NULL;
BLECharacteristic* pNumberCharacteristic = NULL;
//BLECharacteristic* pPresetCharacteristic = NULL;
bool deviceConnected = false;
bool wasConnected = false;

#define SERVICE_UUID                  	"19b10000-e8f2-537e-4f6c-d104768a1214"
#define BUTTON_CHARACTERISTIC_UUID     "19b10001-e8f2-537e-4f6c-d104768a1214"
#define CHECKBOX_CHARACTERISTIC_UUID   "19b10002-e8f2-537e-4f6c-d104768a1214"
#define NUMBER_CHARACTERISTIC_UUID     "19b10003-e8f2-537e-4f6c-d104768a1214"
//#define PRESET_CHARACTERISTIC_UUID     "19b10004-e8f2-537e-4f6c-d104768a1214"

BLEDescriptor pButtonDescriptor(BLEUUID((uint16_t)0x2902));
BLEDescriptor pCheckboxDescriptor(BLEUUID((uint16_t)0x2902));
BLEDescriptor pNumberDescriptor(BLEUUID((uint16_t)0x2902));
//BLEDescriptor pPresetDescriptor(BLEUUID((uint16_t)0x2902));

//*******************************************************************************
// CONTROL FUNCTIONS ************************************************************

void animationc(uint8_t newAnimation) {
   pButtonCharacteristic->setValue(String(newAnimation).c_str());
   pButtonCharacteristic->notify();
   if (debug) {
      Serial.print("Animation: ");
      Serial.println(newAnimation);
   }
}

// UI update functions ***********************************************

void sendReceiptNumber(String receivedID, float receivedValue) {
   // Prepare the JSON document to send
   sendDoc.clear();
   sendDoc["id"] = receivedID;
   sendDoc["val"] = receivedValue;

   // Convert the JSON document to a string
   String jsonString;
   serializeJson(sendDoc, jsonString);

   // Set the value of the characteristic
   pNumberCharacteristic->setValue(jsonString);
   
   // Notify connected clients
   pNumberCharacteristic->notify();
   
   if (debug) {
      Serial.print("Sent receipt for ");
      Serial.print(receivedID);
      Serial.print(": ");
      Serial.println(receivedValue);
   }
}

void sendReceiptCheckbox(String receivedID, bool receivedValue) {
  
   // Prepare the JSON document to send
   sendDoc.clear();
   sendDoc["id"] = receivedID;
   sendDoc["val"] = receivedValue;

   // Convert the JSON document to a string
   String jsonString;
   serializeJson(sendDoc, jsonString);

   // Set the value of the characteristic
   pCheckboxCharacteristic->setValue(jsonString);
   
   // Notify connected clients
   pCheckboxCharacteristic->notify();
   
   if (debug) {
      Serial.print("Sent receipt for ");
      Serial.print(receivedID);
      Serial.print(": ");
      Serial.println(receivedValue);
   }
}

// Handle UI request functions ***********************************************

void processNumber(String receivedID, float receivedValue ) {

   if (receivedID == "inBright") {cBright = receivedValue;};
   if (receivedID == "inSpeed") {cSpeed = receivedValue;};
   if (receivedID == "inColOrd") {cColOrd = receivedValue;};
   if (receivedID == "inRatBase") {cRatBase = receivedValue;};
   if (receivedID == "inRatDiff") {cRatDiff = receivedValue;};
   if (receivedID == "inOffBase") {cOffBase = receivedValue;};
   if (receivedID == "inOffDiff") {cOffDiff = receivedValue;};
   if (receivedID == "inScale") {cScale = receivedValue;};	
   if (receivedID == "inAngle") {cAngle = receivedValue;};	
   if (receivedID == "inZoom") {cZoom = receivedValue;};
   if (receivedID == "inRadius") {cRadius = receivedValue;};
   if (receivedID == "inEdge") {cEdge = receivedValue;};	
   if (receivedID == "inZ") {cZ = receivedValue;};	
   if (receivedID == "inRed") {cRed = receivedValue;};	
   if (receivedID == "inGreen") {cGreen = receivedValue;};	
   if (receivedID == "inBlue") {cBlue = receivedValue;};
   if (receivedID == "inTwist") {cTwist = receivedValue;};
   
   sendReceiptNumber(receivedID, receivedValue);

}

void processCheckbox(String receivedID, bool receivedValue ) {

   if (receivedID == "cxRotateAnim") {rotateAnimations = receivedValue;};
   if (receivedID == "cxLayer1") {Layer1 = receivedValue;};
   if (receivedID == "cxLayer2") {Layer2 = receivedValue;};
   if (receivedID == "cxLayer3") {Layer3 = receivedValue;};
   if (receivedID == "cxLayer4") {Layer4 = receivedValue;};
   if (receivedID == "cxLayer5") {Layer5 = receivedValue;};
    
   sendReceiptCheckbox(receivedID, receivedValue);
}

//*******************************************************************************
// PRESETS **********************************************************************

//String pPresetName;

struct Preset {
   String pPresetName;
   uint8_t pFxIndex;
   uint8_t pBright;
   uint8_t pColOrd;
   float pSpeed;
   float pRatBase;
   float pRatDiff;
   float pOffBase;
   float pOffDiff;
   float pScale;	
   float pAngle;
   float pZoom;
   float pTwist;
   float pRadius;
   float pEdge;	
   float pZ;	
   float pRed;
   float pGreen;	
   float pBlue;   
};

Preset preset1 = {.pPresetName ="preset1"};
Preset preset2 = {.pPresetName ="preset2"};
Preset preset3 = {.pPresetName ="preset3"};

void savePreset(const char* name, const Preset &preset) {
   String path = "/";
   path += name;
   path += ".txt"; 
   File file = LittleFS.open(path, "w");

   if (!file) {
      Serial.print("Failed to open file for writing: ");
      Serial.println(path);
   }

   FastLED.delay(debugDelay);
   file.printf("%d\n", preset.pFxIndex);
   FastLED.delay(debugDelay);
   file.printf("%d\n", preset.pBright);
   FastLED.delay(debugDelay);
   file.printf("%d\n", preset.pColOrd);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pSpeed);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pRatBase);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pRatDiff);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pOffBase);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pOffDiff);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pScale);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pAngle);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pZoom);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pTwist);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pRadius);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pEdge);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pZ);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pRed);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pGreen);
   FastLED.delay(debugDelay);
   file.printf("%f\n", preset.pBlue);
   FastLED.delay(debugDelay);
   
   if (debug) {
      FastLED.delay(debugDelay);
      Serial.println(preset.pFxIndex);
      FastLED.delay(debugDelay);
      Serial.println(preset.pBright);
      FastLED.delay(debugDelay);
      Serial.println(preset.pColOrd);
      FastLED.delay(debugDelay);
      Serial.println(preset.pSpeed);
      FastLED.delay(debugDelay);
      Serial.println(preset.pZoom);
      FastLED.delay(debugDelay);
      Serial.println(preset.pScale);
      FastLED.delay(debugDelay);
      Serial.println(preset.pAngle);
      FastLED.delay(debugDelay);
      Serial.println(preset.pTwist);
      FastLED.delay(debugDelay);
      Serial.println(preset.pRadius);
      FastLED.delay(debugDelay);
      Serial.println(preset.pEdge);
      FastLED.delay(debugDelay);
      Serial.println(preset.pZ);
      FastLED.delay(debugDelay);
      Serial.println(preset.pRatBase);
      FastLED.delay(debugDelay);
      Serial.println(preset.pRatDiff);
      FastLED.delay(debugDelay);
      Serial.println(preset.pOffBase);
      FastLED.delay(debugDelay);
      Serial.println(preset.pOffDiff);
      FastLED.delay(debugDelay);
      Serial.println(preset.pRed);
      FastLED.delay(debugDelay);
      Serial.println(preset.pGreen);
      FastLED.delay(debugDelay);
      Serial.println(preset.pBlue);
   }


   file.close();
   Serial.print("Preset saved to ");
   Serial.println(path);

}

void capturePreset(Preset &preset) {
   FastLED.delay(debugDelay);
   preset.pFxIndex = cFxIndex;
   FastLED.delay(debugDelay);
   preset.pBright = cBright;
   FastLED.delay(debugDelay);
   preset.pColOrd = cColOrd;
   FastLED.delay(debugDelay);
   preset.pSpeed = cSpeed;
   FastLED.delay(debugDelay);
   preset.pRatBase = cRatBase; 
   FastLED.delay(debugDelay);
   preset.pRatDiff = cRatDiff; 
   FastLED.delay(debugDelay);
   preset.pOffBase = cOffBase; 
   FastLED.delay(debugDelay);
   preset.pOffDiff = cOffDiff; 
   FastLED.delay(debugDelay);
   preset.pScale = cScale; 	
   FastLED.delay(debugDelay);
   preset.pAngle = cAngle; 
   FastLED.delay(debugDelay);
   preset.pZoom = cZoom; 
   FastLED.delay(debugDelay);
   preset.pTwist = cTwist; 
   FastLED.delay(debugDelay);
   preset.pRadius = cRadius; 
   FastLED.delay(debugDelay);
   preset.pEdge = cEdge; 	
   FastLED.delay(debugDelay);
   preset.pZ = cZ; 	
   FastLED.delay(debugDelay);
   preset.pRed = cRed; 
   FastLED.delay(debugDelay);
   preset.pGreen = cGreen; 	
   FastLED.delay(debugDelay);
   preset.pBlue = cBlue; 
   FastLED.delay(debugDelay);

   if (debug) {
      FastLED.delay(debugDelay);
      Serial.println(preset.pFxIndex);
      FastLED.delay(debugDelay);
      Serial.println(preset.pBright);
      FastLED.delay(debugDelay);
      Serial.println(preset.pColOrd);
      FastLED.delay(debugDelay);
      Serial.println(preset.pSpeed);
      FastLED.delay(debugDelay);
      Serial.println(preset.pZoom);
      FastLED.delay(debugDelay);
      Serial.println(preset.pScale);
      FastLED.delay(debugDelay);
      Serial.println(preset.pAngle);
      FastLED.delay(debugDelay);
      Serial.println(preset.pTwist);
      FastLED.delay(debugDelay);
      Serial.println(preset.pRadius);
      FastLED.delay(debugDelay);
      Serial.println(preset.pEdge);
      FastLED.delay(debugDelay);
      Serial.println(preset.pZ);
      FastLED.delay(debugDelay);
      Serial.println(preset.pRatBase);
      FastLED.delay(debugDelay);
      Serial.println(preset.pRatDiff);
      FastLED.delay(debugDelay);
      Serial.println(preset.pOffBase);
      FastLED.delay(debugDelay);
      Serial.println(preset.pOffDiff);
      FastLED.delay(debugDelay);
      Serial.println(preset.pRed);
      FastLED.delay(debugDelay);
      Serial.println(preset.pGreen);
      FastLED.delay(debugDelay);
      Serial.println(preset.pBlue);
   }

   //savePreset(preset.pPresetName.c_str(), preset);

}

void applyPreset(const Preset &preset) {
   cFxIndex = preset.pFxIndex;
   cBright = preset.pBright;
   cColOrd = preset.pColOrd;
   cSpeed = preset.pSpeed;
   cRatBase = preset.pRatBase;
   cRatDiff = preset.pRatDiff;
   cOffBase = preset.pOffBase;
   cOffDiff = preset.pOffDiff;
   cScale = preset.pScale;
   cAngle = preset.pAngle;
   cZoom = preset.pZoom;
   cTwist = preset.pTwist;
   cRadius = preset.pRadius;
   cEdge = preset.pEdge;
   cZ = preset.pZ;
   cRed = preset.pRed;
   cGreen = preset.pGreen;
   cBlue = preset.pBlue;
}

void retrievePreset(const char* name, Preset &preset) {
   String path = "/";
   path += name;
   path += ".txt"; 
   File file = LittleFS.open(path, "r");

   if (!file) {
      Serial.print("Failed to open file for reading: ");
      Serial.println(path);
   }
  
   preset.pFxIndex = file.readStringUntil('\n').toInt();
   preset.pBright = file.readStringUntil('\n').toInt();
   preset.pSpeed = file.readStringUntil('\n').toFloat();
   preset.pColOrd = file.readStringUntil('\n').toInt();
   preset.pRatBase = file.readStringUntil('\n').toFloat();
   preset.pRatDiff = file.readStringUntil('\n').toFloat();
   preset.pOffBase = file.readStringUntil('\n').toFloat();
   preset.pOffDiff = file.readStringUntil('\n').toFloat();
   preset.pScale = file.readStringUntil('\n').toFloat();
   preset.pAngle = file.readStringUntil('\n').toFloat();
   preset.pZoom = file.readStringUntil('\n').toFloat();
   preset.pTwist = file.readStringUntil('\n').toFloat();
   preset.pRadius = file.readStringUntil('\n').toFloat();
   preset.pEdge = file.readStringUntil('\n').toFloat();
   preset.pZ = file.readStringUntil('\n').toFloat();
   preset.pRed = file.readStringUntil('\n').toFloat();
   preset.pGreen = file.readStringUntil('\n').toFloat();
   preset.pBlue = file.readStringUntil('\n').toFloat();

   file.close();
   Serial.print("Preset loaded from: ");
   Serial.println(path);

   applyPreset(preset);

}

//*******************************************************************************
// CALLBACKS ********************************************************************

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

class ButtonCharacteristicCallbacks : public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *characteristic) {

      String value = characteristic->getValue();
      if (value.length() > 0) {
         
         uint8_t receivedValue = value[0];
         
         if (debug) {
            Serial.print("Animation: ");
            Serial.println(receivedValue);
         }
       
         if (receivedValue != 99) {
         
            if (receivedValue < 20) {

               if (receivedValue == 1) { //polar waves
                  cFxIndex = 0;
               }
               if (receivedValue == 2) { // spiralus
                  cFxIndex = 1;
               }
               if (receivedValue == 3) { // caleido1
                  cFxIndex = 2;
               }
               if (receivedValue == 4) { // waves
                  cFxIndex = 3;
               }
               if (receivedValue == 5) { // chasing spirals
                  cFxIndex = 4;
               }
               if (receivedValue == 6) { // complex kaleido 6 
                  cFxIndex = 5;
               }
               if (receivedValue == 7) { // water
                  cFxIndex = 6;
               }
               if (receivedValue == 8) { // experiment 10
                  cFxIndex = 7;
               }
               if (receivedValue == 9) { // experiment sm1
                  cFxIndex = 8;
               }
               if (receivedValue == 10) { // test
                  cFxIndex = 9;
               }

               displayOn = true;
               animationc(cFxIndex);
            }
         
         if (receivedValue >= 20 && receivedValue < 30) {

            if (receivedValue == 20) { capturePreset(preset1); }

            if (receivedValue == 21) { retrievePreset("preset1", preset1); }

            if (receivedValue == 22) { capturePreset(preset2); }

            if (receivedValue == 23) { retrievePreset("preset2", preset2); }

         }

       }

       if (receivedValue == 99) { //off
          displayOn = false;
       }
	
      }
   }
};

class CheckboxCharacteristicCallbacks : public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *characteristic) {
  
      String receivedBuffer = characteristic->getValue();
  
      if (receivedBuffer.length() > 0) {
                  
         if (debug) {
            Serial.print("Received buffer: ");
            Serial.println(receivedBuffer);
         }
      
         ArduinoJson::deserializeJson(receivedJSON, receivedBuffer);
         String receivedID = receivedJSON["id"] ;
         bool receivedValue = receivedJSON["val"];
      
         if (debug) {
            Serial.print(receivedID);
            Serial.print(": ");
            Serial.println(receivedValue);
         }
      
         processCheckbox(receivedID, receivedValue);
      
      }
   }
};

class NumberCharacteristicCallbacks : public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *characteristic) {
      
      String receivedBuffer = characteristic->getValue();
      
      if (receivedBuffer.length() > 0) {
      
         if (debug) {
            Serial.print("Received buffer: ");
            Serial.println(receivedBuffer);
         }
      
         ArduinoJson::deserializeJson(receivedJSON, receivedBuffer);
         String receivedID = receivedJSON["id"] ;
         float receivedValue = receivedJSON["val"];
      
         if (debug) {
            Serial.print(receivedID);
            Serial.print(": ");
            Serial.println(receivedValue);
         }
      
         processNumber(receivedID, receivedValue);
      }
   }
};

/*
class PresetCharacteristicCallbacks : public BLECharacteristicCallbacks {
   void onWrite(BLECharacteristic *characteristic) {
      
      String receivedValue = characteristic->getValue();
      
      if (receivedValue.length() > 0) {
      
         if (debug) {
            Serial.print("Received value: ");
            Serial.println(receivedValue);
         }
      
         newPreset = receivedValue;

         //loadPreset(newPreset);
      }
   }
};
*/

//*******************************************************************************
// BLE SETUP FUNCTION ***********************************************************

void bleSetup() {

   BLEDevice::init("AnimARTrix Playground");

   pServer = BLEDevice::createServer();
   pServer->setCallbacks(new MyServerCallbacks());

   BLEService *pService = pServer->createService(SERVICE_UUID);

   pButtonCharacteristic = pService->createCharacteristic(
                     BUTTON_CHARACTERISTIC_UUID,
                     BLECharacteristic::PROPERTY_WRITE |
                     BLECharacteristic::PROPERTY_READ |
                     BLECharacteristic::PROPERTY_NOTIFY
                  );
   pButtonCharacteristic->setCallbacks(new ButtonCharacteristicCallbacks());
   pButtonCharacteristic->setValue(String(cFxIndex).c_str());
   pButtonCharacteristic->addDescriptor(new BLE2902());

   pCheckboxCharacteristic = pService->createCharacteristic(
                     CHECKBOX_CHARACTERISTIC_UUID,
                     BLECharacteristic::PROPERTY_WRITE |
                     BLECharacteristic::PROPERTY_READ |
                     BLECharacteristic::PROPERTY_NOTIFY
                  );
   pCheckboxCharacteristic->setCallbacks(new CheckboxCharacteristicCallbacks());
   pCheckboxCharacteristic->setValue(String(dummy).c_str());
   pCheckboxCharacteristic->addDescriptor(new BLE2902());
   
   pNumberCharacteristic = pService->createCharacteristic(
                     NUMBER_CHARACTERISTIC_UUID,
                     BLECharacteristic::PROPERTY_WRITE |
                     BLECharacteristic::PROPERTY_READ |
                     BLECharacteristic::PROPERTY_NOTIFY
                  );
   pNumberCharacteristic->setCallbacks(new NumberCharacteristicCallbacks());
   pNumberCharacteristic->setValue(String(dummy).c_str());
   pNumberCharacteristic->addDescriptor(new BLE2902());

      
   /*
   pPresetCharacteristic = pService->createCharacteristic(
                     PRESET_CHARACTERISTIC_UUID,
                     BLECharacteristic::PROPERTY_WRITE |
                     BLECharacteristic::PROPERTY_READ |
                     BLECharacteristic::PROPERTY_NOTIFY
                  );
   pPresetCharacteristic->setCallbacks(new PresetCharacteristicCallbacks());
   pPresetCharacteristic->setValue(String(preset).c_str());
   pPresetCharacteristic->addDescriptor(new BLE2902());
   */
      
   //**********************************************************

   pService->start();

   BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
   pAdvertising->addServiceUUID(SERVICE_UUID);
   pAdvertising->setScanResponse(false);
   pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
   BLEDevice::startAdvertising();
   if (debug) {Serial.println("Waiting a client connection to notify...");}

}
