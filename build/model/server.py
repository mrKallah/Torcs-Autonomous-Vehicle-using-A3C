import socket
from util import check_exit, encrypt, decrypt
import ctypes
import numpy as np
import matplotlib.pyplot as plt
import sys
import csv
from pro import process
from video_frame import update

_connected = False

def connect(PORT):

    global _connected
    if not _connected:
        update([[0]])
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


def recv_img():
    PORT = 4321  # int(input('Assign port num:  '))
    s, conn, addr = connect(PORT)
    with conn:
        img = recv_msg(conn, addr, PORT)
        # print(img)
        s.close()
    return img


def format_img(img):
    img = img.reshape(240, 960)
    RGB = []
    for i in range(0, 240):
        row = []
        for j in range(0, 320):
            row.append([img[i, j * 3], img[i, j * 3 + 1], img[i, j * 3 + 2]])
        RGB.append(row)

    np.asarray(RGB)
    RGB = np.flip(RGB, 0)
    img = process(RGB)  # <- pre-processed images here
    # plt.imshow(img)
    # plt.show()
    return img


import pickle


def dump(filename="Untitled", data=None):
    with open(filename + '.pickle', 'wb') as handle:
        pickle.dump(data, handle, protocol=pickle.HIGHEST_PROTOCOL)


def drive_car(action, reset):
    f = open("instructions.ini", "w")
    _action = action - 1
    f.write("[steer]\naccel={}\nsteer={}\nrestart={}\n".format(speed, _action, reset))
    f.write("[steer]\n")
    f.write("accel={}\n".format(speed))
    f.write("steer={}\n".format(_action))
    f.write("restart={}\n".format(reset))
    f.close()

    # with open('instructions.ini', mode='w') as file:
    #     writer = csv.writer(file)
    #     _action = action - 1
    #     writer.writerow("[steer]")
    #     writer.writerow("accel={}".format(speed))
    #     writer.writerow("steer={}".format(_action))
    #     writer.writerow("restart={}".format(reset))


def main():
    while True:
        img = recv_img()
        if img is not None:
            reward = int(img[0]) - 1
            collision = int(img[1])
            # print("col = {} && rew = {}".format(collision, reward))
            img = format_img(img)
            drive_car(0, 0)


if __name__ == '__main__':
    main()


def step(action):
    img, reward, collision = recieve_data(action, 0)
    return (img, reward, collision)
1

def reset():
    f = open("instructions.ini", "w")
    f.write("[steer]\naccel={}\nsteer={}\nrestart={}\n".format(speed, 0, 1))
    f.close()

    img, reward, collision = recieve_data(0, 0)
    return (img)

def recieve_data(action, reset):
    img = None
    while img.__class__ == None.__class__:
        img = recv_img()

    if img is not None:
        # reward = (float(img[0]) / 100)
        reward = float(img[0])

        reward = 1-(reward/100)

        #reward = 1

        # if reward < -99:
        #     reward = 0

        #reward = normallize(reward, 0, 100)

        # if int(reward) != 02:
        #      reward = 1

        collision = bool(img[1])

        img[0] = 0
        img[1] = 0

        img = format_img(img)
        drive_car(action, reset)
        update(img)
    else:
        raise ValueError("No image was received")

    return img, reward, collision

def normallize(x, min, max):
    return (x - min) / (max - min)

speed = 0.05
