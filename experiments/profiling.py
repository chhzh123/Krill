import sys
import os

algs = ["homo1","homo2","heter","mbfs","msssp"]

if len(sys.argv) <= 1:
	print("Error: Please enter the folder path!")
data_path = sys.argv[1]
data_name = data_path.split('/')[-1]
output = open(data_name + ".prof","w")

def write_framework(lst,name,abbr):
	output.write(name + "\t")
	try:
		for alg in algs:
			output.write(lst[abbr][alg] + "\t\t")
	except:
		pass
	output.write("\n")

def write_table(lst,head):
	output.write(head + "\n")
	output.write("\t\t")
	for alg in algs:
		output.write(alg + "\t\t")
	output.write("\n")
	write_framework(lst,"Krill",'k')
	write_framework(lst,"Ligra-S",'s')
	write_framework(lst,"Ligra-P",'p')
	write_framework(lst,"Krill (w/o kerf)",'f')
	write_framework(lst,"Krill (w/o opt)",'t')
	output.write("\n")

print("Extracting profiling results from {} ...".format(data_path))

abbreviation = ['s','p','k','f','t']
realtime, peakmem, llc_load, llc_miss, instructions = {}, {}, {}, {}, {}
for abbr in abbreviation:
	realtime[abbr] = {}
	peakmem[abbr] = {}
	llc_load[abbr] = {}
	llc_miss[abbr] = {}
	instructions[abbr] = {}
output.write("Profiling results of " + data_name + "\n")
for filename in os.listdir(data_path):
	if filename[:5] in algs:
		alg = filename[:5]
	else:
		alg = filename[:4] # bfs
	if ".time" in filename:
		with open(data_path + "/" + filename,"r") as file:
			for line in file:
				name = filename.split('.')[0]
				if "wall clock" in line: # not user time!
					tmp = line.split()[-1]
					realtime[name[-1]][alg] = "{:.2f}".format(int(tmp.split(':')[0]) * 60 + float(tmp.split(':')[1]))
				elif "Maximum resident set size" in line:
					peakmem[name[-1]][alg] = line.split()[-1]
	elif ".perf" in filename:
		with open(data_path + "/" + filename,"r") as file:
			for line in file:
				name = filename.split('.')[0]
				if "LLC-loads" in line:
					llc_load[name[-1]][alg] = line.lstrip().split()[0]
				elif "LLC-load-misses" in line:
					llc_miss[name[-1]][alg] = line.split("#")[1].lstrip().split()[0]
				elif "instructions" in line:
					instructions[name[-1]][alg] = line.lstrip().split()[0]
write_table(realtime,"Real time / Wall clock time (s)")
write_table(peakmem,"Peak memory (KB)")
write_table(llc_load,"Memory access")
write_table(llc_miss,"LLC Hits rate")
write_table(instructions,"# of Instructions")

print("Finished output!")