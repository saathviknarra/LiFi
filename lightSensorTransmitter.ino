int green = 12;
int red = 11;
int ackPin = 10;
int ackData;
int startSigPin = 9;
int precode = 21;
int msgIdx = 0;
#define MSG_LEN 15
char msg[15] = "Hello World!";
char encoding[16] = { \
  0b01111, 0b10010, 0b00101, 0b10101, \
  0b01010, 0b11010, 0b01110, 0b11110, \
  0b01001, 0b11001, 0b01101, 0b10101, \
  0b01011, 0b11011, 0b00111, 0b10111  \
  };
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

void sendData(int dataToSend){
   while(dataToSend){
       //Serial.println(dataToSend&1);
       if(dataToSend & 1){
          digitalWrite(red, HIGH);
          delay(50);
          digitalWrite(red, LOW);
       } else {
          digitalWrite(red, LOW);
          delay(50); 
       }   
       dataToSend=dataToSend>>1;  
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
  sendData(precode);
  while (msgIdx < 12 && msg[msgIdx]!='\0'){
    Serial.print(msg[msgIdx]);
    sendData(encoding[msg[msgIdx]]);
    msgIdx++;
  }
  while(1);

}
