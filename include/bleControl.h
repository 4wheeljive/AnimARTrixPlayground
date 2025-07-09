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

using namespace fl;

bool displayOn = true;
bool debug = true;
bool rotateAnimations = true;
bool colorOrderChanged = false;

double initialFxIndex = 4;   // this should really be changed to uint8_t, but UINumberField requires that it be a double
bool nextFxIndexRandom = false;

uint8_t switchNumber = 1;

String elementID;
String receivedString;

uint8_t dummy = 1;

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

   uint8_t BRIGHTNESS = 25;
  
   double colorOrder = 0;                   // this should really be changed to uint8_t, but UINumberField requires that it be a double
   double fxIndex = initialFxIndex;        // this should really be changed to uint8_t, but UINumberField requires that it be a double

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
   ArduinoJson::JsonDocument receivedJSON;

#endif


//BLE configuration *************************************************************

BLEServer* pServer = NULL;
BLECharacteristic* pButtonCharacteristic = NULL;
BLECharacteristic* pCheckboxCharacteristic = NULL;
BLECharacteristic* pNumberCharacteristic = NULL;
bool deviceConnected = false;
bool wasConnected = false;

#define SERVICE_UUID                  	"19b10000-e8f2-537e-4f6c-d104768a1214"
#define BUTTON_CHARACTERISTIC_UUID     "19b10001-e8f2-537e-4f6c-d104768a1214"
#define CHECKBOX_CHARACTERISTIC_UUID   "19b10002-e8f2-537e-4f6c-d104768a1214"
#define NUMBER_CHARACTERISTIC_UUID     "19b10003-e8f2-537e-4f6c-d104768a1214"

BLEDescriptor pButtonDescriptor(BLEUUID((uint16_t)0x2902));
BLEDescriptor pCheckboxDescriptor(BLEUUID((uint16_t)0x2902));
BLEDescriptor pNumberDescriptor(BLEUUID((uint16_t)0x2902));

// CONTROL FUNCTIONS ***************************************************************

void animationAdjust(double newAnimation) {
   pButtonCharacteristic->setValue(String(newAnimation).c_str());
   pButtonCharacteristic->notify();
   if (debug) {
      Serial.print("Animation: ");
      Serial.println(newAnimation);
   }
}

void sendReceiptNumber(String receivedID, float receivedValue) {
   // Prepare the JSON document to send
   sendDoc.clear();
   sendDoc["id"] = receivedID;
   sendDoc["value"] = receivedValue;

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

void inputSwitcher(String receivedID) {
      if (receivedID == "inputSpeed") {switchNumber = 1;};
      if (receivedID == "inputBrightness") {switchNumber = 2;};
      if (receivedID == "inputColorOrder") {switchNumber = 3;};
      if (receivedID == "inputRatiosBase") {switchNumber = 4;};
      if (receivedID == "inputRatiosDiff") {switchNumber = 5;};
      if (receivedID == "inputOffsetsBase") {switchNumber = 6;};
      if (receivedID == "inputOffsetsDiff") {switchNumber = 7;};
      if (receivedID == "inputScale") {switchNumber = 8;};	
      if (receivedID == "inputAngle") {switchNumber = 9;};	
      if (receivedID == "inputRadiusA") {switchNumber = 10;};	
      if (receivedID == "inputZ") {switchNumber = 11;};	
      if (receivedID == "inputRed") {switchNumber = 12;};	
      if (receivedID == "inputGreen") {switchNumber = 13;};	
      if (receivedID == "inputBlue") {switchNumber = 14;};	
   }

void processNumber(String receivedID, float receivedValue ) {

   inputSwitcher(receivedID);

   switch (switchNumber) {
      case 1:  timeSpeed = receivedValue; break;
      case 2:  BRIGHTNESS = (uint8_t) receivedValue; break;
      case 3:  colorOrder = (uint8_t) receivedValue; colorOrderChanged = true; break;
      case 4:  adjustRatiosBase = receivedValue; break;
      case 5:  adjustRatiosDiff = receivedValue; break;
      case 6:  adjustOffsetsBase = receivedValue; break;
      case 7:  adjustOffsetsDiff = receivedValue; break;
      case 8:  adjustScale = receivedValue; break;
      case 9:  adjustAngle = receivedValue; break;
      case 10:  adjustRadiusA = receivedValue; break;
      case 11:  adjustZ = receivedValue; break;
      case 12:  adjustRed = receivedValue; break;
      case 13:  adjustGreen = receivedValue; break;
      case 14:  adjustBlue = receivedValue; break;
      default:  Serial.println("Unknown input"); return;
   }

   sendReceiptNumber(receivedID, receivedValue);

}

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
  
      pCheckboxCharacteristic->setValue(String(receivedValue).c_str());
      pCheckboxCharacteristic->notify();

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
         float receivedValue = receivedJSON["value"];
      
         if (debug) {
            Serial.print(receivedID);
            Serial.print(": ");
            Serial.println(receivedValue);
         }
      
         processNumber(receivedID, receivedValue);
      }
   }
};

//*******************************************************************************************

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
   pButtonCharacteristic->setValue(String(fxIndex).c_str());
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
      
   //**********************************************************

   pService->start();

   BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
   pAdvertising->addServiceUUID(SERVICE_UUID);
   pAdvertising->setScanResponse(false);
   pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
   BLEDevice::startAdvertising();
   if (debug) {Serial.println("Waiting a client connection to notify...");}

}
