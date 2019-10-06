import matplotlib.pyplot as plt
import time
import cv2


def update(img, name, greyscale):
    fig.canvas.set_window_title(name)
    if greyscale:
        plt.imshow(img, cmap="gray")
    else:
        plt.imshow(img)
    plt.title("What the model sees")
    fig.canvas.draw()
    time.sleep(.05)


plt.rcParams["figure.figsize"] = (2.5, 4.45)

fig = plt.figure()
plt.axis('off')
fig.show()

if __name__ == "__main__":
    img = cv2.imread("test.png")

    while True:
        update(img, "test", False)
