# Credit: Original implementation was based on https://github.com/ikostrikov/pytorch-a3c

import torch.nn as nn
import torch.nn.functional as F
import torch.multiprocessing as mp
import numpy as np
import torchvision.models as models
import torch
from torch import manual_seed
import os
import time
import subprocess
import matplotlib.pyplot as plt
from multiprocessing import set_start_method


from server import step, reset
from __init__ import UPDATE_GLOBAL_ITER, GAMMA, MAX_EP, learning_rate, eps, betas, greyscale, height, width
from utils import set_init, push_and_pull, record
from video_frame import plt_video_frame


try:
    is_cuda = torch.cuda.is_available()
except:
    is_cuda = False


is_cuda = False

manual_seed(0)

# densenet121







os.environ["OMP_NUM_THREADS"] = "1"

N_S = 25088
N_A = 3


if greyscale:
    shape = (height, width, 1)  # (height, width, 3)
    _shape = (1, height, width)  # (3, height, width)
else:
    shape = (height, width, 3) # (height, width, 3)
    _shape = (3, height, width) # (3, height, width)


shape = (height, width, 3) # (height, width, 3)
_shape = (3, height, width) # (3, height, width)


if greyscale:
    # model = models.resnet50(pretrained=False)
    # model.conv1 = nn.Conv2d(1, 64, kernel_size=7, stride=2, padding=3, bias=False)

    model = models.vgg19(pretrained=False)


else:
    model = models.vgg16(pretrained=True)



if is_cuda:
    model = model.cuda()



class SharedAdam(torch.optim.Adam):
    """
    Shared optimizer, the parameters in the optimizer will shared in the multiprocessors.
    """
    def __init__(self, params, lr=learning_rate, betas=betas, eps=eps, weight_decay=0):
        super(SharedAdam, self).__init__(params, lr=lr, betas=betas, eps=eps, weight_decay=weight_decay)
        # State initialization
        for group in self.param_groups:
            for p in group['params']:
                state = self.state[p]
                state['step'] = 0
                state['exp_avg'] = torch.zeros_like(p.data)
                state['exp_avg_sq'] = torch.zeros_like(p.data)

                # share in memory
                state['exp_avg'].share_memory_()
                state['exp_avg_sq'].share_memory_()


class Net(nn.Module):
    def __init__(self, s_dim, a_dim):

        super(Net, self).__init__()
        self.s_dim = s_dim
        self.a_dim = a_dim
        self.pi1 = nn.Linear(s_dim, 200)
        self.pi2 = nn.Linear(200, a_dim)
        self.v1 = nn.Linear(s_dim, 100)
        self.v2 = nn.Linear(100, 1)
        set_init([self.pi1, self.pi2, self.v1, self.v2])
        self.distribution = torch.distributions.Categorical

    def forward(self, x):
        if x.__class__ == np.asarray([]).__class__:
            if np.max(x) > 1:
                x = np.asarray(x, np.float32)
                x = x * 1 / 255
                x = np.resize(x, shape)
                x = np.reshape(x, _shape)
            x = torch.from_numpy(x)

        pi1 = F.relu6(self.pi1(x))
        logits = self.pi2(pi1)
        v1 = F.relu6(self.v1(x))
        values = self.v2(v1)
        return logits, values

    def choose_action(self, s):
        self.eval()
        logits, _ = self.forward(s)

        prob = F.softmax(logits).data
        m = self.distribution(prob)
        out = m.sample().numpy()
        return out

    def loss_func(self, s, a, v_t):
        self.train()
        logits, values = self.forward(s)
        td = v_t - values
        c_loss = td.pow(2)

        probs = F.softmax(logits)
        m = self.distribution(probs)
        exp_v = m.log_prob(a) * td.detach().squeeze()
        a_loss = -exp_v
        total_loss = (c_loss + a_loss).mean()
        return total_loss


