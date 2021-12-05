# -*- coding: utf-8 -*-
"""
StreamSimpleClientToWinUDP.py
Created on Mon Oct 25 23:58:53 2021
"""
import cv2
import socket
import sys
import numpy as np
import datetime
from pyzbar.pyzbar import decode, ZBarSymbol
import ImgRecFunction

HOST = 'localhost'
PORT = 5000
DATASIZE = 65300
STARTCODE = "START"
ENDCODE = "QUITE"

if __name__ == '__main__':
    
    print(cv2.__version__)
    rec = ImgRecFunction.RecFunction()   
    font = cv2.FONT_HERSHEY_SIMPLEX
    #CreateSocket
    client = socket.socket(socket.AF_INET,socket.SOCK_DGRAM)

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
               height = frame.shape[0]
               width = frame.shape[1]
               # QRコードデコード
               value = decode(frame, symbols=[ZBarSymbol.QRCODE])

               if value:
                   for qrcode in value:
                       # QRコード座標取得
                       x, y, w, h = qrcode.rect

                       # QRコードデータ
                       dec_inf = qrcode.data.decode('utf-8')
                       print('dec:', dec_inf)
                       img_bgr = cv2.putText(frame, dec_inf, (x, y - 6), font, .3, (255, 0, 0), 1, cv2.LINE_AA)

                       # バウンディングボックス
                       cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)

                # 画像表示    
               rec.Imshow(frame, '2DReader') 
               
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