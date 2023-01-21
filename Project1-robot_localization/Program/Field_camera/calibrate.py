##import numpy as np
##import cv2
##import glob
##
### termination criteria
##criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
##
### prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
##objp = np.zeros((6*7,3), np.float32)
##objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)
##
### Arrays to store object points and image points from all the images.
##objpoints = [] # 3d point in real world space
##imgpoints = [] # 2d points in image plane.
##
##images = ('chessboard.jpg')
##
##img = cv2.imread(images)
##gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
##
### Find the chess board corners
##ret, corners = cv2.findChessboardCorners(gray, (7,6),None)
##print(corners)
##print("=====")
##
### If found, add object points, image points (after refining them)
##if ret == True:
##  objpoints.append(objp)
##  objpo = np.array(objpoints)
##  print(objpo.shape)
##
##  corners2 = cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
##  imgpoints.append(corners2)
##  imgpo = np.array(imgpoints)
##  print(imgpo.dtype)
##
##  # Draw and display the corners
####  img = cv2.drawChessboardCorners(img, (7,6), corners2,ret)
##  for i,coor in enumerate(corners2):
##    cv2.putText(img, str(i), (int(coor[0,0]),int(coor[0,1])),
##                cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,0,0), 1, cv2.LINE_AA)
##  cv2.imshow('img',img)
##  cv2.waitKey(0)
##
##cv2.destroyAllWindows()
##print("finish")



import numpy as np
import cv2

img = cv2.imread('kamera_atas.jpg')
gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    
objp = np.zeros((10*7,3), np.float32)
objp[:,:2] = np.mgrid[0:7,0:10].T.reshape(-1,2)
objpoints = []
objpoints.append(objp)

imgpoints = np.array([[[[47,232]],[[42,203]],[[37,171]],[[36,138]],[[37,104]],[[39,71]],[[44,42]],
                       [[71,237]],[[66,207]],[[60,173]],[[48,137]],[[59,101]],[[62,66]],[[67,35]],
                       [[98,243]],[[94,212]],[[90,175]],[[88,136]],[[88,97]],[[91,60]],[[96,28]],
                       [[130,248]],[[127,215]],[[122,178]],[[123,136]],[[124,94]],[[126,56]],[[129,22]],
                       [[167,252]],[[165,218]],[[164,179]],[[163,136]],[[163,93]],[[164,52]],[[166,18]],
                       [[206,253]],[[205,220]],[[205,180]],[[206,137]],[[206,93]],[[206,52]],[[206,17]],
                       [[244,253]],[[246,219]],[[248,180]],[[249,136]],[[250,94]],[[249,53]],[[246,19]],
                       [[280,250]],[[285,217]],[[289,179]],[[290,138]],[[290,96]],[[287,56]],[[283,22]],
                       [[312,245]],[[318,214]],[[322,177]],[[324,139]],[[324,99]],[[322,61]],[[317,29]],
                       [[340,240]],[[347,211]],[[351,176]],[[353,140]],[[353,103]],[[350,67]],[[344,35]]]],dtype='f')

ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1],None,None)

h,  w = img.shape[:2]
newcameramtx, roi=cv2.getOptimalNewCameraMatrix(mtx,dist,(w,h),1,(w,h))
# undistort
dst = cv2.undistort(img, mtx, dist, None, newcameramtx)
# crop the image
x,y,w,h = roi
dst = dst[y:y+h, x:x+w]
##cv2.imwrite('calibresult.jpg',dst)
np.save('mtx.npy', mtx)
np.save('dist.npy', dist)
np.save('newcameramtx.npy', newcameramtx)
cv2.imshow('calib',dst)
cv2.waitKey(0)
cv2.destroyAllWindows()
