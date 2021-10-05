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
const int windowSize = 100;

// circuler buffer
const int settlingPoint = 300;
int averagePeriod = 0;
int period;
int valueArray[2][windowSize];
int tPeriod[windowSize];
#define DATA 0
#define DIFF 1
int maxAndMinDiffIndex[windowSize];
int lastAverage;
int index, indexcounter, counter, periodIndex = 0;
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

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // read the value from the sensor:
  while (1) {//this is calibration loop
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
    delay(2);
  }
  Serial.print("\n");
  Serial.print("Calibration Done!");
  Serial.print("\n");
  Serial.print(sum3/windowSize);
  while(1);
}
