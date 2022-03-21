# opencvtes
This is my first work on computer vision. The program is designed to complete the calibration of the camera, then identify objects captured in the video, 
measure distances and predict their possible motion states. Among them, the code of the recognition part performs simple binarization processing on the image, and filters 
the detected contours according to the aspect ratio and area of the contours. The code of the prediction part uses the PNP algorithm, which aims to determine the world 
coordinate system of the object and the camera coordinate system which is used to obtain external parameters, so as to solve the pose of the camera and realize ranging. 
The code in the prediction part uses Kalman filtering, assuming that the object maintains a relatively constant speed (ie, no significant acceleration), the Kalman gain 
is obtained by the predicted value and the measured value, so as to realize the prediction of the motion state of the object at the next moment.
Notice! ! ! The accuracy of the distance measured by this code depends on whether the object's world coordinate system is accurately defined! ! ! This is also the 
difficulty in realizing multi-target ranging, because the real size of different objects is always different.
There are still many imperfections in this code. Interested friends are welcome to give me valuable opinions. If there is something you don't understand, please 
leave a comment or write to 2165990891@qq.com. Thanks.
