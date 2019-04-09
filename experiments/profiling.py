import sys
import os

algs = ["homo1","homo2","heter","mbfs","msssp"]

if len(sys.argv) <= 1:
	print("Error: Please enter the folder path!")
data_path = sys.argv[1]
data_name = data_path.split('/')[-1]
output = open(data_name + ".prof","w")

def write_table(lst,head):
	output.write(head + "\n")
	output.write("\t\t")
	for alg in algs:
		output.write(alg + "\t\t")
	output.write("\n")
	output.write("Krill\t")
	for alg in algs:
		output.write(lst['k'][alg] + "\t\t")
	output.write("\n")
	output.write("Ligra-S\t")
	for alg in algs:
		output.write(lst['s'][alg] + "\t\t")
	output.write("\n")
	output.write("Ligra-P\t")
	for alg in algs:
		output.write(lst['p'][alg] + "\t\t")
	output.write("\n\n")

print("Extracting profiling results from {} ...".format(data_path))

realtime = {'s':{},'p':{},'k':{}}
peakmem = {'s':{},'p':{},'k':{}}
llc_load = {'s':{},'p':{},'k':{}}
llc_miss = {'s':{},'p':{},'k':{}}
instructions = {'s':{},'p':{},'k':{}}
output.write("Profiling results of " + data_name + "\n")
for filename in os.listdir(data_path):
	if ".time" in filename:
		with open(data_path + "/" + filename,"r") as file:
			for line in file:
				name = filename.split('.')[0]
				if "wall clock" in line: # not user time!
					realtime[name[-1]][name[:-1]] = line.split()[-1]
				elif "Maximum resident set size" in line:
					peakmem[name[-1]][name[:-1]] = line.split()[-1]
	elif ".perf" in filename:
		with open(data_path + "/" + filename,"r") as file:
			for line in file:
				name = filename.split('.')[0]
				if "LLC-loads" in line:
					llc_load[name[-1]][name[:-1]] = line.lstrip().split()[0]
				elif "LLC-load-misses" in line:
					llc_miss[name[-1]][name[:-1]] = line.split("#")[1].lstrip().split()[0]
				elif "instructions" in line:
					instructions[name[-1]][name[:-1]] = line.lstrip().split()[0]
write_table(realtime,"Real time / Wall clock time (s)")
write_table(peakmem,"Peak memory (KB)")
write_table(llc_load,"Memory access")
write_table(llc_miss,"LLC Hits rate")
write_table(instructions,"# of Instructions")

print("Finished output!")