import cv2
import threading
import time

from VideoGet import VideoGet
from imageai import Detection
from numpy import expand_dims
from keras.preprocessing.image import load_img
from keras.preprocessing.image import img_to_array
from PIL import Image

# ---------------------------
# Functions
# ---------------------------
def ThreadedDetection(cv2, cam) :
    ## read frames
    ret, img = cam.read()
    ## predict yolo
    img, preds = yolo.detectCustomObjectsFromImage(input_image=img, 
                      custom_objects=None, input_type="array",
                      output_type="array",
                      minimum_percentage_probability=70,
                      display_percentage_probability=False,
                      display_object_name=True)
    ## display predictions
    cv2.imshow("", img)


def load_image_pixels(filename, shape):
    # load the image to get its shape
    image = load_img(filename)
    width, height = image.size
    # load the image with the required size
    image = load_img(filename, target_size=shape)
    # convert to numpy array
    image = img_to_array(image)
    # scale pixel values to [0, 1]
    image = image.astype('float32')
    image /= 255.0
    # # add a dimension so that we have one sample
    # image = expand_dims(image, 0)

    return image, width, height
# ---------------------------
# Main
# ---------------------------
modelpath = "./models/yolo.h5"
#modelpath = "./models/yolo-tiny.h5"
print(modelpath)

yolo = Detection.ObjectDetection()
yolo.setModelTypeAsYOLOv3()
#yolo.setModelTypeAsTinyYOLOv3()
yolo.setModelPath(modelpath)
yolo.loadModel()

# cam = cv2.VideoCapture(0) #0=front-cam, 1=back-cam
# cam.set(cv2.CAP_PROP_BUFFERSIZE, 2)
# cam.set(cv2.CAP_PROP_FRAME_WIDTH, 1280)
# cam.set(cv2.CAP_PROP_FRAME_HEIGHT, 720)

# video_getter = VideoGet(yolo, cam).start()
# x = threading.Thread(target=ThreadedDetection, args=(cv2, cam))
# x.start()

#while True:
    
## read frames
#ret, img = cam.read()
    # ## predict yolo
    # img, preds = yolo.detectCustomObjectsFromImage(input_image=img, 
    #                   custom_objects=None, input_type="array",
    #                   output_type="array",
    #                   minimum_percentage_probability=70,
    #                   display_percentage_probability=False,
    #                   display_object_name=True)
    # ## display predictions
    
    ## press q or Esc to quit        

# if (cv2.waitKey(1) & 0xFF == ord("q")) or (cv2.waitKey(1)==27) or video_getter.stopped:
#     video_getter.stop()
#     break

#img = video_getter.frame
# img = open('./WIN_20211122_19_49_30_Pro.jpg', 'rb')

#image = load_img('WIN_20211122_19_49_30_Pro.jpg')

#image, image_w, image_h = load_image_pixels('Profesional_Developer_Nina2.jpg', (720, 720))
#img = img_to_array(image)
#img = cv2.imread('./Profesional_Developer_Nina2.jpg')
img = cv2.imread('./dead_snake_cropped.jpg')

image, preds = yolo.detectCustomObjectsFromImage(input_image=img, 
                      custom_objects=None, input_type="array",
                      output_type="array",
                      minimum_percentage_probability=70,
                      display_percentage_probability=False,
                      display_object_name=True)

cv2.imshow("image", image)

# Using cv2.imwrite() method
# Saving the image
cv2.imwrite('./detection2.jpg', image)
# ## close camera
cv2.waitKey(0)

# time.sleep(5)
#cam.release()
#cv2.destroyAllWindows()