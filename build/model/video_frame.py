import matplotlib.pyplot as plt
import time
import cv2

def update(img):
	plt.imshow(img)
	fig.canvas.draw()
	time.sleep(.05)

fig = plt.figure()
plt.axis('off')
fig.show()