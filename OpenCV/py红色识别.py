import cv2
import numpy as np

cap = cv2.VideoCapture('F:\\out.avi')

while(1):

    # ��ȡ��Ƶ��ÿһ֡
    _, frame = cap.read()

    # ��ͼƬ�� BGR �ռ�ת���� HSV �ռ�
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # ������HSV�ռ��к�ɫ�ķ�Χ
    lower_blue = np.array([156,43,46])
    upper_blue = np.array([180,255,255])

    # �������϶���ĺ�ɫ����ֵ�õ���ɫ�Ĳ���
    mask = cv2.inRange(hsv, lower_blue, upper_blue)

    res = cv2.bitwise_and(frame,frame, mask= mask)

    cv2.imshow('frame',frame)
    cv2.imshow('mask',mask)
    cv2.imshow('res',res)
    k = cv2.waitKey(5) & 0xFF
    if k == 27:
        break

cv2.destroyAllWindows()