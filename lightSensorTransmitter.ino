int green = 12;
int red = 11;
int ackPin = 10;
int ackData;
int startSigPin = 9;
int precode = 21;
const char msg[] = "Hello World! Let's send some packets!";
int msgIdx = 0;
int precodeIdx = 105;
int fullMesIdx = 5;
int fullmessageTotalLen = 0;
bool fullmessage[500] = {0};
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
#define DELAY 10

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
  // while((msgIdx % 20 != 0 || msgIdx == 0) && (msgIdx < sizeof(msg))) {
  fullmessageTotalLen = (sizeof(msg)*2+19)/20*105;
  while(msgIdx < sizeof(msg)) {
    if(fullMesIdx == precodeIdx){
      fullmessage[fullMesIdx]   = 0;
      fullmessage[fullMesIdx+1] = 0;
      fullmessage[fullMesIdx+2] = 0;
      fullmessage[fullMesIdx+3] = 1;
      fullmessage[fullMesIdx+4] = 0;
      fullmessage[fullMesIdx+5] = 1;
      fullmessage[fullMesIdx+6] = 0;
      fullmessage[fullMesIdx+7] = 1;
      precodeIdx = precodeIdx + 108;
      fullMesIdx = fullMesIdx + 8;
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

void loop() {
  // wait for the switch to turn on once
  while(digitalRead(startSigPin));
  // 0-1-alternation until signified by the sender
  unsigned long endTime=millis()+2500;
  unsigned long curTime=millis();
  while(curTime<endTime) {
    digitalWrite(red, HIGH);
    delay(DELAY);
    digitalWrite(red, LOW);
    delay(DELAY);
    curTime=millis();
  }
  digitalWrite(red, LOW);
  delay(5000);
  //Serial.println("Cali done");
  fullMesIdx = 0;

//  digitalWrite(red, HIGH);
//  delay(DELAY);
  while (fullMesIdx < fullmessageTotalLen){
    if (fullmessage[fullMesIdx]){
      digitalWrite(red, HIGH);
//      Serial.print("1");
      delay(DELAY);
    } else {
      digitalWrite(red, LOW);
//      Serial.print("0");
      delay(DELAY);
    }
    fullMesIdx++;
  }
  digitalWrite(red, LOW);
  delay(DELAY);
  while(1);

}
