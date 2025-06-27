#pragma once 

//#include <Arduino.h>

#include "FastLED.h" 
#include "fl/ui.h"  

#include <WebServer.h>
#include <WiFi.h>

#include "myAnimartrix_UI.h"

// WiFi Elements *************************************************************************************

// Replace with SSID and PASSWORD for the ESP32 ACCESS POINT
// (for testing you can leave the default)
const char* ssid = "ESP32_ACCESS_POINT";
const char* password = "pass123456";

// Assign output variables to GPIO pins
//const int output26 = 5;
//const int output27 = 6;
//String output26State = "off";
String displayState = "on";

// Create a web server object
WebServer server(80);

// Function to handle the root URL and show the current states
void handleRoot() {
 
  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
 
  html += "<link rel=\"icon\" href=\"data:,\">";
 
  html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
 
  html += ".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
 
  html += ".button2 { background-color: #555555; }</style></head>";
 
  html += "<body><h1>AnamARTrix Playground Web Server</h1>";

  // Display controls
 
  html += "<p>Display" + displayState + "</p>";
  
  
  if (displayState == "off") {
    html += "<p><a href=\"displayOn\"><button class=\"button\">ON</button></a></p>";
  } else {
    html += "<p><a href=\"displayOff\"><button class=\"button button2\">OFF</button></a></p>";
  }
    
  html += "</body></html>";
  
  server.send(200, "text/html", html);

}

// Function to handle turning GPIO 27 on
void handleDisplayOn() {
  displayOn = true; 
  displayState = "on";
  handleRoot();
}

// Function to handle turning GPIO 27 off
void handleDisplayOff() {
  displayOn = false; 
  displayState = "off";
  handleRoot();
}

//**************************************************************************************

void wifiSetup() {

  // Set the ESP32 as access point
  Serial.print("Setting as access point ");
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);

  Serial.println("");
  Serial.println("ESP32 Wi-Fi Access Point ready!");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Set up the web server to handle different routes
  server.on("/", handleRoot);
  server.on("/displayOn", handleDisplayOn);
  server.on("/displayOff", handleDisplayOff);

  // Start the web server
  server.begin();
  Serial.println("HTTP server started");
}

/*
void loop() {
  // Handle incoming client requests
  server.handleClient();
}
  */