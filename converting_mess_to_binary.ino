int green = 12;
int red = 11;
#define DELAY 3
int msglen = 12;
char msg[msglen] = "Hello World!";
int msgIdx = 0;
int fullMesIdx = 5;
bool fullmessage[105] = {0};
fullmessage[0] = 1;
fullmessage[1] = 0;
fullmessage[2] = 1;
fullmessage[3] = 0;
fullmessage[4] = 1;
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
  Serial.begin(9600);
}
void loop() {
  while((msgIdx % 20 != 0 || msgIdx == 0) && (msgIdx < msglen)) {
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

  //tranverse the fullmessage array and send the 1s and 0s
  
  while(msgIdx >= msglen){
  }
}
