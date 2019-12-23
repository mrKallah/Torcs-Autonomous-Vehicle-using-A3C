import matplotlib.pyplot as plt
import time
import cv2
import matplotlib
import numpy as np

class plt_video_frame:
    def __init__(self, port):
        '''
        creates the plot window.
        '''
        matplotlib.use("TkAgg")
        self.fig = plt.figure(figsize=(3.25, 2.75))
        self.port = port


    def refresh_plot(self, img):
        '''
        Refreshes the plot with an image.
        '''

        if img.shape[0] != 240:
            img = cv2.resize(img, (320, 240))

        self.fig.clf()
        self.fig.canvas.flush_events()

        self.fig.canvas.set_window_title("{}".format(self.port))

        self.fig.figimage(img)

        self.fig.suptitle("What the model sees")

        self.fig.canvas.draw()
        self.fig.show()

        self.fig.canvas.flush_events()



if __name__ == "__main__":
    img = cv2.imread("test.jpeg")
    img2 = cv2.imread("test.png")

    img = np.asarray(img, dtype=np.float32)
    img2 = np.asarray(img2, dtype=np.float32)

    img = cv2.resize(img, (240, 320)) / 255
    img2 = cv2.resize(img2, (240, 320)) / 255

    v1 = plt_video_frame(0)
    v2 = plt_video_frame(1)

    print("init done")
    time.sleep(2)

    v1.refresh_plot(img)
    print("plot 1")
    time.sleep(2)

    v2.refresh_plot(img2)
    print("plot 2")
    time.sleep(2)

    while True:
        v1.refresh_plot(img)
        v2.refresh_plot(img2)
        img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
        img2 = cv2.rotate(img2, cv2.ROTATE_90_COUNTERCLOCKWISE)

        v1.refresh_plot(img)
        v2.refresh_plot(img2)
        img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
        img2 = cv2.rotate(img2, cv2.ROTATE_90_COUNTERCLOCKWISE)

        v1.refresh_plot(img)
        v2.refresh_plot(img2)
        img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
        img2 = cv2.rotate(img2, cv2.ROTATE_90_COUNTERCLOCKWISE)

        v1.refresh_plot(img)
        v2.refresh_plot(img2)
        img = cv2.rotate(img, cv2.ROTATE_90_CLOCKWISE)
        img2 = cv2.rotate(img2, cv2.ROTATE_90_COUNTERCLOCKWISE)

