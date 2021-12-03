int green = 12;
int red = 11;
int ackPin = 10;
int ackData;
int startSigPin = 9;
int precode = 21;
const char msg[] = "Hello World!";
int msgIdx = 0;
int precodeIdx = 105;
int fullMesIdx = 5;
bool fullmessage[5+(5*sizeof(msg))+((5*sizeof(msg))/100)*5] = {0};
char encoding[16] = { \
  0b01111, 0b10010, 0b00101, 0b10101, \
  0b01010, 0b11010, 0b01110, 0b11110, \
  0b01001, 0b11001, 0b01101, 0b11101, \
  0b01011, 0b11011, 0b00111, 0b10111  \
  };
int charEncoded;
int charBuffer;
int firsthalf;
int secondhalf;

void setup() {
  // put your setup code here, to run once:
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(ackPin, INPUT);
  pinMode(startSigPin, INPUT);
  Serial.begin(9600);
  fullmessage[0] = 1;
  fullmessage[1] = 0;
  fullmessage[2] = 1;
  fullmessage[3] = 0;
  fullmessage[4] = 1;
  while((msgIdx % 20 != 0 || msgIdx == 0) && (msgIdx < sizeof(msg))) {
    if(fullMesIdx == precodeIdx){
      fullmessage[fullMesIdx] = 1;
      fullmessage[fullMesIdx+1] = 0;
      fullmessage[fullMesIdx+2] = 1;
      fullmessage[fullMesIdx+3] = 0;
      fullmessage[fullMesIdx+4] = 1;
      precodeIdx = precodeIdx + 105;
      fullMesIdx = fullMesIdx + 5;
    }
    charBuffer = msg[msgIdx];
    firsthalf = encoding[(charBuffer&0xf)];
    secondhalf = encoding[(charBuffer>>4)];
    int bitNum = 0;
    while(bitNum<5){
      if(firsthalf & 1){
         fullmessage[fullMesIdx] = 1;
      }else{
         fullmessage[fullMesIdx] = 0;
      }
      fullMesIdx++;
      bitNum++;
      firsthalf  = firsthalf >> 1;
    }
    bitNum = 0;
    while(bitNum<5){
      if(secondhalf & 1){
         fullmessage[fullMesIdx] = 1;
      }else{
         fullmessage[fullMesIdx] = 0;
      }
      fullMesIdx++;
      bitNum++;
      secondhalf  = secondhalf >> 1;
    }
    msgIdx++;
  }
}

//void sendOne(){
//  digitalWrite(red, HIGH);
//  delay(DELAY);
//}
//void sendZero(){
//  digitalWrite(red, LOW);
//  delay(DELAY);
//}

void sendData(int dataToSend, int numbit){
   int bitNum = 0;
   while(bitNum<numbit){
       //Serial.println(dataToSend&1);
       if(dataToSend & 1){
          digitalWrite(red, HIGH);
          delay(DELAY);
          digitalWrite(red, LOW);
       } else {
          digitalWrite(red, LOW);
          delay(DELAY);
       }
       dataToSend = dataToSend>>1;
       bitNum++;
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
    delay(DELAY);
    digitalWrite(red, LOW);
    delay(DELAY);
    //exit(0);
  }
  Serial.print("Calibration done!!!");
  Serial.print("\n");
  digitalWrite(red, LOW);
  delay(5000);

  //while(1);
  while (msgIdx < 12 && msg[msgIdx]!='\0'){
    if (msgIdx % 3 == 0) {
      sendData(precode, 5);
    }
    charBuffer = msg[msgIdx];
    charEncoded = encoding[(charBuffer&0xf)];
    //Serial.println(charBuffer>>4);
    Serial.println(charEncoded);

    sendData(charEncoded, 5);

    charEncoded = encoding[(charBuffer>>4)];
    //Serial.println(charBuffer&0xf);
    Serial.println(charEncoded);
    sendData(charEncoded, 5);
    //Serial.println(int(charBuffer));
    msgIdx++;
  }
  while(1);

}
