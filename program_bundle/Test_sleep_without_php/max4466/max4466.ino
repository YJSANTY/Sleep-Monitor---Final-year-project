const int analogPin = 33; // Analog pin for MAX4466 output
const int threshold = 1600; // Adjust this threshold based on your requirements

void setup() {
  Serial.begin(115200);
  delay(1000);
}

void loop() {
  // Read analog value from the MAX4466
  int noiseLevel = analogRead(analogPin);

  // Print the noise level
  Serial.print("Noise Level: ");
  Serial.println(noiseLevel);

  // Check if the noise level exceeds the threshold
  if (noiseLevel < threshold) {
    //Serial.println("Noise detected!");
    // You can add additional actions or trigger events here when noise is detected
  }

  delay(100); // Adjust the delay based on your monitoring frequency
}
