import serial
import time

# NOTE the user must ensure that the serial port and baudrate are correct
serPort = "/dev/ttyS80"
baudRate = 9600
ser = serial.Serial(serPort, baudRate)

sensorPin = A0    # select the input pin for the potentiometer
ledPin = 13       # select the pin for the LED
sensorValue = 0   # variable to store the value coming from the sensor
doneCalibrating = 10
windowSize = 100 #const
int button = 9

#for Manchester 4:5 encoding
preCode = 21 #const
fakePreCode = 26 #const
decoding = [
  0b0000, 0b0000, 0b0000, 0b0000, 0b0000, 0b0000, 0b0000, 0b0000,
  0b0000, 0b0001, 0b0100, 0b0101, 0b0000, 0b0000, 0b0110, 0b0111,
  0b0000, 0b0000, 0b1000, 0b1001, 0b0010, 0b0011, 0b1010, 0b1011,
  0b0000, 0b0000, 0b1100, 0b1101, 0b1110, 0b1111, 0b0000, 0b0000
]

# circuler buffer
ignoreOnes = False
dataReceived[30]
preCodeFlag = False
printOutput = 0
symbol = 0
newBit = False
idle = True
settlingPoint = 120 #const
averagePeriod = 0
period;#used to be double
valueArray = [[0]*[windowSize]]*2
tPeriod = [windowSize]
DATA = 0
DIFF = 1
maxAndMinDiffIndex = [windowSize]
lastAverage
index, indexcounter, counter, periodIndex, symbolCounter = 0
len = 0
sum, sum2, sum3 = 0
prev = 0
#int diffPrev = [0,0];#first element is Diffvalue second is Index
dataMin = 1024
dataMax = 0
diffMax = 0
diffMin = 50
middleIndex = 50
calData = 0
i, j, k = 0
tmp0, tmp1, tmp2 = 0
upperCutDiff = 0
lowerCutDiff = 0
indexMaxMin = 0
newMax = -1022
newMin = 1023
sampleCounter=0;

def setup():
# declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT)
  pinMode(doneCalibrating, OUTPUT)
  pinMode(button, INPUT)
  Serial.begin(9600)

def calibrationFunc():
    sensorValue = analogRead(sensorPin)

    # update the valueArray and its params
    prev = valueArray[DATA][index]
    valueArray[DATA][index] = sensorValue
    index = (index + 1) % windowSize
    len = (len + 1) > windowSize ? windowSize : len + 1
    sum = sum - prev + sensorValue
    if ( prev == dataMin ):
        dataMin = 1024;
        for i in range(windowSize): # TODO: this loop is crazy; needs optimization
            dataMin = valueArray[DATA][i] < dataMin ? valueArray[DATA][i] : dataMin
    elif (prev == dataMax):
        dataMax = 0
        for i in range(windowSize): # TODO: this loop is crazy; needs optimization
            dataMax = valueArray[DATA][i] > dataMax ? valueArray[DATA][i] : dataMax
    if (sensorValue > dataMax):
        dataMax = sensorValue
    elif (sensorValue < dataMin):
        dataMin = sensorValue
    # calibrate the middle point
    if (len == windowSize):
        calData = (valueArray[DATA][middleIndex] - sum / windowSize) / (dataMax - dataMin) * 100 + 50
        middleIndex = (middleIndex + 1) % windowSize







