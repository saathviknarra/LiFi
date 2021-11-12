int sensorPin = A0;    // select the input pin for the potentiometer

void setup() {
  Serial.begin(115200);
}

void loop() {
  int val;
  val = analogRead(sensorPin);
  Serial.println(val);
  delay(2);
}
