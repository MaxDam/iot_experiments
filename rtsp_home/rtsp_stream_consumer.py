import cv2
cap = cv2.VideoCapture("rtsp://admincamera:adminpwd@192.168.1.7:554/stream1")

while(cap.isOpened()):
    ret, frame = cap.read()
    frame = cv2.resize(frame, (800,420), cv2.INTER_AREA)
    cv2.imshow('frame', frame)
    if cv2.waitKey(20) & 0xFF == ord('q'):
        break
cap.release()
cv2.destroyAllWindows()