import numpy as np

import cv2
import time

from __init__ import greyscale, speed


_connected = False


def get_image_from_buffer(port):
    """
    This function will tell Torcs that it is ready to recieve an image, read said image and format it in the apropriate
    manner that the model expects.
    :param port: The ID of this this worker. (used to be the port for network communication but is now the ID for the worker, consider refractoring)
    :return: returns the image in full, the reward and the collision status.
    """
    com_file = "/tmp/is{}ready".format(port)
    fifo_pipe = '/tmp/{}.fifo'.format(port)

    # open the communication file and tell torcs the model is ready to receive an image
    f = open(com_file, 'w')
    if greyscale:
        f.write("1")
    else:
        f.write("2")
    f.close()

    # wait for the fifo file to exist, then read it
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


    # convert the binaries to a numpy array as integers
    nparr = np.frombuffer(message, np.uint8)

    # set the encoded variables to 0 to iterate from
    height = 0
    width = 0
    colorspace = 0
    reward = 0
    collision = 0

    # find how many bits too search for the total height width and colorspace
    height_bits = nparr[0]
    width_bits = nparr[1]
    colorspace_bits = nparr[2]
    reward_bits = nparr[3]
    collision_bits = nparr[4]

    # startbits are the amount of bit vars that are extracted above.
    startbits = 5

    # accumulate the height width and colorspace from the allocated bytes (rename bit to byte as they are bytes)
    for i in range(height_bits):
        height += nparr[startbits + i]

    for i in range(width_bits):
        width += nparr[height_bits + startbits + i]

    for i in range(colorspace_bits):
        colorspace += nparr[height_bits + width_bits + startbits + i]

    for i in range(reward_bits):
        reward += nparr[height_bits + width_bits + colorspace_bits + startbits + i]

    for i in range(collision_bits):
        collision += nparr[height_bits + width_bits + colorspace_bits + reward_bits + startbits + i]

    # remove the encoded start bytes and the \0 end bytes from the array
    nparr = np.copy(nparr[height_bits + width_bits + colorspace_bits + reward_bits + collision_bits + startbits:-1])

    # reshape the image to the appropriate size
    if colorspace == 1:
        img = np.reshape(nparr, (height, width)).astype(np.float32)/255
    else:
        img = np.reshape(nparr, (height, width, colorspace)).astype(np.float32) / 255

    # if the image arrives as grayscale convert to rgb as the model expects rgb.
    if colorspace == 1:
        img = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)

    return img, reward, collision



def drive_car(action, reset, ID, _break=0, gear=1, clutch=0):
    """
    Drives the car.
    :param action: steering information
    :param reset: whether or not to restart the game
    :param ID: the worker ID
    :param _break: breaking value
    :param gear: gear value
    :param clutch: clutch value
    :return:
    """
    f = open("instructions/{}.ini".format(ID), "w")
    f.write("[steer]\n"
            "accel={}\n"
            "steer={}\n"
            "brake={}\n"
            "gear={}\n"
            "clutch={}\n"
            "restart={}\n"
            .format(speed, action-1, _break, gear, clutch, reset))
    f.close()


def step(action, reset, PORT):
    """

    :param action:
    :param PORT:
    :return:
    """
    drive_car(action, reset, PORT)
    img, reward, collision = get_image_from_buffer(PORT)
    return (img, reward, collision)
