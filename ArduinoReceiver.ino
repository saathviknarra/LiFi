/*
  Analog Input

  Demonstrates analog input by reading an analog sensor on analog pin 0 and
  turning on and off a light emitting diode(LED) connected to digital pin 13.
  The amount of time the LED will be on and off depends on the value obtained
  by analogRead().

  The circuit:
  - potentiometer
    center pin of the potentiometer to the analog input 0
    one side pin (either one) to ground
    the other side pin to +5V
  - LED
    anode (long leg) attached to digital output 13 through 220 ohm resistor
    cathode (short leg) attached to ground

  - Note: because most Arduinos have a built-in LED attached to pin 13 on the
    board, the LED is optional.

  created by David Cuartielles
  modified 30 Aug 2011
  By Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/AnalogInput
*/

int sensorPin = A0;    // select the input pin for the potentiometer
int ledPin = 13;      // select the pin for the LED
int sensorValue = 0;  // variable to store the value coming from the sensor
int doneCalibrating = 10;
const int windowSize = 100;
int button = 9;

//for Manchester 4:5 encoding
const int preCode = 21;
const int fakePreCode = 26;
int decoding[32] = {\
  0b0000, 0b0000, 0b0000, 0b0000, 0b0000, 0b0000, 0b0000, 0b0000,\
  0b0000, 0b0001, 0b0100, 0b0101, 0b0000, 0b0000, 0b0110, 0b0111,\
  0b0000, 0b0000, 0b1000, 0b1001, 0b0010, 0b0011, 0b1010, 0b1011,\
  0b0000, 0b0000, 0b1100, 0b1101, 0b1110, 0b1111, 0b0000, 0b0000 \
};

// circuler buffer
bool ignoreOnes = false;
char dataReceived[30];
bool preCodeFlag = false;
int printOutput = 0;
int symbol = 0;
bool newBit = false;
bool idle = true;
const int settlingPoint = 120;
int averagePeriod = 0;
double period;//used to be double
int valueArray[2][windowSize];
int tPeriod[windowSize];
#define DATA 0
#define DIFF 1
int maxAndMinDiffIndex[windowSize];
int lastAverage;
int index, indexcounter, counter, periodIndex, symbolCounter = 0;
int len = 0;
double sum, sum2, sum3 = 0;
int prev = 0;
//int diffPrev = [0,0];//first element is Diffvalue second is Index
int dataMin = 1024;
int dataMax = 0;
int diffMax = 0;
int diffMin = 50;
int middleIndex = 50;
int calData;
int i, j, k;
int tmp0, tmp1, tmp2;
int upperCutDiff;
int lowerCutDiff;
int indexMaxMin = 0;
int newMax = -1022;
int newMin = 1023;

double sampleCounter=0;

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  pinMode(doneCalibrating, OUTPUT);
  pinMode(button, INPUT);
  Serial.begin(9600);
}



void calibrationFunc(){
    sensorValue = analogRead(sensorPin);

    // update the valueArray and its params
    prev = valueArray[DATA][index];
    valueArray[DATA][index] = sensorValue;
    index = (index + 1) % windowSize;
    len = (len + 1) > windowSize ? windowSize : len + 1;
    sum = sum - prev + sensorValue;
    if ( prev == dataMin ) {
      dataMin = 1024;
      for (int i = 0; i < windowSize; i++) { // TODO: this loop is crazy; needs optimization
        dataMin = valueArray[DATA][i] < dataMin ? valueArray[DATA][i] : dataMin;
      }
    } else if (prev == dataMax) {
      dataMax = 0;
      for (int i = 0; i < windowSize; i++) { // TODO: this loop is crazy; needs optimization
        dataMax = valueArray[DATA][i] > dataMax ? valueArray[DATA][i] : dataMax;
      }
    }
    if (sensorValue > dataMax) {
      dataMax = sensorValue;
    } else if (sensorValue < dataMin) {
      dataMin = sensorValue;
    }

    // calibrate the middle point
    if (len == windowSize) {
      calData = (valueArray[DATA][middleIndex] - sum / windowSize) / (dataMax - dataMin) * 100 + 50;
      middleIndex = (middleIndex + 1) % windowSize;
    }
}

