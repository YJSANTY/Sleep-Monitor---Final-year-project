#include <Wire.h>
#include <DFRobot_MAX30102.h>

DFRobot_MAX30102 particleSensor;

const int analogPinADXL335_X = 32;  // Analog pin for ADXL335 X-axis
const int analogPinADXL335_Y = 34;  // Analog pin for ADXL335 Y-axis
const int analogPinADXL335_Z = 35;  // Analog pin for ADXL335 Z-axis

const int analogPinMax4466 = 33;    // Analog pin for MAX4466 output
const int thresholdMax4466 = 1600;  // Adjust this threshold based on your requirements

int xZero, yZero, zZero;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Init ADXL335 accelerometer
  xZero = analogRead(analogPinADXL335_X);
  yZero = analogRead(analogPinADXL335_Y);
  zZero = analogRead(analogPinADXL335_Z);

  // Init MAX30102 sensor
  Wire.begin(21, 22);  // Use GPIO21 for SDA and GPIO22 for SCL
  while (!particleSensor.begin()) {
    Serial.println("MAX30102 was not found");
    delay(1000);
  }

  // Set MAX30102 sensor configuration
  particleSensor.sensorConfiguration(/*ledBrightness=*/50, /*sampleAverage=*/SAMPLEAVG_4, \
                                     /*ledMode=*/MODE_MULTILED, /*sampleRate=*/SAMPLERATE_100, \
                                     /*pulseWidth=*/PULSEWIDTH_411, /*adcRange=*/ADCRANGE_16384);
}

boolean topicPrinted = false;

void loop() {
  // ADXL335 accelerometer
  int xValueADXL335 = analogRead(analogPinADXL335_X) - xZero;
  int yValueADXL335 = analogRead(analogPinADXL335_Y) - yZero;
  int zValueADXL335 = analogRead(analogPinADXL335_Z) - zZero;

  // Calculate the magnitude of acceleration
  float magnitude = sqrt(pow(xValueADXL335, 2) + pow(yValueADXL335, 2) + pow(zValueADXL335, 2));

  // MAX30102 pulse oximeter
  int32_t SPO2;          // SPO2
  int8_t SPO2Valid;       // Flag to display if SPO2 calculation is valid
  int32_t heartRate;      // Heart-rate
  int8_t heartRateValid;  // Flag to display if heart-rate calculation is valid

  particleSensor.heartrateAndOxygenSaturation(/**SPO2=*/&SPO2, /**SPO2Valid=*/&SPO2Valid, /**heartRate=*/&heartRate, /**heartRateValid=*/&heartRateValid);

  // MAX4466 microphone
  int noiseLevelMax4466 = analogRead(analogPinMax4466);

  // Print the topic only once
  if (!topicPrinted) {
    Serial.println("SPO2,HeartRate,NoiseLevel,Magnitude");
    topicPrinted = true;
  }

  // Print all values in a single line for CSV format
  Serial.print(map(SPO2, 0, 90, 0, 100));  // Assuming the raw SpO2 value is in the range 0 to 1024
  Serial.print(",");
  Serial.print(heartRate);
  Serial.print(map(heartRate, 0, 170, 64, 70));
  Serial.print(",");
  Serial.print(noiseLevelMax4466);
  Serial.print(",");
  Serial.println(magnitude);

  delay(500);  // Adjust the delay based on your monitoring frequency
}
