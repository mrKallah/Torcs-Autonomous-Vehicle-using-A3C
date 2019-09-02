import torchvision.models as models
import torchvision.transforms as transforms
import numpy as np
from PIL import Image
import torch
import cv2

from helper import load_stuff

try:
    is_cuda = torch.cuda.is_available()
except:
    is_cuda = False


is_cuda = False


# densenet121
model = models.vgg16(pretrained=True)

if is_cuda:
    model = model.cuda()

def feature_vec(img):

    if img.__class__ != np.asarray([]).__class__:
        return img
    # img = np.uint8(img)
    img = np.asarray(img, np.float32)
    img = np.resize(img, (224, 224, 3))
    img = np.reshape(img, (3, 224, 224))
    # img_pil = cv2.cvtColor(img_pil, cv2.COLOR_RGB2GRAY)
    img = img * 1 / 255
    img_tensor = torch.from_numpy(img)
    img_tensor = img_tensor.unsqueeze_(0)

    if is_cuda:
        img_tensor = img_tensor.cuda()


    return model.features(img_tensor).view(-1)
