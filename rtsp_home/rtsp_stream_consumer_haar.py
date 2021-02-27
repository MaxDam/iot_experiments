import cv2 as cv

RTSP_STREAM = "rtsp://admincamera:adminpwd@192.168.1.7:554/stream1"
	
cap = cv.VideoCapture(RTSP_STREAM)

faceCascade = cv.CascadeClassifier('haarcascade_frontalface_default.xml')

def detect_face(acquiredFrame):
	#acquiredFrame = cv.resize(acquiredFrame, (800,420), cv.INTER_AREA)
	gray = cv.cvtColor(acquiredFrame, cv.COLOR_BGR2GRAY)
	faces = faceCascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30), flags=cv.CASCADE_SCALE_IMAGE)
	for (x, y, w, h) in faces:
		cv.rectangle(acquiredFrame, (x, y), (x+w, y+h), (0, 0, 255), 2)
		cv.imshow('frame', acquiredFrame)		
	
while(cap.isOpened()):
	ret, frame = cap.read()
	
	if ret:
		detect_face(frame)
	
	if cv.waitKey(20) & 0xFF == ord('q'):
		break
		
cap.release()
cv.destroyAllWindows()