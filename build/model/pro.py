import cv2
import matplotlib.pyplot as plt
import numpy as np
import time

blur_size = 5
kernel = np.ones((blur_size, blur_size), np.float32) / (blur_size * blur_size)

def plot_image(image, name="", plt_show=True):
    '''
	Plots one image
	:param image: the image to plot
	:param img_shape: the shape of the image
	:param plt_show: debug parameter, if false it wont plot the image
	:param number of color channels, 1 for greyscale and 3 for color
	:param name: title of image default: ""
	'''

    img_shape = np.asarray(image).shape

    fig = plt.figure()
    fig.canvas.set_window_title(name)
    plt.title(name)
    if len(img_shape) == 4:
        cpy = image.reshape((img_shape[0], img_shape[1], 4))
        cv2.imwrite("output/cpy.png", cpy)
        cpy = cv2.imread("output/cpy.png")
        cpy = cv2.cvtColor(cpy, cv2.COLOR_BGR2RGB)
        plt.imshow(cpy, interpolation='nearest')
    elif len(img_shape) == 3:
        cpy = image.reshape((img_shape[0], img_shape[1], 3))
        cpy = cv2.cvtColor(cpy, cv2.COLOR_BGR2RGB)
        cv2.imwrite("output/cpy.png", cpy)
        cpy = cv2.imread("output/cpy.png")
        plt.imshow(cpy, interpolation='nearest')
    else:
        cv2.imwrite("output/cpy.png", image)
        cpy = cv2.imread("output/cpy.png")
        plt.imshow(cpy, interpolation='nearest', cmap='gray')

    if plt_show == True:
        plt.show()


def rm_green(img, height, width, r_threshold=0.1, g_threshold=0.1, b_threshold=0.1):
    """
	Makes the any pixel within a color range white and any pixel above that pixel white.
	Can be used for removing redundant information if you have a color boundary.
	Example: If you have an image:
	OOOOOOOOOOO
	#O#########
	O#OO###OO##
	OOOOOOOOOOO
	OOOOOOOOOOO

	where
	# represents within threshold
	O represents any color
	@ represents white

	then the output image would be
	@@@@@@@@@@@
	@O@@@@@@@@@
	O@OO@@@OO@@
	OOOOOOOOOOO
	OOOOOOOOOOO


	:param img: the image to process
	:param height: the height of the input image
	:param width: the width of the input image
	:param r_threshold: the threshold for the red color range
	:param g_threshold: the threshold for the green color range
	:param b_threshold: the threshold for the blue color range
	:return: the processed image and the lowest non white pixel
	"""

    # create a copy of image to avoid editing the original image
    green = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    bottom = height
    for hor in range(width):
        found = False
        for vert in range(height):
            # reverses the range, if range(0,3) then hori and verti start at 3 and stop at 0
            hori = width - hor - 1
            verti = height - vert - 1

            # if a pixel within the threshold boundary has been found, then make the whole column white above said pixel
            if found:
                green[verti][hori] = 1
            else:
                # if the pixel is within the threshold boundary then set found to true and
                # change the pixel at that point to be white
                if (green[verti][hori][0] < r_threshold) and (green[verti][hori][1] > g_threshold) and (green[verti][hori][2] < b_threshold):
                    found = True
                    green[verti][hori] = 1
                    if verti < bottom:
                        bottom = verti
    return green


def rm_line(img, height, width):
    """
	Looks at the bottom line of the image and if a pixel is white, then make all pixels above it black
	:param img: the image to process
	:param height: the height of the input image
	:param width: the width of the input image
	:return: the processed image
	"""

    # create a copy of image to avoid editing the original image
    line = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
    x = height - 1
    for i in range(width):
        # if the pixel at the bottom is white then remove any pixels above it
        if (line[x][i][0] == 255) & (line[x][i][0] == 255) & (line[x][i][0] == 255):
            cv2.line(line, (i, 0), (i, height), (0, 0, 0), 1)

    return line


def process(img, greyscale, height, width):

    if greyscale == 3:
        greyscale = False

    # set height and width
    strength = 0.2

    if not greyscale:
        img[np.where((img < [strength, strength, strength]).all(axis=2))] = [0, 1, 0]

    if greyscale:
        # removes the green top of the image
        img = rm_green_bw(img, height, width)
    else:
        # removes the green top of the image
        img = rm_green(img, height, width)

    return img


def rm_green_bw(img, height, width, threshold=0.28):

    # go through from side to side
    for w in range(0, width):
        found = False
        # go though from bottom to top
        for h in range(0, height):
            h = height - h - 1

            # if image pixel is darker than threshold then remove all pixels above
            if img[h][w] <= threshold:
                found = True
            if found:
                img[h][w] = 1
    return img

def plot_process(imgs):
    names = []
    names.append("Original image")
    names.append("Highlight edge")
    names.append("Blurred edge")
    names.append("Remove above edge")

    for i in range(len(imgs)):
        plot_image(imgs[i], names[i])


def plot_one(img, name):
    # plotting
    plot_image(img, name=name)


def get_images(path):
    import glob
    images = []
    for filename in glob.glob(path):  # assuming gif
        image = cv2.imread(filename)
        images.append(image)
    return images


def main():
    # read image from file
    images = get_images('input/*.png')

    for img in images:
        img = cv2.resize(img, (200, 200))
        imgs = process(img)
        plot_process(imgs)
    # plot_image(imgs[-1])


if __name__ == '__main__':
    main()
