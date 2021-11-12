import serial
import time
import os, fnmatch

def find(pattern, path):
    result = []
    for root, dirs, files in os.walk(path):
        for name in files:
            if fnmatch.fnmatch(name, pattern):
                result.append(os.path.join(root, name))
    return result

# NOTE the user must ensure that the serial port and baudrate are correct
serPort = find('*tty.usbmodem*', '/dev/')[0]
baudRate = 115200
ser = serial.Serial(serPort, baudRate)

def readInt():
    getChar = ser.read()
    strNum = ''
    while getChar == b'\n':
        getChar = ser.read()
    while getChar != b'\n':
        strNum = strNum+getChar.decode("utf-8")
        getChar = ser.read()
    strNum = strNum+getChar.decode("utf-8")
    return strNum

cnt = 0
printList = []
while True:
    #buf = readInt()
    cnt += 1
    buf = ser.read_until(expected=b'\n')
    printList.append([time.time(), int(buf)])
    if cnt == 1000:
        break
print(printList)
