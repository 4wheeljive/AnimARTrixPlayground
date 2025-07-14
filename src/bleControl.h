#pragma once

#include "FastLED.h" 
#include <ArduinoJson.h>

//#include "LittleFS.h"
//#define FORMAT_LITTLEFS_IF_FAILED true 

/* If you use more than ~4 characteristics, you need to increase numHandles in this file:
C:\Users\...\.platformio\packages\framework-arduinoespressif32\libraries\BLE\src\BLEServer.h
Setting numHandles = 60 has worked for 7 characteristics.  
*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>

using namespace fl;

bool displayOn = true;
bool debug = true;
bool colorOrderChanged = false;

uint8_t initialFxIndex = 0;

uint8_t switchNumber = 1;

String elementID;
String receivedString;
String newPreset;

uint8_t dummy = 1;

// UI Elements *************************************************************************************

#ifdef SCREEN_TEST

   //I have not used the Screen Test/ WASM functionality in some time. Certain variables/UI elements may be broken/missing/disconnected

   using namespace fl;

   UITitle title("AnimARTrix Playground");
   UIDescription description("Test of user-controlled inputs for selected Animartrix effects. @author of fx is StefanPetrick");
   UISlider brightness("Brightness", BRIGHTNESS, 0, 255);
   UINumberField fxIndex("Animartrix - index", initialFxIndex, 0, 9); // NUM_ANIMATIONS - 1 // currently creates a float; rest of program assumes uint8_t; not sure it will work
   UINumberField colorOrder("Color Order", 0, 0, 5); // currently creates a float; rest of program assumes uint8_t; not sure it will work
   UISlider timeSpeed("Speed", 1, .1, 10, -1); //multiplied

   UISlider adjustRatiosBase("Ratios: Base", 0, -1, 1, -1);  // added/subtracted
   UISlider adjustRatiosDiff("Ratios: Diff", 1, .1, 10, -1);  // multiplied

   UISlider adjustOffsetsBase("Offsets: Base", 1, .1, 10, -1);  // multiplied
   UISlider adjustOffsetsDiff("Offsets: Diff" , 1, .1, 10, -1);  //multiplied

   UISlider adjustScale("Scale", 1, .3, 3, -1); //multiplied

   UISlider adjustRadiusA("Radius", 0, -10, 30, 1);  // added/subtracted
   UISlider adjustRadiusB("Radius", 0, -10, 10, 1);  // added/subtracted

   UISlider adjustAngle("Angle", 1, .1, 10, -1); //multiplied
   //UISlider adjustDistance("Distance", 1, .1, 10, -1); //multiplied
   UISlider adjustZ("Magic Z", 1, .1, 2, -1); //multiplied

   UISlider adjustRed("Red", 1, .1, 10, -1);  //multiplied 
   UISlider adjustGreen("Green", 1, .1, 10, -1);   //multiplied
   UISlider adjustBlue("Blue", 1, .1, 10, -1);  //multiplied

#else

   using namespace ArduinoJson;

   String preset = "default";

   uint8_t adjustBrightness = 25;
  
   bool rotateAnimations = false;
   uint8_t adjustColorOrder = 0;                  
   uint8_t fxIndex = initialFxIndex;

   float timeSpeed = 1.f; 
   float adjustSpeed = 1.f;

   float adjustRatiosBase = 0.0f; 
   float adjustRatiosDiff= 1.f; 

   float adjustOffsetsBase = 1.f; 
   float adjustOffsetsDiff = 1.f; 

   float adjustScale = 1.f; 

   float adjustRadiusA = 0.0f; 
   float adjustRadiusB = 0.0f;

   float adjustDistance = 1.f;

   float adjustAngle = 1.f; 

   float adjustZ = 1.f; 

   float adjustRed = 1.f; 
   float adjustGreen = 1.f; 
   float adjustBlue = 1.f; 

   ArduinoJson::JsonDocument sendDoc;
   ArduinoJson::JsonDocument receivedJSON;
   //ArduinoJson::JsonDocument jsonPreset;


  /*
  struct Preset {
      uint8_t pBrightness;
      float pSpeed;
      uint8_t pColorOrder;
      float pRatiosBase;
      float pRatiosDiff;
      float pOffsetsBase;
      float pOffsetsDiff;
      float pScale;	
      float pAngle;	
      float pDistance;
      float pRadiusA;
      float pRadiusB;	
      float pZ;	
      float pRed;
      float pGreen;	
      float pBlue;   
   };

   Preset preset1;
   */

#endif


//BLE configuration *************************************************************

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

// CONTROL FUNCTIONS ***************************************************************

