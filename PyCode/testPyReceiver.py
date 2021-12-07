import serial
import time
import os, fnmatch
import matplotlib.pyplot as plt
import collections
import threading
import queue
import numpy as np
from scipy.signal import argrelextrema

SAVE_COUNT = 2000
BUFFER_LEN = 500
SIG_ONE = 30
SIG_ZERO = 80
SIG_MID = 50
PKT_LEN = 20
PREAMBLE_CODE = 21
decoding = [
  0b0000, 0b0000, 0b0000, 0b0000, 0b0000, 0b0000, 0b0000, 0b0000,
  0b0000, 0b0001, 0b0100, 0b0101, 0b0000, 0b0000, 0b0110, 0b0111,
  0b0000, 0b0000, 0b1000, 0b1001, 0b0010, 0b0011, 0b1010, 0b1011,
  0b0000, 0b0000, 0b1100, 0b1101, 0b1110, 0b1111, 0b0000, 0b0000
]

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
sample_q = queue.Queue()
cal_q = queue.Queue()
payload_q = queue.Queue()
cal_event = threading.Event()
signal_range = 0
signal_aver = 0
period = 0

def printFig(dataList, name):
    res = []
    for a in dataList:
        try:
            res.append(int(a))
        except ValueError:
            pass
    plt.plot(range(len(res)), res)
    plt.show()
    plt.savefig(name)

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

