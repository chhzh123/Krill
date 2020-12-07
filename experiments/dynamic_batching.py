import os
import time
import subprocess
import random
import numpy as np

KRILL_PATH = "../apps"
LIGRA_PATH = "../../ligra/apps"
GRAPHM_PATH = "../../GraphM/GridGraph-M/examples"
DATASET_PATH = "../../Dataset"

# DATA = "soc-LiveJournal1"
# Vertices = 4847571
# Edges = 68993773
# SIZE = 523
# SIZEW = 694

# DATA = "rMatGraph24"
# Vertices = 33554432
# Edges = 168000000
# SIZE = 1659
# SIZEW = 2089

# DATA = "twitter7"
# Vertices = 41652231
# Edges = 1468365182
# SIZE = 12411
# SIZEW = 16108

DATA = "com-friendster"
Vertices = 124836180
Edges = 1806067135
SIZE = 16907
SIZEW = 21474

DATASET = "{}/{}".format(DATASET_PATH, DATA)
DATASETW = "{}/{}-w".format(DATASET_PATH, DATA)
DATASET_GRID = "{}/{}-grid".format(DATASET_PATH, DATA)
DATASETW_GRID = "{}/{}-w-grid".format(DATASET_PATH, DATA)
MAX_ITER = 15
CACHE_SIZE = 20 # MB
MEMORY_BUDGET = 32 # GB
PARTITION = 4

all_time = []

tot = 15 # second / unit
interval = []
for i in range(7):
    res = random.expovariate(8) * tot # poisson process
    interval.append(res)
print(interval,flush=True)

def fun_s(cmds):
    # Ligra-S
    global all_time
    start_time = time.time()
    for i, cmd in enumerate(cmds):
        p1 = subprocess.Popen(cmd, shell=True)
        if i != 7:
            sleep = interval[i]
            print("Sleep {:2f}s".format(sleep),flush=True)
            p2 = subprocess.Popen("sleep {}".format(sleep), stdout=subprocess.PIPE, shell=True)
            exit_codes = [p.wait() for p in (p1,p2)]
        else:
            p1.wait()
    end_time = time.time()
    used_time = end_time - start_time
    all_time.append(used_time)
    print("Ligra-S Time used: {:.2f}s\n".format(used_time))

def fun_p(cmds):
    # Ligra-P
    global all_time
    all_p = []
    start_time = time.time()
    for i, cmd in enumerate(cmds):
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
        all_p.append(p)
        if i != 7:
            sleep = interval[i]
            print("Sleep {:2f}s".format(sleep),flush=True)
            os.system("sleep {}".format(sleep))
    exit_codes = [p.wait() for p in all_p]
    end_time = time.time()
    used_time = end_time - start_time
    all_time.append(used_time)
    print("Ligra-P Time used: {:.2f}s\n".format(used_time),flush=True)

def fun_k(cmd):
    # Krill
    global all_time
    start_time = time.time()
    os.system(cmd)
    end_time = time.time()
    used_time = end_time - start_time
    all_time.append(used_time)
    print("Krill Time used: {:.2f}s\n".format(used_time),flush=True)

def fun_gm(cmd):
    # GraphM
    global all_time
    start_time = time.time()
    os.system(cmd)
    end_time = time.time()
    used_time = end_time - start_time
    all_time.append(used_time)
    print("GraphM Time used: {:.2f}s\n".format(used_time),flush=True)

def homo1l():
    cmd = []
    for i in range(4):
        cmd.append("./{}/BFS -r {} {}".format(LIGRA_PATH, 10*i, DATASET))
    for i in range(4):
        cmd.append("./{}/Components {}".format(LIGRA_PATH, DATASET))
    fun_s(cmd)
    fun_p(cmd)

def homo1k():
    cmd = "./{}/Homo1 {} 0".format(KRILL_PATH, DATASET)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    fun_k(cmd)

def homo1gm():
    cmd = "./{}/Homo1 {} 4 {} {} {} {}".format(GRAPHM_PATH, DATASET_GRID, MAX_ITER, CACHE_SIZE, SIZE, MEMORY_BUDGET)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    fun_gm(cmd)

