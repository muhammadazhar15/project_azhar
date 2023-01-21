import cv2
import numpy as np

bgr_map = cv2.imread("bgrMap.jpg")
gray_map = cv2.cvtColor(bgr_map,cv2.COLOR_BGR2GRAY)
ret, bin_map = cv2.threshold(gray_map, 200, 255, cv2.THRESH_BINARY)

kernel = np.ones((2,2), np.uint8)
# bin_map = cv2.dilate(bin_map, kernel, iterations=5)
bin_map = (255-bin_map)

dist = cv2.distanceTransform(bin_map, distanceType=cv2.DIST_L2, maskSize=3, dstType=cv2.CV_8U)
dist2 = cv2.normalize(dist, None, alpha=0, beta=255, norm_type=cv2.NORM_MINMAX, dtype=cv2.CV_8U)
##print(dist2.shape)
##dist2 = 255-dist2
data_bin = dist2.flatten()
print(data_bin.dtype)
data_bin_dob = data_bin.astype(np.double)
print(data_bin_dob.dtype)

file=open("error_bgrMap","wb")
array=bytearray(data_bin_dob)
file.write(array)
file.close()

##open_file=open("error.bin","rb")
##number=list(open_file.read())
##print(len(number))
##open_file.close()
##
##open_file=open("errortable_lama.bin","rb")
##number=list(open_file.read(8))
##print(number)
##open_file.close()

cv2.imshow('BGR MAP', bgr_map)
cv2.imshow('Gray MAP', gray_map)
cv2.imshow('BIN MAP', bin_map)
cv2.imshow('dist MAP', dist)
cv2.imshow('dist2 MAP', dist2)


if cv2.waitKey(0) & 0xff == 27:
    cv2.destroyAllWindows()
