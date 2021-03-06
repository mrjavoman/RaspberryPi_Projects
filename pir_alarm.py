import time
import datetime
import RPi.GPIO as GPIO
import picamera
import urllib
import urllib2
import sys

#SMS settings
#Replace the xxxxxxx with the number you wish to text.
to = "xxxxxxx"
#Replace the xxxxxx with the hash given to you by smspi.co.uk
hash = "xxxxx"


def sms(to,message,hash):
    values = {
          'to' : to,
          'message' : message,
          'hash' : hash } # Grab your hash from http://www.smspi.co.uk

    url = 'http://www.smspi.co.uk/send/'

    postdata = urllib.urlencode(values)
    req = urllib2.Request(url, postdata)

    print 'Attempt to send SMS ...'

    try:
      response = urllib2.urlopen(req)
      response_url = response.geturl()
      if response_url==url:
        print response.read()
    except urllib2.URLError, e:
      print 'Send failed!'
      print e.reason
    

GPIO.setmode(GPIO.BOARD)

pir = 7

GPIO.setup(pir, GPIO.IN)
camera = picamera.PiCamera()
while True:
    if GPIO.input(pir):
        a = datetime.datetime.now()
        a = str(a)
        a = a[0:19]
        alert = ("Alarm at "+str(a))
        print(alert)
        pic = (a)+(".jpg")
        vid = (a)+(".h264")
        message = (alert),(pic),(vid)
        camera.resolution = (1024, 768)
        camera.capture(pic)
        time.sleep(1)
        camera.resolution = (640, 480)
        camera.start_recording(vid)
        camera.wait_recording(10)
        camera.stop_recording()
        #sms(to,message,hash)
        time.sleep(5)
