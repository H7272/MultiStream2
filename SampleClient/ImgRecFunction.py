# -*- coding: utf-8 -*-
"""
ImgRecFunction.py
Created on Wed Nov  3 00:33:47 2021
"""
import cv2
import datetime

STATE_OFF = 0
STATE_ON  = 1 
class RecFunction:
    def  __init__(self):
        self.state = STATE_OFF
        self.writer = cv2.VideoWriter()
        self.fourcc = cv2.VideoWriter_fourcc('m', 'p', '4', 'v')
        self.FILENAME = 'video.mp4' 
        self.WIDTH = 640
        self.HIGHT = 480
        self.FPS = 25
        
    def ChangeState(self):
        if STATE_OFF == self.state: 
            self.state = STATE_ON
            self.writer = self.getwriter(datetime.datetime.now().strftime('%Y%m%d_%H%M%S'))
            return True
        if STATE_ON == self.state:
            self.state = STATE_OFF
            self.writer.release()
            return True
        return False
    
    def GetFrameInf(self,client):
        try:
            FrameInfo = client.recv(32).decode('shift_jis')
        except:
            return False
        self.SetCaption(list(map(int, FrameInfo.split(',')[0:3])))
        return True
        
    def SetCaption(self,caption):
        self.WIDTH = caption[0]
        self.HIGHT = caption[1]
        self.FPS = caption[2]
        
    def getwriter(self, dayst):
         return cv2.VideoWriter(dayst+self.FILENAME, self.fourcc , self.FPS , (self.WIDTH , self.HIGHT))
      
    def Imshow(self, frame):
        if STATE_OFF == self.state:
            cv2.imshow('frame',frame)
            return
        cv2.rectangle(frame, (5, 5), (self.WIDTH-5, self.HIGHT-5), (0, 0, 255)) #TOP_LEFT,BOTTTOM_RIGHT,COLOR
        cv2.imshow('frame',frame)
        self.writer.write(frame)
        return

    def Close(self):
        self.state = STATE_OFF
        self.writer.release()
        return