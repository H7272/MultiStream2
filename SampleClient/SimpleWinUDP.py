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
PORT = 5000
DATASIZE = 65300
STARTCODE = "START"
ENDCODE = "QUITE"

if __name__ == '__main__':
   
    #CreateSocket
    client = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

    #Send Start Code.
    client.sendto(STARTCODE.encode('utf-8'),(HOST,PORT)) 
    
    #GetFramInfo From Service.
    try:
        FrameInfo = client.recv(32).decode('shift_jis')
    except:
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
               cv2.imshow('frame',frame)
               
            #ColseInput'q'On Frame.
            key = cv2.waitKey(1)
   
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
           
    cv2.destroyAllWindows()
    client.close()