void loop() {
  while(digitalRead(button));
  Serial.println("Button works");
  // read the value from the sensor:
  while (1) {//this is calibration loop
    calibrationFunc();
    if (len > 2) {
      tmp0 = (index - 2 + windowSize) % windowSize;
      tmp1 = (index - 1 + windowSize) % windowSize;
      prev = valueArray[DIFF][tmp1];
      valueArray[DIFF][tmp1] = valueArray[DATA][index] - valueArray[DATA][tmp0];
      if ( prev == diffMin) {
        diffMin = 1024;
        for (int i = 0; i < windowSize; i++) { // TODO: this loop is crazy; needs optimization
          diffMin = valueArray[DIFF][i] < diffMin ? valueArray[DIFF][i] : diffMin;
        }
      } else if (prev == diffMax) {
        diffMax = 0;
        for (int i = 0; i < windowSize; i++) { // TODO: this loop is crazy; needs optimization
          diffMax = valueArray[DIFF][i] > diffMax ? valueArray[DIFF][i] : diffMax;
        }
      }
      if (valueArray[DIFF][tmp1] > diffMax) {
        diffMax = valueArray[DIFF][tmp1];
      } else if (valueArray[DIFF][tmp1] < diffMin) {
        diffMin = valueArray[DIFF][tmp1];
      }
      upperCutDiff = diffMax - ((diffMax - diffMin) / 10);
      lowerCutDiff = diffMin + ((diffMax - diffMin) / 10);
    }
    if (valueArray[DIFF][tmp1] >= upperCutDiff) {//finding values above uppercut for DIFF
      if (newMax == -1023) {
        maxAndMinDiffIndex[indexcounter] = indexMaxMin;
//        Serial.print((maxAndMinDiffIndex[indexcounter] - maxAndMinDiffIndex[(indexcounter+windowSize-1)%windowSize]+windowSize)%windowSize);//TODO: needs to be fixed
        prev = tPeriod[indexcounter];
        tPeriod[indexcounter] = (maxAndMinDiffIndex[indexcounter] - maxAndMinDiffIndex[(indexcounter+windowSize-1)%windowSize]+windowSize)%windowSize;//finding individual periods
        sum3 += tPeriod[indexcounter]-prev;
//        Serial.print(sum3/windowSize);
        periodIndex++;

        indexcounter = (indexcounter + 1) % windowSize;
//        Serial.print (indexMaxMin);
//        Serial.print("\n");
//        Serial.print("newMin = ");
//        Serial.print (newMin);
//        Serial.print("\n");
      }
      newMin = 1024;
      indexMaxMin = valueArray[DIFF][tmp1] > newMax ? tmp1 : indexMaxMin;
      newMax = valueArray[DIFF][tmp1] > newMax ? valueArray[DIFF][tmp1] : newMax;
      //      Serial.print(valueArray[DIFF][tmp1]);
      //      Serial.print("\n");
    } else if (valueArray[DIFF][tmp1] <= lowerCutDiff) {//finding values above uppercut for DIFF
      if (newMin == 1024) {
        maxAndMinDiffIndex[indexcounter] = indexMaxMin;
//        Serial.print((maxAndMinDiffIndex[indexcounter] - maxAndMinDiffIndex[(indexcounter+windowSize-1)%windowSize]+windowSize)%windowSize);//TODO: needs to be fixed
        prev = tPeriod[indexcounter];
        tPeriod[indexcounter] = (maxAndMinDiffIndex[indexcounter] - maxAndMinDiffIndex[(indexcounter+windowSize-1)%windowSize]+windowSize)%windowSize;//finding individual periods
        sum3 += tPeriod[indexcounter]-prev;
//        Serial.print(sum3/windowSize);
        periodIndex++;

        indexcounter = (indexcounter + 1) % windowSize;
//        Serial.print (indexMaxMin);
//        Serial.print("\n");
//        Serial.print("newMax = ");
//        Serial.print(newMax);
//        Serial.print("\n");
      }
      newMax = -1023;
      indexMaxMin = valueArray[DIFF][tmp1] < newMin ? tmp1 : indexMaxMin;
//      Serial.print("Diff = ");
//      Serial.print(valueArray[DIFF][tmp1]);
//      Serial.print("\n");
      newMin = valueArray[DIFF][tmp1] < newMin ? valueArray[DIFF][tmp1] : newMin;
      //      Serial.print(valueArray[DIFF][tmp1]);
      //      Serial.print("\n");
    }
    if(periodIndex == settlingPoint){

          break;
        }
//    Serial.print(valueArray[DIFF][tmp1]);
//    Serial.print("\n");
//    if(indexMaxMin >= windowSize){
    //      for(int a = 49; a < windowSize; a++){
    //        sum2 += maxAndMinDiffIndex[a] - maxAndMinDiffIndex[a-1];
    //        counter++;
    //      }
    //      period = sum2/counter;
    //}

    // print it
    //Serial.print(sum3/windowSize);
    //Serial.print("\n");
    delayMicroseconds(2);
  }
  Serial.print("\n");
  Serial.print("Calibration Done!");
  Serial.print("\n");
  period = sum3/windowSize*1.0;
  sampleCounter = 0;
  Serial.print(period);
  Serial.print("\n");
  digitalWrite(doneCalibrating, HIGH);
  delay(100);
  digitalWrite(doneCalibrating, LOW);
  delay(2000);
  int indexReceived = 0;
  int newIdx = 0;
  while(1){
      sensorValue = analogRead(sensorPin);
      calData = (sensorValue - sum / windowSize) / (dataMax - dataMin) * 100 + 50;//can be replaced with up and lower bounds representiting 0 and 1
      //Serial.println(calData);
      if(ignoreOnes && calData > 80){
        ignoreOnes = false;
      }else if(idle && calData < 50){
//        Serial.println("A");
        //Serial.println(testCounter);
        //symbol = (symbol << 1) + 1;
        symbolCounter = 0;
        idle = !idle;
//        Serial.println("Precode");
        sampleCounter = (period)/2;
      }else if(!idle){
        sampleCounter--;
        //Serial.println(calData);
//        Serial.println(sampleCounter);
        if(sampleCounter < 0){
        //Serial.println(calData);
        //Serial.println(testCounter);
        if(calData < 50){
          symbol = (symbol << 1) + 1;
//          Serial.println(symbol);
//          Serial.println(1);
        }else{
          symbol = (symbol << 1) + 0;
//          Serial.println(symbol);
//          Serial.println(0);
        }
        symbolCounter++;
        if(symbolCounter % 5 == 0){
            if(!preCodeFlag && symbol == preCode){
                  preCodeFlag = true;
                  symbol = 0;
                  symbolCounter = 0;
             } else if (!preCodeFlag && symbol == fakePreCode) {

                  symbol = symbol & 0x0f;
                  //Serial.println(symbol);
                  symbolCounter = 4;
             } else if(!preCodeFlag){
//              Serial.println(symbol);
              symbol = 0;
              idle = !idle;
              symbolCounter = 0;
             }
             else if(preCodeFlag){
//                Serial.println(int(decoding[symbol]));
                if(indexReceived%6 == 5){//add ending with 0 before switching to nonidle
                  //ignoreOnes = true;
                  idle = !idle;
                  preCodeFlag = false;
                }
//                if(indexReceived == 23){
//                  break;
//                }
                indexReceived++;//modulate by 6
                  if(symbolCounter == 10) {
                    printOutput = ( decoding[symbol] << 4) + printOutput;
//                    Serial.print("2)");
                    Serial.println(char(printOutput));
//                    dataReceived[newIdx] = char(printOutput);
                    newIdx++;
                    if(printOutput == 0){
                      break;
                    }
                    printOutput = 0;
                    symbolCounter = 0;
                  } else {
                    printOutput = decoding[symbol];
//                    Serial.print("1)");
//                    Serial.println(printOutput);
                  }
                    symbol = 0;
                  //Serial.print("Symbol = ");
                  //Serial.println(symbol);

                //  while(1);
             }
          }
        sampleCounter += period;
        }
      }

      //1010
      delayMicroseconds(2);
  }
//  Serial.println(newIdx);
//  for(i = 0; i < 24; i++){
//   Serial.println(dataReceived[i]);
//  }
}
