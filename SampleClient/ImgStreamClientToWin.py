# -*- coding: utf-8 -*-
"""
ImgStreamClient.py
Created on Mon Oct 25 23:58:53 2021
"""
import cv2
import socket
import sys
import numpy as np
import datetime

import ImgRecFunction

HOST = 'localhost'
PORT = 8080
ADMISSIONCODE = "333"

if __name__ == '__main__':
    rec = ImgRecFunction.RecFunction()
    client =socket.socket(socket.AF_INET,socket.SOCK_STREAM)
    #Connect CameraService
    try:
        client.connect((HOST,PORT))
    except:
        print("Connect Fail")
        sys.exit()
    #Send Addmission Code.
    client.send(ADMISSIONCODE.encode('utf-8')) 
    #GetFramInfo From Service.
    if False == rec.GetFrameInf(client):
        client.close()
        sys.exit()
        
    while True:
        try:
            RecvImg = client.recv(16384)
             #Convert to Image Matrix.
            img_array = np.frombuffer(RecvImg, dtype=np.uint8)
            frame = cv2.imdecode(img_array, cv2.IMREAD_COLOR)         
            if frame is not None:
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
                break
        except:
               break
           
    rec.Close()
    cv2.destroyAllWindows()
    client.close()