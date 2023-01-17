import numpy as np
import cv2 as cv
import math

img = cv.imread("tes.png")
gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
ret, img_map = cv.threshold(gray, 50, 255, cv.THRESH_BINARY_INV)

r=20 #radius robot
angl=30 #line scan 360/1

def calculate_distance(img,pos,r,angl):
  line_dist = []
  line_pos = np.empty(shape=(0,2))
  for i in range(0,360,angl):
    angle = int(pos[2]+i)
    if angle >= 360:
      angle -= 360
    x = int(pos[0] + r*math.cos(angle*math.pi/180))
    y = int(pos[1] + r*math.sin(angle*math.pi/180))
    x1 = x 
    y1 = y 

    angle_ratio = math.tan(angle*math.pi/180)
    if angle <= 90:
      xadd = 1
      yadd = 1
    elif angle > 90 and angle <= 180:
      xadd = -1
      yadd = 1
    elif angle >180 and angle <= 270:
      xadd = -1
      yadd = -1
    else:
      xadd = 1
      yadd = -1

    if x>=img.shape[1] or x<=0 or y>=img.shape[0] or y<=0:
      x = x1
      y = y1
      line_pos = np.vstack((line_pos,[x,y]))
      distance = math.sqrt((y-y1)**2+(x-x1)**2)
      line_dist = np.append(line_dist,distance)
      continue
    
    while img[y,x] != 255:
      if angle_ratio >= 0:
        if x==x1:
          x += xadd
        else:
          if (y-y1)/(x-x1) < angle_ratio:
            y += yadd
          else:
            x += xadd
      else:
        if x==x1:
          x += xadd
        else:
          if (y-y1)/(x-x1) < angle_ratio:
              x += xadd
          else:
              y += yadd
      if x>=img.shape[1] or x<=0 or y>=img.shape[0] or y<=0:
        x = x1
        y = y1
        break
    line_pos = np.vstack((line_pos,[x,y]))
    distance = math.sqrt((y-y1)**2+(x-x1)**2)
    line_dist = np.append(line_dist,distance)
  return line_pos,np.float32(line_dist)

print("ok")
distance_x = np.empty(shape=(0,img_map.shape[0],360),dtype=np.float32)
for i in range(img_map.shape[1]):
  distance_y = np.empty(shape=(0,360),dtype=np.float32)
  for j in range(img_map.shape[0]):
    _,line_distance = calculate_distance(img_map,(i,j,0),r,1)
    distance_y = np.vstack((distance_y,line_distance))
  distance_x = np.vstack((distance_x,[distance_y]))
  print(distance_x.shape)
np.save('data_map.npy', distance_x)
