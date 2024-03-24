const int xPin = 34; // Analog pin for X-axis
const int yPin = 35; // Analog pin for Y-axis
const int zPin = 32; // Analog pin for Z-axis

int xZero, yZero, zZero;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Capture initial readings as the "zero" values
  xZero = analogRead(xPin);
  yZero = analogRead(yPin);
  zZero = analogRead(zPin);
}

void loop() {
  // Read analog values from the accelerometer
  int xValue = analogRead(xPin) - xZero;
  int yValue = analogRead(yPin) - yZero;
  int zValue = analogRead(zPin) - zZero;

  // Print the adjusted values
  Serial.print("X: ");
  Serial.print(xValue);
  Serial.print("\tY: ");
  Serial.print(yValue);
  Serial.print("\tZ: ");
  Serial.println(zValue);

  delay(500); // Delay for readability
}
