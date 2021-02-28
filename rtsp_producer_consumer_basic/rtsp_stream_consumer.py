import cv2
#cap = cv2.VideoCapture("rtsp://freja.hiof.no:1935/rtplive/_definst_/hessdalen03.stream")
cap = cv2.VideoCapture("rtsp://localhost:8554/mystream")

while(cap.isOpened()):
    ret, frame = cap.read()
    cv2.imshow('frame', frame)
    if cv2.waitKey(20) & 0xFF == ord('q'):
        break
cap.release()
cv2.destroyAllWindows()