class Worker(mp.Process):
    def __init__(self, gnet, opt, global_ep, global_ep_r, res_queue, name):
        super(Worker, self).__init__()
        self.name = 'w%i' % name
        self.g_ep, self.g_ep_r, self.res_queue = global_ep, global_ep_r, res_queue
        self.gnet, self.opt = gnet, opt
        # local network
        self.lnet = Net(N_S, N_A)

        f = open("ports.txt", "r")
        contents = f.read()

        self.ip, self.port = contents.split(" ")
        self.port = int(self.port)

        self.frame = plt_video_frame(self.port)

        #drive_car(0, 1, self.port)



        subprocess.Popen(("konsole", "--noclose", "-e", "torcs"))


        # Wait for 300 milliseconds so torcs can increment the port
        time.sleep(.300)
        #output, error = self.sp.communicate()


        #os.system('torcs')


    def run(self):
        total_step = 1
        while self.g_ep.value < MAX_EP:
            # restarts the game
            s = reset(self.port, self.frame)
            # updates the video frame for matplotlib
            self.frame.refresh_plot(s)

            #print("img_array", s)
            s = feature_vec(s)

            # feature_vec
            buffer_s, buffer_a, buffer_r = [], [], []
            ep_r = 0.0
            while True:

                # choosing action takes 0.003945827484130859s
                # step takes 0.50535451889038086s
                # feature_vec takes 3.1696090698242188e-05s

                # have the model make a decision
                a = self.lnet.choose_action(s)

                # drive the vehicle
                s_, r, done = step(a, self.port, self.frame)

                # updates the video frame for matplotlib
                self.frame.refresh_plot(s_)

                s_ = feature_vec(s)
                # -1 is one char longer than 1 or 0, so adding a space to 1 and 0 in order to avoid having shaky text
                if a-1 == -1:
                    print("{}, action = {}, reward = {}, episode reward = {}, restart = {}".format(self.name, a - 1, round(r, 2), round(ep_r, 2), done))
                else:
                    print("{}, action = {},  reward = {}, episode reward = {}, restart = {}".format(self.name, a - 1, round(r, 2), round(ep_r, 2), done))

                ep_r += r
                buffer_a.append(a)
                buffer_s.append(s)
                buffer_r.append(r)

                # update global and assign to local net
                if total_step % UPDATE_GLOBAL_ITER == 0 or done:  
                    # sync
                    push_and_pull(self.opt, self.lnet, self.gnet, done, s_, buffer_s, buffer_a, buffer_r, GAMMA)
                    buffer_s, buffer_a, buffer_r = [], [], []

                    if done:  # done and print information
                        record(self.g_ep, self.g_ep_r, ep_r, self.res_queue,
                               self.name)
                        break
                s = s_
                total_step += 1


        self.res_queue.put(None)


def feature_vec(img):

    if img.__class__ != np.asarray([]).__class__:
        return img
    img = np.asarray(img, np.float32)
    img = np.resize(img, shape)
    img = np.reshape(img, _shape)
    img = img * 1 / 255
    img_tensor = torch.from_numpy(img)
    img_tensor = img_tensor.unsqueeze_(0)

    if is_cuda:
        img_tensor = img_tensor.cuda()

    return model.features(img_tensor).view(-1)


if __name__ == "__main__":

    path = "instructions/"
    import shutil
    try:
        shutil.rmtree(path)
        time.sleep(.5)
    except:
        None

    os.mkdir(path)



    set_start_method('spawn')
    # global network
    gnet = Net(N_S, N_A)
    # share the global parameters in multiprocessing
    gnet.share_memory()
    opt = SharedAdam(gnet.parameters(), lr=learning_rate)      # global optimizer
    global_ep, global_ep_r, res_queue = (mp.Value('i', 0), mp.Value('d', 0.), mp.Queue())
    #worker_amount = mp.cpu_count()
    worker_amount = 1

    # parallel training
    workers = [Worker(gnet, opt, global_ep, global_ep_r, res_queue, i) for i in range(worker_amount)]

    [w.start() for w in workers]
    res = []                    # record episode reward to plot
    while True:
        r = res_queue.get()
        if r is not None:
            res.append(r)
        else:
            break
    [w.join() for w in workers]


    plt.plot(res)
    plt.axis('on')
    plt.ylabel('Moving average ep reward')
    plt.xlabel('Step')
    plt.show()
