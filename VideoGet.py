from threading import Thread
from imageai import Detection
import cv2

class VideoGet:
    """
    Class that continuously gets frames from a VideoCapture object
    with a dedicated thread.
    """

    def __init__(self, yolo, cam):
        self.stream = cam
        (self.grabbed, self.frame) = self.stream.read()
        self.yolo = yolo
        self.stopped = False

    def start(self):
        Thread(target=self.get, args=()).start()
        return self

    def get(self):
        while not self.stopped:
            if not self.grabbed:
                self.stop()
            else:
                (self.grabbed, img) = self.stream.read()

                #print("before")
                (self.frame, preds) = self.yolo.detectObjectsFromImage(input_image=img, 
                      #custom_objects=None, 
                      input_type="array",
                      output_type="array",
                      minimum_percentage_probability=70,
                      display_percentage_probability=False,
                      display_object_name=True)    
                #print("after")                             

    def stop(self):
        self.stopped = True