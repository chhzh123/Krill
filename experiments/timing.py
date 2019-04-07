import sys
import os

algs = ["homo1","homo2","heter","mbfs","msssp"]
ligra = {'s':{},'p':{}}
if len(sys.argv) == 1:
	print("Error: Please enter the data path!")
data_path = sys.argv[1]
data_name = data_path.split('/')[-1]
output = open(data_name + ".timing","w")
print("Extracting timing results from {}...".format(data_path))
for filename in os.listdir(data_path):
	with open(data_path + "/" + filename,"r") as file:
		for line in file:
			name = filename.split('.')[0]
			ligra[name[-1]][name[:-1]] = line.split()[-1]
			break
output.write("\t")
for alg in algs:
	output.write(alg + " ")
output.write("\n")
output.write("Ligra-S\t")
for alg in algs:
	output.write(ligra['s'][alg] + " ")
output.write("\n")
output.write("Ligra-P\t")
for alg in algs:
	output.write(ligra['p'][alg] + " ")
print("Finished output!")