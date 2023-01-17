### import the opencv library
##import cv2
##import numpy as np
##import socket
##import struct
##import select
##
##lapangan = cv2.imread('map_lapangan.jpg')
####lapangan = cv2.resize(map_lapangan, (550,450), interpolation = cv2.INTER_AREA)
##trajectory=np.zeros(shape=(0,2))
##traject_real=np.zeros(shape=(0,2))
###terima data dari robot
##UDP_IP = "172.16.80.189"
##UDP_PORT = 8787
##sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
##sock.setblocking(0)
##sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1)
##sock.bind((UDP_IP, UDP_PORT))
### define a video capture object
##cap = cv2.VideoCapture(1)
##width = cap.get(cv2.CAP_PROP_FRAME_WIDTH)
##height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
##mtx = np.load('mtx.npy')
##dist = np.load('dist.npy')
##newcameramtx = np.load('newcameramtx.npy')
##polygon = [[[3,2],[305,0],[297,198],[12,192]]]
##status_tracking = False
##datax = []
##datay = []
####untuk threshold hsv
##cv2.namedWindow('thrs', cv2.WINDOW_AUTOSIZE)
##cv2.resizeWindow('thrs', 480, 360)
##lowH = np.load('lowH.npy')
##highH = np.load('highH.npy')
##lowS = np.load('lowS.npy')
##highS = np.load('highS.npy')
##lowV = np.load('lowV.npy')
##highV = np.load('highV.npy')
##def callback(x):
##  pass
### create trackbars for color change
##cv2.createTrackbar('lowH','thrs',lowH,179,callback)
##cv2.createTrackbar('highH','thrs',highH,179,callback)
##cv2.createTrackbar('lowS','thrs',lowS,255,callback)
##cv2.createTrackbar('highS','thrs',highS,255,callback)
##cv2.createTrackbar('lowV','thrs',lowV,255,callback)
##cv2.createTrackbar('highV','thrs',highV,255,callback)
##
##
##
##while(True):
##  ready = select.select([sock], [], [], 1) #timeout 1s
##  if ready[0]:
##    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
##    robotx = struct.unpack('f', data[0:4])
##    roboty = struct.unpack('f', data[4:8])
##    mode = struct.unpack('h', data[8:10])
##    print("UDP receive")
##  else:
##    print("UDP not receive")
##  # Capture the video frame
##  ret, frame = cap.read()
##  M = cv2.getRotationMatrix2D((int(width/2), int(height/2)), 2.5, 1.0)
##  rotated = cv2.warpAffine(frame, M, (int(width), int(height))) ##rotate image
##  crop = rotated[120:int(height-90),155:int(width-113)] ##crop image
##  crop = cv2.rotate(crop, cv2.ROTATE_180) ##agar terlihat tidak terbalik
##  
##  ##undistort image
##  dst = cv2.undistort(crop, mtx, dist, None, newcameramtx)
##  h,  w = dst.shape[:2]
##  undistortion = dst[40:int(h-30),40:int(w-25)] ##crop image
##  # First find the minX minY maxX and maxY of the polygon
##  minX = undistortion.shape[1]
##  maxX = -1
##  minY = undistortion.shape[0]
##  maxY = -1
##  for point in polygon[0]:
##    x = point[0]
##    y = point[1]
##
##    if x < minX:
##        minX = x
##    if x > maxX:
##        maxX = x
##    if y < minY:
##        minY = y
##    if y > maxY:
##        maxY = y
##  # Go over the points in the image if thay are out side of the emclosing rectangle put zero
##  # if not check if thay are inside the polygon or not
##  cropedImage = np.zeros_like(undistortion)
##  for y in range(0,undistortion.shape[0]):
##    for x in range(0, undistortion.shape[1]):
##      if x < minX or x > maxX or y < minY or y > maxY:
##        continue
##      if cv2.pointPolygonTest(np.asarray(polygon),(x,y),False) >= 0:
##        cropedImage[y, x, 0] = undistortion[y, x, 0]
##        cropedImage[y, x, 1] = undistortion[y, x, 1]
##        cropedImage[y, x, 2] = undistortion[y, x, 2]
##  # Now we can crop again just the envloping rectangle
##  finalImage = cropedImage[minY:maxY,minX:maxX]
##  # Now strectch the polygon to a rectangle. We take the points that
##  polygonStrecth = np.float32([[0,0],[finalImage.shape[1],0],[finalImage.shape[1],finalImage.shape[0]],[0,finalImage.shape[0]]])
##  # Convert the polygon corrdanite to the new rectnagle
##  polygonForTransform = np.zeros_like(polygonStrecth)
##  i = 0
##  for point in polygon[0]:
##    x = point[0]
##    y = point[1]
##    newX = x - minX
##    newY = y - minY
##    polygonForTransform[i] = [newX,newY]
##    i += 1
##  # Find affine transform
##  M = cv2.getPerspectiveTransform(np.asarray(polygonForTransform).astype(np.float32), np.asarray(polygonStrecth).astype(np.float32))
##  # Warp one image to the other
##  warpedImage = cv2.warpPerspective(finalImage, M, (finalImage.shape[1], finalImage.shape[0]))
##  resized = cv2.resize(warpedImage, (450,300), interpolation = cv2.INTER_AREA)
##  h,  w = resized.shape[:2]
##  resized = resized[0:int(h-5),0:int(w-0)] ##crop image
##  h,  w = resized.shape[:2]
##  hsv = cv2.cvtColor(resized, cv2.COLOR_BGR2HSV)
##
##  # get trackbar positions and thershold image
##  lowH = cv2.getTrackbarPos('lowH', 'thrs')
##  highH = cv2.getTrackbarPos('highH', 'thrs')
##  lowS = cv2.getTrackbarPos('lowS', 'thrs')
##  highS = cv2.getTrackbarPos('highS', 'thrs')
##  lowV = cv2.getTrackbarPos('lowV', 'thrs')
##  highV = cv2.getTrackbarPos('highV', 'thrs')
##  lower_hsv = np.array([lowH, lowS, lowV])
##  higher_hsv = np.array([highH, highS, highV])
##  mask = cv2.inRange(hsv, lower_hsv, higher_hsv)
####  print(mask.shape)
##  
####  resized = cv2.circle(resized, (int(resized.shape[1]/2), int(resized.shape[0]/2)), 3, (255,0,0), -1)
####  resized = cv2.line(resized, (int(resized.shape[1]/2),0), (int(resized.shape[1]/2),int(resized.shape[0])), (0,0,0), 1)
####  resized = cv2.line(resized, (0,int(resized.shape[0]/2)), (int(resized.shape[1]),int(resized.shape[0]/2)), (0,0,0), 1)
##  contours, hierarchy = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)
##  # draw the biggest contour (c) in red
##  if len(contours) > 0:
##    c = max(contours, key = cv2.contourArea)
##    (x,y), r = cv2.minEnclosingCircle(c)
##    posx = 300 - ( 1.9327*((h/2)-y) + 1.1839 ) # - diganti + untuk ganti gawang
##    posy = 450 - ( 1.9536*((w/2)-x) - 0.8165 ) # - diganti + untuk ganti gawang
##    if status_tracking == True:
##      datax = np.append(datax,round(posx,1))
##      datay = np.append(datay,round(posy,1))
##      trajectory=np.vstack((trajectory,np.array([100+int(posy),100+int(posx)])))
##      traject_real=np.vstack((traject_real,np.array([100+int(roboty[0]),100+int(robotx[0])])))
##      lapangan = cv2.polylines(lapangan, [np.int32(trajectory)], isClosed=False, color=(255,0,0),thickness=3)
##      lapangan = cv2.polylines(lapangan, [np.int32(traject_real)], isClosed=False, color=(0,0,255),thickness=3)
####      print(datax)
####      print(datay)
##    resized = cv2.circle(resized, (int(x),int(y)), int(r), (0,0,255), 2)
##    resized = cv2.putText(resized, 'X:'+str(round(posx,1))+'|'+'Y:'+str(round(posy,1)), (int(resized.shape[1]-130),
##                      int(resized.shape[0]-7)), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,0,0), 1, cv2.LINE_AA)
##  
##  # Display the resulting frame
####  cv2.imshow('undistort', resized)
##  cv2.imshow('lapangan', lapangan)
####  cv2.imshow('mask', mask)
##  
##  c = cv2.waitKey(10)
##  if c == ord('p'):
##    status_tracking = not status_tracking
##  if c == 27:
##    break
##
### After the loop release the cap object
##sock.close()
##cap.release()
##cv2.destroyAllWindows()
##np.save('lowH.npy', lowH)
##np.save('highH.npy', highH)
##np.save('lowS.npy', lowS)
##np.save('highS.npy', highS)
##np.save('lowV.npy', lowV)
##np.save('highV.npy', highV)
##np.save('datax.npy', datax)
##np.save('datay.npy', datay)
##


