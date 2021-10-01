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
int valueArray[3][100];

// 
#define DATA 0
#define DIFF 1
#define SAMP 2


int lastAverage;
int index = 0;
int len = 0;
int sum = 0;
int prev = 0;
int dataMin = 0;
int dataMax = 0;
int middleIndex = 50;
int calData;
int i, j, k;
int tmp0, tmp1, tmp2;

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600); 
}

void loop() {

  while(1) {
    // read the value from the sensor:
    sensorValue = analogRead(sensorPin);
  
    // update the valueArray and its params
    prev = valueArray[DATA][index];
    valueArray[DATA][index]=sensorValue;
    index = (index+1)%windowSize;
    len = (len+1)>windowSize ? windowSize : len+1;
    sum = sum - prev + sensorValue;
    if( prev==dataMin ){
      dataMin=1024;
      for(int i=0; i<windowSize; i++){ // TODO: this loop is crazy; needs optimization
        dataMin = valueArray[DATA][i]<dataMin ? valueArray[DATA][i] : dataMin;  
      }  
    } else if (prev == dataMax){
      dataMax=0;
      for(int i=0; i<windowSize; i++){ // TODO: this loop is crazy; needs optimization
        dataMax = valueArray[DATA][i]>dataMax ? valueArray[DATA][i] : dataMax;  
      }  
    }
  
    // calibrate the middle point
    if(len==windowSize){
      calData = (valueArray[DATA][middleIndex]-sum/windowSize)/(dataMax-dataMin)*100+50;  
      middleIndex = (middleIndex+1)%windowSize;
    }

    // update the DIFF if possible 
    if(len>2) {
      tmp0 = (index-2+windowSize)%windowSize;
      tmp1 = (index-1+windowSize)%windowSize
      valueArray[DIFF][tmp1] = valueArray[DIFF][index]-valueArray[DIFF][tmp0];
    }
    
    // print it
    Serial.print(calData);
    Serial.print("\n");
  
    // remove the delay
    //delay(1);
  }
}
