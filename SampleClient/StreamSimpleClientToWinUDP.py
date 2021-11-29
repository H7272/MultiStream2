# -*- coding: utf-8 -*-
"""
StreamSimpleClientToWinUDP.py
Created on Mon Oct 25 23:58:53 2021
"""
import cv2
import socket
import sys
import numpy as np
import ImgRecFunction

HOST = 'localhost'
PORT = 50000
DATASIZE = 32678
STARTCODE = "START"
ENDCODE = "QUITE"

if __name__ == '__main__':
   
    
    rec = ImgRecFunction.RecFunction()
    client = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)
    print(client.gettimeout())
    
    #Send Start Code.
    client.sendto(STARTCODE.encode('utf-8'),(HOST,PORT)) 
    
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
                #Send Quite.
                client.sendto("QUITE".encode('utf-8'),(HOST,PORT)) 
                break
            # Send Alive.
            if count > 20:
                client.sendto("Alive".encode('utf-8'),(HOST,PORT))
                count = 0
            count += 1
        except:
               break
           
    rec.Close()
    cv2.destroyAllWindows()
    client.close()