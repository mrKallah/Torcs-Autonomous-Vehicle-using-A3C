import socket
from util import check_exit, encrypt, decrypt
import ctypes
import numpy as np
import matplotlib.pyplot as plt
import sys
import csv
from pro import process


def connect(PORT):
    HOST = ""
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    # print(s.getsockname())
    s.bind((HOST, PORT))
    s.listen(1)
    # print("Waiting for connection....")
    conn, addr = s.accept()
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
    with open('drive_instructions.csv', mode='w') as file:
        writer = csv.writer(file)
        action -= 1
        print("speed = {}, turning = {}, reset = {}".format(speed, action, reset))
        writer.writerow([speed, action, reset])  # [acceleration (0 - 1), turning (-1 - 1), restart (0 = no, 1 = yes)]


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
    # print("step, action = {}".format(action))

    img = None
    while img.__class__ == None.__class__:
        img = recv_img()


    if img is not None:
        reward = int(img[0]) - 1
        collision = int(img[1])
        # print("col = {} && rew = {}".format(collision, reward))
        img = format_img(img)
        # print("drive")
        drive_car(action, 0)
        # print("return")
    else:
        raise ValueError("No image was received")



    return (img, reward, collision)


def reset():
    with open('drive_instructions.csv', mode='w') as file:
        writer = csv.writer(file)
        writer.writerow([speed, 0, 1])  # [acceleration (0 - 1), turning (-1 - 1), restart (0 = no, 1 = yes)]

    img = None
    while img.__class__ == None.__class__:
        img = recv_img()


    if img is not None:
        reward = int(img[0]) - 1
        collision = bool(img[1])
        # print("col = {} && rew = {}".format(collision, reward))
        img = format_img(img)
        # print("drive")
        drive_car(0, 1)
        # print("return")
    else:
        raise ValueError("No image was received")
    return (img)


speed = 0.05
