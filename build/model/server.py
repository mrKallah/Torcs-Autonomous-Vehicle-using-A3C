import numpy as np
from pro import process
import cv2
import time

from __init__ import do_preprocess, greyscale


_connected = False


def get_image_from_buffer(port, frame):
    com_file = "/tmp/is{}ready".format(port)
    fifo_pipe = '/tmp/{}.fifo'.format(port)

    # open the communication file and tell torcs the model is ready to receive an image
    f = open(com_file, 'w')
    if greyscale:
        f.write("1")
    else:
        f.write("2")
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



    # remove the hwc encoding and the \0 from the array

    nparr = np.copy(nparr[height_bits + width_bits + colorspace_bits + reward_bits + collision_bits + startbits:-1])

    # reward = nparr[0]
    # collision = bool(nparr[1])
    # nparr[0] = 0
    # nparr[1] = 0

    # reshape the image to the appropriate size
    if colorspace == 1:
        img = np.reshape(nparr, (height, width)).astype(np.float32)/255
    else:
        img = np.reshape(nparr, (height, width, colorspace)).astype(np.float32) / 255

    # rotate and flip the image as it is stored upside down and flipped
    # img = cv2.rotate(img, cv2.ROTATE_180)
    # img = cv2.flip(img, 1)

    t1 = time.time()

    if do_preprocess:
        img = process(img)

    if colorspace == 1:
        img = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)

    t2 = time.time()

    # print("time = {}s, do_preprocess = {}, colorspace = {}".format(t2 - t1, do_preprocess, colorspace))

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


def step(action, PORT, frame):
    img, reward, collision = get_image_from_buffer(PORT, frame)
    drive_car(action, 0, PORT)
    return (img, reward, collision)



def reset(PORT, frame):
    drive_car(0, 1, PORT)

    img, reward, collision = get_image_from_buffer(PORT, frame)
    return (img)


def normallize(x, min, max):
    return (x - min) / (max - min)

speed = 0.05
