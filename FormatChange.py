import cv2
import numpy as np
from os import path
import os
filepath = 'DragonBaby\img/'
filepath2 = 'DragonBabyBMP/'
j=0
for i in os.listdir(filepath):
    j=j+1
    imgpath = filepath + i
    # print(filepath+i, file=data)
    # print(filepath2+i, file=data2)
    image1 = cv2.imread(imgpath)
    # image2 = cv2.imread(filepath2+i)
    # image = np.concatenate([image1, image2], axis=1)
    # videoWriter.write(image)
    cv2.imwrite(filepath2+'%04d.bmp'%j , image1)

data = open('DragonBaby.txt', 'w+')
data2= open('DragonBabyeout.txt','w+')
filepath2 = 'DragonBabyBMP/'
filepath3='DragonBabyResult/'
for i in os.listdir(filepath2):
    print(filepath2+i, file=data)
    print(filepath3+i, file=data2)

image_names = []
image_arr = []

fps =5
size=(640, 360)
videoWriter = cv2.VideoWriter("video.avi",cv2.VideoWriter_fourcc('M','J','P','G'),fps,size)
filepath4= 'baby/'
for i in os.listdir(filepath4):
    image2 = cv2.imread(filepath4+i)
    # image = np.concatenate([image1, image2], axis=1)
    videoWriter.write(image2)









