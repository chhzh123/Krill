import os
import time
import subprocess
import random

KRILL_PATH = "../apps"
LIGRA_PATH = "../../ligra/apps"
GRAPHM_PATH = "../../GraphM/GridGraph-M/examples"
DATASET_PATH = "../../Dataset"
DATA = "rMatGraph24"
Vertices = 33554432
Edges = 168000000
SIZE = 1659
SIZEW = 2089
DATASET = "{}/{}".format(DATASET_PATH, DATA)

tot = 10 # second
interval = []
for i in range(7):
    res = random.expovariate(8) * tot # poisson process
    interval.append(res)
print(interval)

def fun_s():
    # Ligra-S
    start_time = time.time()
    for i in range(4):
        # job_start = time.time()
        p1 = subprocess.Popen("./{}/BFS -r {} {}".format(LIGRA_PATH, 10*i, DATASET), shell=True)
        # print("Job used: {:.2f}".format(time.time() - job_start))
        sleep = interval[i]
        print("Sleep {:2f}s".format(sleep))
        p2 = subprocess.Popen("sleep {}".format(sleep), shell=True)
        exit_codes = [p.wait() for p in (p1,p2)]
    for i in range(4):
        # job_start = time.time()
        p1 = subprocess.Popen("./{}/Components {}".format(LIGRA_PATH, DATASET), shell=True)
        # print("Job used: {:.2f}".format(time.time() - job_start))
        if i != 3:
            sleep = interval[i+4]
            print("Sleep {:2f}s".format(sleep))
            p2 = subprocess.Popen("sleep {}".format(sleep), shell=True)
            exit_codes = [p.wait() for p in (p1,p2)]
        else:
            p1.wait()
    end_time = time.time()
    print("Ligra-S Time used: {:.2f}s\n".format(end_time - start_time))

def fun_p():
    # Ligra-P
    all_p = []
    start_time = time.time()
    for i in range(4):
        p = subprocess.Popen("./{}/BFS -r {} {}".format(LIGRA_PATH, 10*i, DATASET), shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
        all_p.append(p)
        sleep = interval[i]
        print("Sleep {:2f}s".format(sleep))
        os.system("sleep {}".format(sleep))
    for i in range(4):
        p = subprocess.Popen("./{}/Components {}".format(LIGRA_PATH, DATASET), shell=True, cwd=os.path.dirname(os.path.realpath(__file__)))
        all_p.append(p)
        if i != 3:
            sleep = interval[i+4]
            print("Sleep {:2f}s".format(sleep))
            os.system("sleep {}".format(sleep))
    exit_codes = [p.wait() for p in all_p]
    end_time = time.time()
    print("Ligra-P Time used: {:.2f}s\n".format(end_time - start_time))

def fun_k():
    cmd = "./{}/Homo1 {} 0".format(KRILL_PATH, DATASET)
    t = 0
    for int_time in interval:
        t += int_time
        cmd += " {}".format(t)
    start_time = time.time()
    os.system(cmd)
    end_time = time.time()
    print("Krill Time used: {:.2f}s\n".format(end_time - start_time))

fun_s()
fun_p()
fun_k()