# import the opencv library
import cv2
import numpy as np
import socket
import struct
import select

lapangan = cv2.imread('bgrMap.jpg')
##lapangan = cv2.resize(map_lapangan, (550,450), interpolation = cv2.INTER_AREA)
trajectory=np.zeros(shape=(0,2))
traject_real=np.zeros(shape=(0,2))
#terima data dari robot
UDP_IP = "172.16.80.118"
UDP_PORT = 8787
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock.setblocking(0)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_RCVBUF, 1)
sock.bind((UDP_IP, UDP_PORT))
# define a video capture object
cap = cv2.VideoCapture(1)
width = cap.get(cv2.CAP_PROP_FRAME_WIDTH)
height = cap.get(cv2.CAP_PROP_FRAME_HEIGHT)
mtx = np.load('mtx.npy')
dist = np.load('dist.npy')
newcameramtx = np.load('newcameramtx.npy')
polygon = [[[3,2],[305,0],[297,198],[12,192]]]
status_tracking = False
datax = []
datay = []



while(True):
  ready = select.select([sock], [], [], 1) #timeout 1s
  ret, frame = cap.read()
  if ready[0]:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    robotx = struct.unpack('f', data[0:4])
    roboty = struct.unpack('f', data[4:8])
    mode = struct.unpack('h', data[8:10])
    print("UDP receive")
  else:
    print("UDP not receive")
  
  if status_tracking == True:
    traject_real=np.vstack((traject_real,np.array([100+int(roboty[0]),100+int(robotx[0])])))
    lapangan = cv2.polylines(lapangan, [np.int32(traject_real)], isClosed=False, color=(0,0,255),thickness=3)  
    
  # Display the resulting frame
##  cv2.imshow('undistort', resized)
  lapangan = cv2.resize(lapangan,(700,500))
  cv2.imshow('lapangan', lapangan)
  cv2.imshow('frame', frame)
##  cv2.imshow('mask', mask)
  
  c = cv2.waitKey(10)
  if c == ord('p'):
    status_tracking = not status_tracking
  if c == 27:
    break

# After the loop release the cap object
sock.close()
cap.release()
cv2.destroyAllWindows()

