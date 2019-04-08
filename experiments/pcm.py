import sys
import os

algs = ["bfs","pr","cc","sssp"]
res = {"bfs":[],"pr":[],"cc":[],"sssp":[]}
io = {"bfs":10,"pr":85,"cc":10,"sssp":10}

if len(sys.argv) <= 1:
	print("Error: Please enter the folder path!")
data_path = sys.argv[1]
data_name = data_path.split('/')[-1]
output = open(data_name + ".pcm","w")

print("Extracting pcm results from {} ...".format(data_path))
for filename in os.listdir(data_path):
	with open(data_path + "/" + filename,"r") as file:
		for (i,line) in enumerate(file):
			if i < 2 or len(line.split(';')) < 3:
				continue
			else:
				res[filename.split('.')[0][:-3]].append(float(line.split(';')[9])) # Memory (MB/s)
for alg in algs:
	res[alg] = res[alg][io[alg]:io[alg]+300]
# for alg in algs:
# 	output.write(alg + "\t")
# output.write("\n")
# for i in range(mintime):
# 	for alg in algs:
# 		output.write(str(res[alg][i]) + "\t")
# 	output.write("\n")
for alg in algs:
	output.write(alg + " = np.array((")
	for i in range(min(len(res[alg]),300)):
		output.write(str(res[alg][i]) + ",")
	output.write("))\n")
print("Finished output!")