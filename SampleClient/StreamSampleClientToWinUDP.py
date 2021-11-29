# -*- coding: utf-8 -*-
"""
IStreamSampleClientToWinUDP.py
Created on Mon Oct 25 23:58:53 2021
"""
import cv2
import socket
import sys
import numpy as np
import datetime
import ImgRecFunction

HOST = 'localhost'
PORT = 50000
DATASIZE = 32678
STARTCODE = "START"
ENDCODE = "QUITE"

if __name__ == '__main__':
    print(cv2.__version__)
    fgbg = cv2.bgsegm.createBackgroundSubtractorMOG()
    
    hog = cv2.HOGDescriptor()
    hog.setSVMDetector(cv2.HOGDescriptor_getDefaultPeopleDetector())
    hogParams = {
    "winStride":(8,8),
    "padding":(0,0),
    "scale":1.05,
    "hitThreshold":0,
    "finalThreshold":1
    }
    
    rec = ImgRecFunction.RecFunction()
    client = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    print(client.gettimeout())
    
    #Send Addmission Code.
    client.sendto(STARTCODE.encode('utf-8'),(HOST,PORT)) 
    #data = client.recv(64).decode('utf-8')  
    #GetFramInfo From Service.
    if False == rec.GetFrameInf(client):
        client.close()
        sys.exit()
    client.settimeout(20)    
    count = 0
    while True:
        try:
            RecvImg = client.recv(DATASIZE)
             #Convert to Image Matrix.
            img_array = np.frombuffer(RecvImg, dtype=np.uint8)
            frame = cv2.imdecode(img_array, cv2.IMREAD_COLOR)         
            if frame is not None:
               gframe = fgbg.apply(frame)
               human, r = hog.detectMultiScale(gframe, **hogParams)
               if (len(human)>0):
                   print("Detect",len(human)
                         )
                   for (x, y, w, h) in human:
                       cv2.rectangle(frame, (x, y), (x+w, y+h), (255,255,255), 3)
               rec.Imshow(frame) 
            #ColseInput'q'On Frame, Input'r' On Frme Stat or Stop Rec.
            key = cv2.waitKey(1)
            #Rec.
            if key & 0xFF == ord('r'):
                if False == rec.ChangeState():
                    break
                continue
            #Close.   
            if key & 0xFF == ord('q'):
                client.sendto("QUITE".encode('utf-8'),(HOST,PORT)) 
                break
            if count > 20:
                client.sendto("Alive".encode('utf-8'),(HOST,PORT))
                count = 0
            count += 1
        except:
               break
           
    rec.Close()
    cv2.destroyAllWindows()
    client.close()