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

def connect(PORT, name, greyscale):

    global _connected
    if not _connected:
        refresh_plot([[0]], name, greyscale)
        print("Waiting for connection....")

    HOST = ""
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # print(s.getsockname())
    s.bind((HOST, PORT))
    s.listen(1)
    # print("Waiting for connection....")
    conn, addr = s.accept()
    _connected = True
    return s, conn, addr


def recv_msg(conn, addr, PORT):
    # print('Connection established at ip:{}, port:{}'.format(addr[0], PORT))
    # print("--" * 5)
    # print("Waiting for client message...")
    try:
        data = conn.recv(230400)
        result = (ctypes.c_ubyte * 230400).from_buffer_copy(data)
        # check_exit(data)                # <-----------
        img = np.asarray(result, dtype=int)
        # print(img.shape)
        return img
    except:
        return None


def send_msg(s):
    d = str.encode(input())
    s.sendall(encrypt(d))
    check_exit(d)


def recv_img(PORT, name, greyscale):
    # PORT = 4321  # int(input('Assign port num:  '))
    s, conn, addr = connect(PORT, name, greyscale)
    with conn:
        img = recv_msg(conn, addr, PORT)
        # print(img)
        s.close()
    return img


def format_img(img, greyscale):
    # reshape img from 1d to 2d
    img = img.reshape(240, 960)  # 0.00004

    start_time = time.time()

    RGB = []
    if greyscale: # takes 0.210s # convert from 240,960 to 240, 320 ()
        for i in range(0, 240):
            row = []
            for j in range(0, 320):
                # row.append(img[i, j * 3] * 0.299 + img[i, j * 3 + 1] * 0.587 + img[i, j * 3 + 2] + 0.114) # 0.41s
                row.append(np.dot([img[i, j*3], img[i, j * 3 + 1], img[i, j * 3 + 2]], [0.299, 0.587, 0.114])) # 0.23s
            RGB.append(row)
        img = RGB
    else: # convert from 240,960 to 240, 320 3
        for i in range(0, 240):
            row = []
            for j in range(0, 320):
                row.append([img[i, j * 3], img[i, j * 3 + 1], img[i, j * 3 + 2]])
            RGB.append(row)
        img = RGB


    # convert image to np array of type float32. Div by 255 as
    img = np.asarray(img).astype(np.float32)/255 # takes 0.0049s

    # resizes the image to the hight and width the model requires
    img = cv2.resize(img, (height, width))  # takes  0.000333s


    # image comes in with top at bottom and bottom at top
    img = np.flip(img, 0) # takes 0.000001s

    # preprocessing the image
    img = process(img, greyscale, height, width)  # takes 0.08s

    # converting image back to RGB
    if greyscale:
        img = np.stack((img,)*3, axis=-1) # takes 0.0001s
    else:
        img = np.asarray(img)

    return img



def dump(filename="Untitled", data=None):
    import pickle
    with open(filename + '.pickle', 'wb') as handle:
        pickle.dump(data, handle, protocol=pickle.HIGHEST_PROTOCOL)


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
    img, reward, collision = recieve_data(action, 0, PORT, name, greyscale)
    return (img, reward, collision)



def reset(PORT, name, greyscale):
    drive_car(0, 1, PORT)

    img, reward, collision = recieve_data(0, 0, PORT, name, greyscale)
    return (img)


def recieve_data(action, reset, PORT, name, greyscale):
    img = None
    while img.__class__ == None.__class__:
        img = recv_img(PORT, name, greyscale)

    # reward = (float(img[0]) / 100)
    reward = float(img[0])

    reward = 1-(reward/100)

    collision = bool(img[1])

    img[0] = 0
    img[1] = 0

    # formats and preprocesses the image
    img = format_img(img, greyscale)

    # writes driving instructions to file
    drive_car(action, reset, PORT)


    refresh_plot(img, name, greyscale)

    return img, reward, collision


def normallize(x, min, max):
    return (x - min) / (max - min)

speed = 0.05
