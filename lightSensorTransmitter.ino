int green = 12;
int red = 11;
int ackPin = 10;
int ackData;
int startSigPin = 9;
int precode = 13;
int sendData;

void setup() {
  // put your setup code here, to run once:
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(ackPin, INPUT);
  pinMode(startSigPin, INPUT);
  Serial.begin(9600); 
}

void sendOne(){
  digitalWrite(red, HIGH);
  delay(50);
}
void sendZero(){
  digitalWrite(red, LOW);
  delay(50);  
}
void sendPrecode(){
   sendData = precode;
   while(sendData){
       Serial.println(sendData&1);
       if(sendData & 1){
          digitalWrite(red, HIGH);
          delay(50);
          digitalWrite(red, LOW);
       } else {
          digitalWrite(red, LOW);
          delay(50); 
       }   
       sendData=sendData>>1;  
   }
}


void loop() {
  Serial.print("START");
  // wait for the switch to turn on once
  while(1){
    if(digitalRead(startSigPin)==0)
      break;
  }
  Serial.print("START Calibration...");
  // 0-1-alternation until signified by the sender
  while(1) {
    ackData=digitalRead(ackPin);
    if (ackData){
      break;
    }
    digitalWrite(red, HIGH);
    delay(50);
    digitalWrite(red, LOW);
    delay(50);
    //exit(0);
  }
  Serial.print("Calibration done!!!");
  Serial.print("\n");
  digitalWrite(red, LOW);
  delay(5000);
  sendPrecode();
  while(1);

}