void animationAdjust(uint8_t newAnimation) {
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

void processNumber(String receivedID, float receivedValue ) {

   if (receivedID == "inputBrightness") {adjustBrightness = receivedValue;};
   if (receivedID == "inputSpeed") {adjustSpeed = receivedValue;};
   if (receivedID == "inputColorOrder") {adjustColorOrder = receivedValue;};
   if (receivedID == "inputRatiosBase") {adjustRatiosBase = receivedValue;};
   if (receivedID == "inputRatiosDiff") {adjustRatiosDiff = receivedValue;};
   if (receivedID == "inputOffsetsBase") {adjustOffsetsBase = receivedValue;};
   if (receivedID == "inputOffsetsDiff") {adjustOffsetsDiff = receivedValue;};
   if (receivedID == "inputScale") {adjustScale = receivedValue;};	
   if (receivedID == "inputAngle") {adjustAngle = receivedValue;};	
   if (receivedID == "inputDistance") {adjustDistance = receivedValue;};
   if (receivedID == "inputRadiusA") {adjustRadiusA = receivedValue;};
   if (receivedID == "inputRadiusB") {adjustRadiusB = receivedValue;};	
   if (receivedID == "inputZ") {adjustZ = receivedValue;};	
   if (receivedID == "inputRed") {adjustRed = receivedValue;};	
   if (receivedID == "inputGreen") {adjustGreen = receivedValue;};	
   if (receivedID == "inputBlue") {adjustBlue = receivedValue;};
 
   sendReceiptNumber(receivedID, receivedValue);

}

/*
void loadPreset(String presetName) {

   // Open the applicable file store
   File configFile = LittleFS.open("/config.json", "r"); // replace "config.json" with "presetName.json")
   if (!configFile) {
     Serial.println("Failed to open config file for reading");
     return;
   }

   //Extract preset variable data into JSON document
   DynamicJsonDocument jsonPreset(1024);
   DeserializationError error = deserializeJson(jsonPreset, configFile);
   if (error) {
     Serial.print(F("deserializeJson() failed: "));
     Serial.println(error.c_str());
     configFile.close();
     return;
   }
   
   // Retrieve parameter settings from the JSON document
   

   // For each parameter with a stored value:
   if pBrightness


         // create a variable that contains the parameterID (e.g., "inputBrightness")  
         String parameterID = jsonPreset[receivedID];
         // create a variable that contains the parameter value 
         float parameterValue = jsonPreset[receivedValue];
         // send a processNumber request with the applicable arguments
         processNumber(parameterID, parameterValue);
   


   configFile.close();
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
            if (receivedValue == 8) { // water
               fxIndex = 7;
            }
            if (receivedValue == 9) { // experiment 10
               fxIndex = 8;
            }
            if (receivedValue == 10) { // experiment sm1
               fxIndex = 9;
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

// PRESET STRUCTURE *******************************************************************
/*
void littleFSsetup() {

  if(!LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
   Serial.println("LittleFS Mount Failed");
   return;
  }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs.open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\r\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("- failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("- message appended");
    } else {
        Serial.println("- append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("- file renamed");
    } else {
        Serial.println("- rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\r\n", path);
    if(fs.remove(path)){
        Serial.println("- file deleted");
    } else {
        Serial.println("- delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    Serial.printf("Testing file I/O with %s\r\n", path);

    static uint8_t buf[512];
    size_t len = 0;
    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }

    size_t i;
    Serial.print("- writing" );
    uint32_t start = millis();
    for(i=0; i<2048; i++){
        if ((i & 0x001F) == 0x001F){
          Serial.print(".");
        }
        file.write(buf, 512);
    }
    Serial.println("");
    uint32_t end = millis() - start;
    Serial.printf(" - %u bytes written in %u ms\r\n", 2048 * 512, end);
    file.close();

    file = fs.open(path);
    start = millis();
    end = start;
    i = 0;
    if(file && !file.isDirectory()){
        len = file.size();
        size_t flen = len;
        start = millis();
        Serial.print("- reading" );
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            if ((i++ & 0x001F) == 0x001F){
              Serial.print(".");
            }
            len -= toRead;
        }
        Serial.println("");
        end = millis() - start;
        Serial.printf("- %u bytes read in %u ms\r\n", flen, end);
        file.close();
    } else {
        Serial.println("- failed to open file for reading");
    }
}


/* EXAMPLE COMMANDS ***************************************
    createDir(LittleFS, "/mydir"); // Create a mydir folder
    writeFile(LittleFS, "/mydir/hello1.txt", "Hello1"); // Create a hello1.txt file with the content "Hello1"
    listDir(LittleFS, "/", 1); // List the directories up to one level beginning at the root directory
    deleteFile(LittleFS, "/mydir/hello1.txt"); //delete the previously created file
    removeDir(LittleFS, "/mydir"); //delete the previously created folder
    listDir(LittleFS, "/", 1); // list all directories to make sure they were deleted
    
    writeFile(LittleFS, "/hello.txt", "Hello "); //Create and write a new file in the root directory
    appendFile(LittleFS, "/hello.txt", "World!\r\n"); //Append some text to the previous file
    readFile(LittleFS, "/hello.txt"); // Read the complete file
    renameFile(LittleFS, "/hello.txt", "/foo.txt"); //Rename the previous file
    readFile(LittleFS, "/foo.txt"); //Read the file with the new name
    deleteFile(LittleFS, "/foo.txt"); //Delete the file
    testFileIO(LittleFS, "/test.txt"); //Testin
    deleteFile(LittleFS, "/test.txt"); //Delete the file
   */

/*
void createPreset(char* testText) {
 
   writeFile(LittleFS, "/config.json", testText);    

   File configFile = LittleFS.open("/config.json", "w");
      if (!configFile) {
      Serial.println("Failed to open config file for writing");
      return;
      }
   
   DynamicJsonDocument doc(1024); // Adjust size as needed
   // Add variables to the JSON document
   doc["ssid"] = "your_ssid";
   doc["password"] = "your_password";
   doc["some_setting"] = 123;
   
   serializeJson(doc, configFile);
   configFile.close();

}
*/