def homo2l():
    cmd = []
    for i in range(4):
        cmd.append("./{}/BellmanFord -r {} {}".format(LIGRA_PATH, 71*i+2, DATASETW))
    for i in range(4):
        cmd.append("./{}/PageRankDelta -maxiters {} {}".format(LIGRA_PATH, MAX_ITER, DATASET))
    fun_s(cmd)
    fun_p(cmd)

def homo2k():
    cmd = "./{}/Homo2 {} -w 0".format(KRILL_PATH, DATASETW)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    fun_k(cmd)

def homo2gm():
    cmd = "./{}/Homo2 {} 4 {} {} {} {}".format(GRAPHM_PATH, DATASETW_GRID, MAX_ITER, CACHE_SIZE, SIZEW, MEMORY_BUDGET)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    fun_gm(cmd)

def heterl():
    cmd = []
    for i in range(2):
        cmd.append("./{}/BFS -r {} {}".format(LIGRA_PATH, 71*(i+1)+2, DATASET))
        cmd.append("./{}/Components {}".format(LIGRA_PATH, DATASET))
        cmd.append("./{}/PageRankDelta -maxiters {} {}".format(LIGRA_PATH, MAX_ITER, DATASET))
        cmd.append("./{}/BellmanFord -r {} {}".format(LIGRA_PATH, 101*(i+1)+1, DATASETW))
    fun_s(cmd)
    fun_p(cmd)

def heterk():
    cmd = "./{}/Heter {} -w 0".format(KRILL_PATH, DATASETW)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    fun_k(cmd)

def hetergm():
    cmd = "./{}/Heter {} 2 {} {} {} {}".format(GRAPHM_PATH, DATASETW_GRID, MAX_ITER, CACHE_SIZE, SIZEW, MEMORY_BUDGET)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    fun_gm(cmd)

def mbfsl():
    cmd = []
    for i in range(8):
        cmd.append("./{}/BFS -r {} {}".format(LIGRA_PATH, 91*(i+1), DATASET))
    fun_s(cmd)
    fun_p(cmd)

def mbfsk():
    cmd = "./{}/M-BFS {} 0".format(KRILL_PATH, DATASET)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    fun_k(cmd)

def mbfsgm():
    cmd = "./{}/M-BFS {} 8 {} {} {} {}".format(GRAPHM_PATH, DATASET_GRID, MAX_ITER, CACHE_SIZE, SIZE, MEMORY_BUDGET)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    fun_gm(cmd)

def mssspl():
    cmd = []
    for i in range(8):
        cmd.append("./{}/BellmanFord -r {} {}".format(LIGRA_PATH, 211*(i+1), DATASETW))
    fun_s(cmd)
    fun_p(cmd)

def mssspk():
    cmd = "./{}/M-SSSP {} -w 0".format(KRILL_PATH, DATASETW)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    fun_k(cmd)

def mssspgm():
    cmd = "./{}/M-SSSP {} 8 {} {} {} {}".format(GRAPHM_PATH, DATASETW_GRID, MAX_ITER, CACHE_SIZE, SIZEW, MEMORY_BUDGET)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    fun_gm(cmd)

def multibfsk():
    for i in range(1,10):
        cmd = "./{}/Multi-BFS {} -n {}".format(KRILL_PATH, DATASET, 2**i)
        fun_k(cmd)

heterl()
hetergm()
heterk()

homo1l()
homo1gm()
homo1k()

homo2l()
homo2gm()
homo2k()

mbfsl()
mbfsgm()
mbfsk()

mssspl()
mssspgm()
mssspk()

multibfsk()

print("Ligra-S\tLigra-P\tGraphM")
names = ["heter","homo1","homo2","mbfs","msssp"]
for i in range(5):
    out =  "{}\t".format(names[i])
    out =  "{:.2f}x\t".format(float(all_time[i*4]) / all_time[i*4+3])
    out += "{:.2f}x\t".format(float(all_time[i*4+1]) / all_time[i*4+3])
    out += "{:.2f}x".format(float(all_time[i*4+2]) / all_time[i*4+3])
    print(out)
print()