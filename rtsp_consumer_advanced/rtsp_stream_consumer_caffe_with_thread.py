from threading import Thread
import cv2 as cv
import numpy as np
import time
import os

RTSP_STREAM = "rtsp://admincamera:adminpwd@192.168.1.7:554/stream1"

class ThreadedCamera(object):
    def __init__(self, src=0):
        self.net = cv.dnn.readNetFromCaffe("model/weights-prototxt.txt", "model/res_ssd_300Dim.caffeModel" )

        os.environ["OPENCV_FFMPEG_CAPTURE_OPTIONS"] = "rtsp_transport;udp"
        self.capture = cv.VideoCapture(src, cv.CAP_FFMPEG)
        self.capture.set(cv.CAP_PROP_BUFFERSIZE, 2)

        if not self.capture.isOpened():
            print("RTSP stream not found")
            exit(-1)

        # FPS = 1/X
        # X = desired FPS
        self.FPS = 1 / 30
        self.FPS_MS = int(self.FPS * 1000)

        # Start frame retrieval thread
        self.thread = Thread(target=self.update, args=())
        self.thread.daemon = True
        self.thread.start()

    def update(self):
        while True:
            if self.capture.isOpened():
                (self.status, self.frame) = self.capture.read()
            time.sleep(self.FPS)

    def detect_face(self):
        (height, width) = self.frame.shape[:2]
        blob = cv.dnn.blobFromImage(cv.resize(self.frame, (300, 300)), 1.0, (300, 300), (104.0, 177.0, 123.0))
        self.net.setInput(blob)
        detections = self.net.forward()
        if len(detections) > 0:
            for i in range(0, detections.shape[2]):
                confidence = detections[0, 0, i, 2]
                if confidence < 0.3:
                    continue
                box = detections[0, 0, i, 3:7] * np.array(
                    [width, height, width, height]
                )
                (x1, y1, x2, y2) = box.astype("int")
                y = y1 - 10 if y1 - 10 > 10 else y1 + 10
                # cv.rectangle(self.frame, (x1, y1), (x2, y2), (0, 255, 0), 3)
                self.drawRect(self.frame, x1, y1, x2-x1, y2-y1, 8, 3)

        # time.sleep(5)

    # disegna il rettangolo di selezione
    def drawRect(self, frame, x, y, w, h, corner_size=3, rect_size=2, corner_color=(0, 255, 0), rect_color=(200, 200, 200)):
        edge_len = int(min(w, h) / 2 * 0.45)
        cv.rectangle(frame, (x, y), (x + w, y + h), rect_color, rect_size)
        cv.line(frame, (x, y), (x, y + edge_len), corner_color, corner_size)
        cv.line(frame, (x, y), (x + edge_len, y), corner_color, corner_size)
        cv.line(frame, (x + w, y), (x + w, y + edge_len), corner_color, corner_size)
        cv.line(frame, (x + w, y), (x + w - edge_len, y), corner_color, corner_size)
        cv.line(frame, (x, y + h), (x, y + h - edge_len), corner_color, corner_size)
        cv.line(frame, (x, y + h), (x + edge_len, y + h), corner_color, corner_size)
        cv.line(frame, (x + w, y + h), (x + w - edge_len, y + h), corner_color, corner_size)
        cv.line(frame, (x + w, y + h), (x + w, y + h - edge_len), corner_color, corner_size)

    def show_frame(self):
        if self.status:
            self.detect_face()
            frm = cv.resize(self.frame, (800, 420), cv.INTER_AREA)
            cv.imshow("frame", frm)
        cv.waitKey(self.FPS_MS)

    def end(self):
        self.capture.release()
        cv.destroyAllWindows()


if __name__ == "__main__":
    tcamera = ThreadedCamera(RTSP_STREAM)

    loop = True
    while loop:
        try:
            tcamera.show_frame()
            if cv.waitKey(1) == ord("q"):
                print("break")
                loop = False
                break;
        except AttributeError:
            pass

    tcamera.end()
