import configparser
import ast

config = configparser.ConfigParser(inline_comment_prefixes=";")
config.read('torcs.ini')

GAMMA = float(config["model"]["gamma"])
UTIL_LR = float(config["model"]["util_learning_rate"])
learning_rate = float(config["model"]["learning_rate"])
eps = float(config["model"]["eps"])
betas = ast.literal_eval(config["model"]["betas"])
UPDATE_GLOBAL_ITER = int(config["model"]["update_global_iter"])
MAX_EP = int(config["model"]["max_ep"])
MAX_EP_STEP = int(config["model"]["max_ep_step"])

greyscale = config["general"]["cvt_to_black_and_white"].lower() == "true"
do_preprocess = config["general"]["do_preprocess"].lower() == "true"

do_resize = bool(config["torcs"]["do_resize"])

# this is the height and width for the models training and has nothing to do with the resizing from torcs
height = 224
width = 224

