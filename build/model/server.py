import socket
from util import check_exit, encrypt, decrypt
import ctypes
import numpy as np
from pro import process
from video_frame import refresh_plot
import cv2
import time
import matplotlib.pyplot as plt

from __init__ import height
from __init__ import width


_connected = False


def get_image_from_buffer(port):
    com_file = "/tmp/is{}ready".format(port)
    fifo_pipe = '/tmp/{}.fifo'.format(port)

    # open the communication file and tell torcs the model is ready to recieve an image
    f = open(com_file, 'w')
    f.write("1")
    f.close()

    # check if fifo file exists
    read = False
    while not read:
        try:
            # read the fifo file as a binary file
            with open(fifo_pipe, 'rb') as fo:
                message = fo.read()
                read = True
                fo.close()
        except IOError:
            read = False

    # convert the binaries to a numpy array
    nparr = np.frombuffer(message, np.uint8)

    # set the height width and colorspace to 0 to iterate from
    height = 0
    width = 0
    colorspace = 0

    # find how many bits too search for the total height width and colorspace
    height_bits = nparr[0]
    width_bits = nparr[1]
    colorspace_bits = nparr[2]

    # accumulate the height width and colorspace from the allocated bytes (rename bit to byte as they are bytes)
    for i in range(height_bits):
        height += nparr[3 + i]

    for i in range(width_bits):
        width += nparr[height_bits + 3 + i]

    for i in range(colorspace_bits):
        colorspace += nparr[height_bits + width_bits + 3 + i]

    # remove the hwc encoding and the \0 from the array
    nparr = np.copy(nparr[height_bits + width_bits + colorspace_bits + 3:-1])

    reward = nparr[0]
    collision = bool(nparr[1])
    nparr[0] = 0
    nparr[1] = 0

    # reshape the image to the appropriate size
    img = np.reshape(nparr, (height, width, colorspace)).astype(np.float32)/255

    # rotate and flip the image as it is stored upside down and flipped
    img = cv2.rotate(img, cv2.ROTATE_180)
    img = cv2.flip(img, 1)

    img = process(img, colorspace, height, width)

    refresh_plot(img, "{}".format(port), colorspace)

    return img, reward, collision



def drive_car(action, reset, PORT, _break=0, gear=1, clutch=0):
    f = open("instructions/{}.ini".format(PORT), "w")
    f.write("[steer]\n"
            "accel={}\n"
            "steer={}\n"
            "brake={}\n"
            "gear={}\n"
            "clutch={}\n"
            "restart={}\n"
            .format(speed, action-1, _break, gear, clutch, reset))
    f.close()


def step(action, PORT, name, greyscale):
    img, reward, collision = get_image_from_buffer(PORT) # recieve_data(action, 0, PORT, name, greyscale)
    print("reward = {}".format(reward))
    print("collision = {}".format(collision))
    drive_car(action, 0, PORT)
    return (img, reward, collision)



def reset(PORT, name, greyscale):
    drive_car(0, 1, PORT)

    img, reward, collision = get_image_from_buffer(PORT) # recieve_data(0, 0, PORT, name, greyscale)
    print("reward = {}".format(reward))
    print("collision = {}".format(collision))
    return (img)

def init(port):
    refresh_plot([[0]], "{}".format(port), False)

def normallize(x, min, max):
    return (x - min) / (max - min)

speed = 0.05
