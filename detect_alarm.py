import cv2
import requests
import os
import time
from imageai import Detection
from keras.preprocessing.image import load_img
from keras.preprocessing.image import img_to_array

# Iterate through pictures
directory = "./test_images"

for file in os.listdir(directory):
    if file.endswith(".jpg") or file.endswith(".jpeg") or file.endswith(".png"):
        print(file)
        image_path = f"{directory}/{file}"
        ## Send through image detector ##

        # Load model #
        modelpath = "./models/yolo.h5"
        #modelpath = "./models/yolo-tiny.h5"
        print(modelpath)

        yolo = Detection.ObjectDetection()
        yolo.setModelTypeAsYOLOv3()
        #yolo.setModelTypeAsTinyYOLOv3()
        yolo.setModelPath(modelpath)
        yolo.loadModel()

        # Detect object
        img = cv2.imread(image_path)

        image, preds = yolo.detectCustomObjectsFromImage(input_image=img, 
                            custom_objects=None, input_type="array",
                            output_type="array",
                            minimum_percentage_probability=70,
                            display_percentage_probability=False,
                            display_object_name=True)

        print(preds)
        pred_obj = [x['name'] for x in preds]
        obj_num = len(pred_obj)
        if(obj_num > 0) :

            if(obj_num == 1) :
                msg_title = f'A {pred_obj[0]} has been detected!'
            elif (obj_num > 1) :
                msg_title = f'A {pred_obj[0]} and a {pred_obj[1]} have been detected!'


            #cv2.imshow("image", image)

            # Using cv2.imwrite() method
            # Saving the image
            result_image = f'./result_images/detection_{file}.jpg'
            cv2.imwrite(result_image, image)
            # ## close camera
            #cv2.waitKey(0)

            # Send message through pushover
            
            print(image_path)
            req = requests.post("https://api.pushover.net/1/messages.json",data={"token": "abc5kk96z5ave92xg8edp6cc89dnui","user": "ur8ix1abhx1s8wn3s4s6m8dd3trbiu","message": ("Alert: " + msg_title)},files = {"attachment": (str(file), open(result_image ,"rb"), "image/jpeg")})

            time.sleep(5)
            print(req.text)