def loop():
    while(digitalRead(button))
    print("Button works")
    #read the value from the sensor:
    while(1):
        #this is calibration loop
        calibrationFunc()
        if (len > 2):
            tmp0 = (index - 2 + windowSize) % windowSize
            tmp1 = (index - 1 + windowSize) % windowSize
            prev = valueArray[DIFF][tmp1]
            valueArray[DIFF][tmp1] = valueArray[DATA][index] - valueArray[DATA][tmp0]
            if ( prev == diffMin):
                diffMin = 1024
                for i in range(windowSize): # TODO: this loop is crazy; needs optimization
                    diffMin = valueArray[DIFF][i] < diffMin ? valueArray[DIFF][i] : diffMin

            elif (prev == diffMax):
                diffMax = 0
                for i in range(windowSize): # TODO: this loop is crazy; needs optimization
                    diffMax = valueArray[DIFF][i] > diffMax ? valueArray[DIFF][i] : diffMax
            if (valueArray[DIFF][tmp1] > diffMax):
                diffMax = valueArray[DIFF][tmp1]
            elif (valueArray[DIFF][tmp1] < diffMin):
                diffMin = valueArray[DIFF][tmp1]
            upperCutDiff = diffMax - ((diffMax - diffMin) / 10)
            lowerCutDiff = diffMin + ((diffMax - diffMin) / 10)
        if (valueArray[DIFF][tmp1] >= upperCutDiff): #finding values above uppercut for DIFF
            if (newMax == -1023):
                maxAndMinDiffIndex[indexcounter] = indexMaxMin
                #        print((maxAndMinDiffIndex[indexcounter] - maxAndMinDiffIndex[(indexcounter+windowSize-1)%windowSize]+windowSize)%windowSize);//TODO: needs to be fixed
                prev = tPeriod[indexcounter]
                tPeriod[indexcounter] = (maxAndMinDiffIndex[indexcounter] - maxAndMinDiffIndex[(indexcounter+windowSize-1)%windowSize]+windowSize)%windowSize #finding individual periods
                sum3 += tPeriod[indexcounter]-prev
                #        print(sum3/windowSize);
                periodIndex += 1

                indexcounter = (indexcounter + 1) % windowSize
                #        print (indexMaxMin);
                #        print("\n");
                #        print("newMin = ");
                #        print (newMin);
                #        print("\n");
            newMin = 1024
            indexMaxMin = valueArray[DIFF][tmp1] > newMax ? tmp1 : indexMaxMin
            newMax = valueArray[DIFF][tmp1] > newMax ? valueArray[DIFF][tmp1] : newMax
            #            print(valueArray[DIFF][tmp1]);
            #            print("\n");
        elif (valueArray[DIFF][tmp1] <= lowerCutDiff) { #finding values above uppercut for DIFF
            if (newMin == 1024):
                maxAndMinDiffIndex[indexcounter] = indexMaxMin
                #        print((maxAndMinDiffIndex[indexcounter] - maxAndMinDiffIndex[(indexcounter+windowSize-1)%windowSize]+windowSize)%windowSize);//TODO: needs to be fixed
                prev = tPeriod[indexcounter]
                tPeriod[indexcounter] = (maxAndMinDiffIndex[indexcounter] - maxAndMinDiffIndex[(indexcounter+windowSize-1)%windowSize]+windowSize)%windowSize #finding individual periods
                sum3 += tPeriod[indexcounter]-prev
                #        print(sum3/windowSize);
                periodIndex += 1

                indexcounter = (indexcounter + 1) % windowSize
                #        print (indexMaxMin);
                #        print("\n");
                #        print("newMax = ");
                #        print(newMax);
                #        print("\n");
            newMax = -1023
            indexMaxMin = valueArray[DIFF][tmp1] < newMin ? tmp1 : indexMaxMin
            #      print("Diff = ");
            #      print(valueArray[DIFF][tmp1]);
            #      print("\n");
            newMin = valueArray[DIFF][tmp1] < newMin ? valueArray[DIFF][tmp1] : newMin
            #            print(valueArray[DIFF][tmp1]);
            #            print("\n");
        if (periodIndex == settlingPoint):
            break
        #    print(valueArray[DIFF][tmp1]);
        #    print("\n");
        #    if(indexMaxMin >= windowSize){
        #          for(int a = 49; a < windowSize; a++){
        #            sum2 += maxAndMinDiffIndex[a] - maxAndMinDiffIndex[a-1];
        #            counter++;
        #          }
        #          period = sum2/counter;
        #    }

        #     print it
        #    print(sum3/windowSize);
        #    print("\n");
        delayMicroseconds(2)
    print("\n")
    print("Calibration Done!")
    print("\n")
    period = sum3/windowSize*1.0
    sampleCounter = 0
    print(period)
    print("\n")
    digitalWrite(doneCalibrating, HIGH)
    delay(100)
    digitalWrite(doneCalibrating, LOW)
    delay(2000)
    indexReceived = 0
    newIdx = 0
    while(1):
        sensorValue = analogRead(sensorPin)
        calData = (sensorValue - sum / windowSize) / (dataMax - dataMin) * 100 + 50 #can be replaced with up and lower bounds representiting 0 and 1
        #print(calData)
        if(ignoreOnes && calData > 80):
            ignoreOnes = False;
        elif(idle && calData < 50):
            #        print("A");
            #        print(testCounter);
            #        symbol = (symbol << 1) + 1;
            symbolCounter = 0
            idle = !idle
            #        print("Precode");
            sampleCounter = (period)/2
        elif(!idle):
            sampleCounter -= 1
            #        print(calData);
            #        print(sampleCounter);
            if(sampleCounter < 0):
                #print(calData);
                #print(testCounter);
                if(calData < 50):
                    symbol = (symbol << 1) + 1
                #          print(symbol);
                #          print(1);
                else:
                    symbol = (symbol << 1) + 0
                #          print(symbol);
                #          print(0);
                symbolCounter++
                if(symbolCounter % 5 == 0):
                    if(!preCodeFlag && symbol == preCode):
                        preCodeFlag = True
                        symbol = 0
                        symbolCounter = 0
                    elif (!preCodeFlag && symbol == fakePreCode):
                        symbol = symbol & 0x0f
                        #                  print(symbol);
                        symbolCounter = 4
                    elif(!preCodeFlag):
                        #              print(symbol);
                        symbol = 0
                        idle = !idle
                        symbolCounter = 0
                    elif(preCodeFlag):
                        #                print(int(decoding[symbol]));
                        if(indexReceived%6 == 5): #add ending with 0 before switching to nonidle
                            #ignoreOnes = true;
                            idle = !idle
                            preCodeFlag = False
                        #                if(indexReceived == 23){
                        #                  break;
                        #                }
                        indexReceived++;#modulate by 6
                        if(symbolCounter == 10):
                            printOutput = ( decoding[symbol] << 4) + printOutput
                            #                    print("2)");
                            print(char(printOutput))
                            #                    dataReceived[newIdx] = char(printOutput);
                            newIdx += 1
                            if(printOutput == 0):
                                break
                            printOutput = 0
                            symbolCounter = 0
                        else:
                            printOutput = decoding[symbol]
                        #                    print("1)");
                        #                    print(printOutput);
                        symbol = 0
                        #                  print("Symbol = ");
                        #                  print(symbol);

                        #                  while(1);
            sampleCounter += period

        #      1010
        delayMicroseconds(2)
    #  print(newIdx);
    #  for(i = 0; i < 24; i++){
    #   print(dataReceived[i]);
    #  }
