import matplotlib.pyplot as plt
import time
import cv2


def refresh_plot(img, name, greyscale):
    fig.canvas.set_window_title(name)

    if greyscale:
        plt.imshow(img, cmap="gray")
    else:
        plt.imshow(img)

    plt.title("What the model sees")

    fig.canvas.draw()

    time.sleep(.01)

    fig.clear()

import matplotlib
matplotlib.use("TkAgg")
plt.rcParams["figure.figsize"] = (2.5, 4.45)

fig = plt.figure()
plt.axis('off')
fig.show()

if __name__ == "__main__":
    img = cv2.imread("test.png")

    while True:
        refresh_plot(img, "test", False)
