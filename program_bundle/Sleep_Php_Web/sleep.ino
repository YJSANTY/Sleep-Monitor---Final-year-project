
//======================================== Including the libraries.
#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include "DHT.h"
#include <Wire.h>
//======================================== 

const int analogPinADXL335_X = 32;  // Analog pin for ADXL335 X-axis
const int analogPinADXL335_Y = 34;  // Analog pin for ADXL335 Y-axis
const int analogPinADXL335_Z = 35;  // Analog pin for ADXL335 Z-axis
const int analogPinMax4466 = 33;    // Analog pin for MAX4466 output

int xZero, yZero, zZero;

// Defines the Digital Pin of the "On Board LED".
#define ON_Board_LED 2 

//======================================== SSID and Password of your WiFi router.
const char* ssid = "Santhosh";
const char* password = "09111972";
//======================================== 


//======================================== Variables for HTTP POST request data.
String postData = ""; //--> Variables sent for HTTP POST request data.
String payload = "";  //--> Variable for receiving response from HTTP POST.
//======================================== 


void setup() {
  Serial.begin(115200);
  delay(1000);

  // Init ADXL335 accelerometer
  xZero = analogRead(analogPinADXL335_X);
  yZero = analogRead(analogPinADXL335_Y);
  zZero = analogRead(analogPinADXL335_Z);

  Serial.begin(115200); //--> Initialize serial communications with the PC.
  pinMode(ON_Board_LED,OUTPUT); //--> On Board LED port Direction output.
  digitalWrite(ON_Board_LED, HIGH); //--> Turn on Led On Board.
  delay(2000);

  digitalWrite(ON_Board_LED, LOW); //--> Turn off Led On Board.


  //---------------------------------------- Make WiFi on ESP32 in "STA/Station" mode and start connecting to WiFi Router/Hotspot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  //---------------------------------------- 
  
  Serial.println();
  Serial.println("-------------");
  Serial.print("Connecting");

  //---------------------------------------- The process of connecting the WiFi on the ESP32 to the WiFi Router/Hotspot.
  // The process timeout of connecting ESP32 with WiFi Hotspot / WiFi Router is 20 seconds.
  // If within 20 seconds the ESP32 has not been successfully connected to WiFi, the ESP32 will restart.
  // I made this condition because on my ESP32, there are times when it seems like it can't connect to WiFi, so it needs to be restarted to be able to connect to WiFi.

  int connecting_process_timed_out = 20; //--> 20 = 20 seconds.
  connecting_process_timed_out = connecting_process_timed_out * 2;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    //........................................ Make the On Board Flashing LED on the process of connecting to the wifi router.
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    //........................................ 

    //........................................ Countdown "connecting_process_timed_out".
    if(connecting_process_timed_out > 0) connecting_process_timed_out--;
    if(connecting_process_timed_out == 0) {
      delay(1000);
      ESP.restart();
    }
    //........................................ 
  }
  //---------------------------------------- 
  
  digitalWrite(ON_Board_LED, LOW); //--> Turn off the On Board LED when it is connected to the wifi router.
  
  //---------------------------------------- If successfully connected to the wifi router, the IP Address that will be visited is displayed in the serial monitor
  Serial.println();
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  //Serial.print("IP address: ");
  //Serial.println(WiFi.localIP());
  Serial.println("-------------");
  //---------------------------------------- 


  delay(2000);
}

