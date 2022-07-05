# RaspberryPi_Projects

# Pre-trained models
https://github.com/kiyoshiiriemon/yolov4_darknet#pre-trained-models

# Compile low level code
```sh 
    g++ low_level/detect_alarm.cpp -o low_level/detect `pkg-config --cflags --libs opencv4` 
```