int sensorPin = A0;    // select the input pin for the potentiometer
int button = 9;

void setup() {
  Serial.begin(115200);
  pinMode(button, INPUT);
}

void loop() {
  while(digitalRead(button));
  unsigned long endTime=millis()+2000;
  unsigned long curTime=millis();
  while(curTime<endTime){
    Serial.println(analogRead(sensorPin));
    curTime=millis();
  }
  //cal_done
  delay(2000); // ms
  while(1){
    Serial.println(analogRead(sensorPin));
  }
}
