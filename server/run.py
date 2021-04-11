import sys
import subprocess

alg = sys.argv[1]
dataset = sys.argv[2]
path = "../../Dataset"

if alg in ["Heter","Homo2","M-SSSP"]:
    weighted = True

if dataset == "CP":
    data = "cit-Patents"
elif dataset == "LJ":
    data = "soc-LiveJournal1"
elif dataset == "RMAT":
    data = "rMatGraph24"
elif dataset == "TW":
    data = "twitter7"
elif dataset == "FT":
    data = "com-friendster"

if weighted:
    data += "-w"

cmd = "LD_LIBRARY_PATH=$LD_LIBRARY_PATH:. ./main {} ../apps/{} {}/{}".format("-w" if weighted else "",alg,path,data)
p = subprocess.Popen(cmd, shell=True)
exit_codes = p.wait()