def basic_print():
    cnt = 0
    printList = []
    global ser
    while True:
        #buf = readInt()
        cnt += 1
        buf = ser.read_until(expected=b'\n')
        printList.append([time.time(), int(buf)])
        sum=0

        if cnt == 100:
            break
    printFig([x[1] for x in printList], "sensorValue")
    printFig([x[0]*1000//1%1000 for x in printList[:-1]], "time")
    printFig([(printList[x+1][0]-printList[x][0])*100000 for x in range(len(printList)-1)], "time-diff")
    print(printList)

def sampling_thread(que):
    print("[SAMPLING] start ...")
    while True:
        #buf = readInt()
        global circular_buf
        global ser
        buf = ser.read_until(expected=b'\n')
        try:
            que.put([int(buf), time.time()])
        except ValueError:
            pass
    print("[SAMPLING] done!")

def calibration_thread(que, event):
    print("[CALIBRATION] start ...")
    buffer =  collections.deque(maxlen=BUFFER_LEN)
    cnt = 0
    while True:
        new_data = que.get()
        buffer.append(new_data[0])
        if len(buffer) < BUFFER_LEN:
            continue
        else:
            if cnt < SAVE_COUNT:
                cnt+=1
                continue
            else:
                max_ = max(buffer)
                min_ = min(buffer)
                global signal_aver
                global signal_range
                signal_aver = sum(buffer) / len(buffer)
                signal_range = max_ - min_
                data_list = [ (a-signal_aver)/signal_range*100.0+50 for a in buffer]
                diff_list = [ data_list[a+1] - data_list[a] for a in range(len(data_list)-1)]
                diff_max = max(diff_list)
                diff_min = min(diff_list)
                diff_upper_cut = diff_max - 0.2*(diff_max-diff_min)
                diff_lower_cut = diff_min + 0.2*(diff_max-diff_min)
                upper_list = []
                lower_list = []
                tmp = []
                find_flag = "up"
                for i, v in enumerate(diff_list):
                    if v > diff_upper_cut and find_flag == "up":
                        tmp.append((v, i))
                    elif v > diff_upper_cut and find_flag == "down":
                        if tmp:
                            lower_list.append(min(tmp)[1])
                        tmp=[]
                        find_flag = "up"
                    elif v < diff_lower_cut and find_flag == "down":
                        tmp.append((v, i))
                    elif v < diff_lower_cut and find_flag == "up":
                        if tmp:
                            upper_list.append(max(tmp)[1])
                        tmp=[]
                        find_flag = "down"

                global period
                upper_period = [ upper_list[a+1] - upper_list[a] for a in range(len(upper_list)-1)]
                lower_period = [ lower_list[a+1] - lower_list[a] for a in range(len(lower_list)-1)]
                upper_period = upper_period[1:-1] if len(upper_period) > 2 else []
                lower_period = lower_period[1:-1] if len(lower_period) > 2 else []
                period = np.average(np.concatenate([upper_period, lower_period]))/2.0
                print(period)
                #period = np.average(np.diff(np.sort(np.concatenate([local_max_index, local_min_index]))))
                break
    event.set()
    print("[CALIBRATION] done!")

def preprocess_sig_thread(que_in, que_out, event):
    event.wait()
    print("[PREPROCESS] wait calibration done")
    while True:
        try:
            tmp = que_in.get(True, 1)
            # Waits for 3 seconds, otherwise throws `Queue.Empty`
        except queue.Empty:
            break
    print("[PREPROCESS] start ...")
    global signal_aver
    global signal_range
    with que_in.mutex:
        que_in.queue.clear()
    good_time = time.time()+1.2
    good = False
    while True:
        in_data = que_in.get()
        if good:
            que_out.put((in_data[0]-signal_aver)/signal_range*100.0+50)
        else:
            if time.time() > good_time:
                good = True

def sig_to_dig_test_thread(que_in, que_out, event):

    print("[SIG_TO_DIG] start ...")
    start_flag = False
    cur_index  = 0
    total_cnt  = 0
    whole_str  = ""
    prev_flag  = -1
    preamble_check = False
    preamble_detect = 0
    global period
    event.wait()
    with que_in.mutex:
        que_in.queue.clear()
    while True:
        cal_data = que_in.get()
        if cal_data < SIG_ONE and not start_flag:
            start_flag = True
            cur_index = period
            whole_str = "1"
            #que_out.put(1)
            total_cnt=1
        elif start_flag:
            cur_index = cur_index-1
            if cur_index < 1.0:
                if cal_data < SIG_MID:
                    whole_str = whole_str+"1"
                else:
                    whole_str = whole_str+"0"

                total_cnt += 1
                cur_index += period

            if total_cnt>200:
                break
    print("pkt_len:", total_cnt)
    print("whole_str:", whole_str)

def sig_to_dig_thread(que_in, que_out, event):
    event.wait()
    print("[SIG_TO_DIG] start ...")
    start_flag = False
    cur_index  = 0
    whole_str  = ""
    preamble_check = False
    symbol_paresed = 0
    bitcount = 0
    global period
    with que_in.mutex:
        que_in.queue.clear()
    while True:
        cal_data = que_in.get()
        if cal_data < SIG_MID and not start_flag:
            print("[TRIGGER CAL DATA]", cal_data)
            start_flag = True
            preamble_check = False
            cur_index = period/3
            symbol_paresed = 0
        elif start_flag:
            cur_index = cur_index-1
            if cur_index < 1.0:
                if cal_data < SIG_MID:
                    symbol_paresed = ((symbol_paresed<<1)+1)&0x1F
                else:
                    symbol_paresed = ((symbol_paresed<<1)+0)&0x1F
                cur_index += period
                bitcount  += 1
                if not preamble_check:
                    # print(symbol_paresed)
                    if symbol_paresed == PREAMBLE_CODE:
                        preamble_check = True
                        symbol_paresed = 0
                        bitcount = 0
                        total_cnt = 0
                    else:
                        continue
                elif bitcount==5:
                    total_cnt += 1
                    #print(symbol_paresed)
                    que_out.put(symbol_paresed)
                    symbol_paresed = 0
                    bitcount = 0
                    if total_cnt == PKT_LEN:
                        total_cnt = 0
                        start_flag = False


def extract_payload_thread(que_in, event):
    event.wait()
    print("[PAYLOAD_EXTRCTION] start ...")
    while True:
        data = que_in.get()
        # print(data)

        data = decoding[data]
        payload_halfbyte = data
        data = que_in.get()
        # print(data)

        data = decoding[data]
        payload_byte = payload_halfbyte + (data<<4)
        print(chr(payload_byte), end = ' ')
        print(payload_byte)


        payload_byte = 0

DEBUG = 0

if DEBUG:
    basic_print()
else:
    # def receiving_thread(que, event):
    thread_sampling           = threading.Thread(target = sampling_thread, args = (sample_q, ))
    thread_calibration        = threading.Thread(target = calibration_thread, args = (sample_q, cal_event, ))
    thread_preprocess_sig     = threading.Thread(target = preprocess_sig_thread, args = (sample_q, cal_q, cal_event, ))
    thread_sig_to_dig         = threading.Thread(target = sig_to_dig_thread, args = (cal_q, payload_q, cal_event, ))
    thread_payload_extraction = threading.Thread(target = extract_payload_thread, args = (payload_q, cal_event, ))
    #basic_print()

    # start all threads
    thread_sampling.start()
    thread_calibration.start()
    thread_preprocess_sig.start()
    thread_sig_to_dig.start()
    thread_payload_extraction.start()

    # join all threads
    thread_sampling.join()
    thread_calibration.join()
    thread_preprocess_sig.join()
    thread_sig_to_dig.join()
    thread_payload_extraction.join()
