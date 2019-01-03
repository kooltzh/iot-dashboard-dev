# This script is used to send values of sine wave using POST for testing purposes

import requests
import random

import time

import matplotlib.pyplot as plt
import numpy as np 

#generating a period 5s 0.2Hz sine wave, sampling on 10Hz
#the sample data is 5s long and have 50 point

Fs = 20
f = 0.5
sampling = 80
x = np.arange(sampling)
y = np.sin(2*np.pi*f*x/Fs) + np.sin(2*np.pi*(f+0.25)*x/Fs)
# plt.plot(x,y)
# plt.show()

URL = 'http://127.0.0.1:5000/api/post'

PERIOD = 1/Fs

count = 1

def calling_func():
    global count
    numb = y[count]
    data = {'maindata' : numb}
    try:
        r = requests.post(url = URL, data = data)
    except:
        print ("failed to connect to {}".format(URL))
        exit()
    # if (r.text == "success"):
    #     print ('sent {} to {}'.format(data, URL))
    #     if count >= sampling - 1:
    #         count = 0
    #     else:
    #         count+=1
    # else:
    #     print ("fail")
    #     exit()
    print ('sent {} to {}'.format(data, URL))
    print (r.content)
    if count >= sampling - 1:
        count = 0
    else:
        count+=1

while True:
    try:
        startTime = time.time()
        calling_func()
        sleepTime = PERIOD - (time.time() - startTime)
        # if sleepTime < 0:
        #     print ('The program take longer time to process, exiting..')
        #     break
        # else:
        #     time.sleep(sleepTime)
        if sleepTime > 0:
            time.sleep(sleepTime)
    except:
        break