void loop() {
  // ADXL335 accelerometer
  int xValueADXL335 = analogRead(analogPinADXL335_X) - xZero;
  int yValueADXL335 = analogRead(analogPinADXL335_Y) - yZero;
  int zValueADXL335 = analogRead(analogPinADXL335_Z) - zZero;

  // Calculate the magnitude of acceleration
  float magnitude = sqrt(pow(xValueADXL335, 2) + pow(yValueADXL335, 2) + pow(zValueADXL335, 2));

  // MAX4466 microphone
  //int noiseLevelMax4466 = analogRead(analogPinMax4466);

  // MAX4466 microphone
  int rawNoiseLevelMax4466 = analogRead(analogPinMax4466);

  // Save only the first three digits from the left
  int noiseLevelMax4466 = map(rawNoiseLevelMax4466, 0, 1023, 0, 999);
  
  // Print the topic only once
  static boolean topicPrinted = false;
  if (!topicPrinted) {
    Serial.println("SPO2,HeartRate,NoiseLevel,Body Motion");
    topicPrinted = true;
  }

    // Generate a random value between 90 and 95 for SPO2
  int spo2Value = random(90, 96);

  int heartbeat = random(65, 69);

  // Print all values in a single line for CSV format
  Serial.print(spo2Value);
  Serial.print(",");
  Serial.print(heartbeat);
  Serial.print(",");
  Serial.print(noiseLevelMax4466);
  Serial.print(",");
  Serial.println(magnitude);

  //---------------------------------------- Check WiFi connection status.
  if(WiFi.status()== WL_CONNECTED) {
    HTTPClient http;  //--> Declare object of class HTTPClient.
    int httpCode;     //--> Variables for HTTP return code.
    
    //........................................ Process to get LEDs data from database to control LEDs.
    postData = "id=esp32_01";
    payload = "";
  
    digitalWrite(ON_Board_LED, HIGH);
    Serial.println();
    Serial.println("---------------getdata.php");
    // In this project I use local server or localhost with XAMPP application.
    // So make sure all PHP files are "placed" or "saved" or "run" in the "htdocs" folder.
    // I suggest that you create a new folder for this project in the "htdocs" folder.
    // The "htdocs" folder is in the "xampp" installation folder.
    // The order of the folders I recommend:
    // xampp\htdocs\your_project_folder_name\phpfile.php
    //
    // ESP32 accesses the data bases at this line of code: 
    // http.begin("http://REPLACE_WITH_YOUR_COMPUTER_IP_ADDRESS/REPLACE_WITH_PROJECT_FOLDER_NAME_IN_htdocs_FOLDER/getdata.php");
    // REPLACE_WITH_YOUR_COMPUTER_IP_ADDRESS = there are many ways to see the IP address, you can google it. 
    //                                         But make sure that the IP address used is "IPv4 address".
    // Example : http.begin("http://192.168.0.0/Sleep/getdata.php");
    http.begin("http://192.168.0.43/Sleep/getdata.php");  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");        //--> Specify content-type header
   
    httpCode = http.POST(postData); //--> Send the request
    payload = http.getString();     //--> Get the response payload
  
    Serial.print("httpCode : ");
    Serial.println(httpCode); //--> Print HTTP return code
    Serial.print("payload  : ");
    Serial.println(payload);  //--> Print request response payload
    
    http.end();  //--> Close connection
    Serial.println("---------------");
    digitalWrite(ON_Board_LED, LOW);
    //........................................ 

    //........................................ The process of sending the DHT11 sensor data and the state of the LEDs to the database.

    postData = "id=esp32_01";
    postData += "&SpO2=" + String(spo2Value);
    postData += "&Heart_Beat=" + String(heartbeat);
    postData += "&Noice=" + String(noiseLevelMax4466);
    postData += "&Body_Motion =" + String(magnitude);
    payload = "";
  
    digitalWrite(ON_Board_LED, HIGH);
    Serial.println();
    Serial.println("---------------updateDHT11data_and_recordtable.php");
    // Example : http.begin("http://192.168.0.0/Sleep/updateDHT11data_and_recordtable.php");
    http.begin("http://192.168.0.43/Sleep/updateDHT11data_and_recordtable.php");  //--> Specify request destination
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  //--> Specify content-type header
   
    httpCode = http.POST(postData); //--> Send the request
    payload = http.getString();  //--> Get the response payload
  
    Serial.print("httpCode : ");
    Serial.println(httpCode); //--> Print HTTP return code
    Serial.print("payload  : ");
    Serial.println(payload);  //--> Print request response payload
    
    http.end();  //Close connection
    Serial.println("---------------");
    digitalWrite(ON_Board_LED, LOW);
    //........................................ 
    
    delay(4000);
  }

}
