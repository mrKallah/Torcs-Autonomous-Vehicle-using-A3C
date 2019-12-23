from pro import process
import numpy as np
import cv2


img = cv2.imread("torcs.png")
img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
img = np.asarray(img / 255, dtype=np.float32)

img_pro = process(img)

img_pro = img_pro * 255
print(img_pro)
cv2.imwrite("torcs_pro.png", img_pro)
