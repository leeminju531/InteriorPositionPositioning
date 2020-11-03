import threading
import time
import serial
import numpy as np
import csv
import sys
# 코드 시작 전에, Collection 이 경로에 있는지 먼저 확인할 것!
port = 'COM8'
baud = 9600
# ★★★★★★★데이터를 수집하기 전에 해당 라벨의 좌표를 입력해야함 ★★★★★★★★★★
LABEL_X = 9
LABEL_Y = 6
#★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★★


ser = serial.Serial(port,baud)

startCollecting = False
dataManage = False

Lock  = threading.Lock()
data = " "
rssi = [0,0,0,0]

exLock = threading.Lock()
exception = False

def checkData():
    global dataManage
    global data
    global exception
    global ser
    while(1) :
        try:
            if ser.readable:

                res = ser.readline()
                code = res.decode()[:len(res)-1]
                data = str(code)
                print(data)
                
                isFail = data[0]+data[1]
                if data[0] == '/' :
                    Lock.acquire()
                    dataManage = True
                    Lock.release()   
                elif isFail == '-1' :
                    print('전체 스캔 안됨 재확인 요망')
                           
        except :
            print('PORT 선 뽑힘')
            exLock.acquire()
            exception = True
            exLock.release()
            return


        
        # 지정된 데이터가 들어오면 dataManage = true로 써주기


if __name__ == '__main__':
    Thread = threading.Thread(target=checkData)
    Thread.daemon = True # 메인루프 종료시 쓰레드 같이 종료
    Thread.start()

    rssi_data = open('Collection_Rssi.csv','a', newline='') #'a' 옵션: append 를 의미 
    wr = csv.writer(rssi_data)

    while(1) :
        
        #1)  포트 뽑으면 메인 스레드 바로 종료되도록 만들기
        Lock.acquire()
        if dataManage == True:
            dataManage = False
            rssi[0] = int(data[1]+data[2])
            rssi[1] = int(data[3]+data[4])
            rssi[2] = int(data[5]+data[6])
            rssi[3] = int(data[7]+data[8])

            startCollecting = True
        
        Lock.release()
        
        if startCollecting == True:
            startCollecting = False
            
            wr.writerow([ rssi[0],rssi[1],rssi[2],rssi[3],LABEL_X,LABEL_Y ])
            print('Feature)  rssi1 : {}, rssi2 : {}, rssi3 : {}, rssi4 : {}'.format(rssi[0],rssi[1],rssi[2],rssi[3]))
            print('Label)  X:{}, Y:{} 수집 완료'.format(LABEL_X,LABEL_Y))
           
    
        
        exLock.acquire()
        if exception == True:
            print('프로세스 강제 종료')
            rssi_data.close() 
            sys.exit()
        exLock.release()


        
            
     



