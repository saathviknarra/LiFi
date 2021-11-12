int sensorPin = A0;    // select the input pin for the potentiometer

void setup() {
  Serial.begin(9600);
}

void loop() {
  val = analogRead(sensorPin);
  Serial.println(val);
}
