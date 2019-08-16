import sys
import os

algs = ["homo1","homo2","heter","mbfs","msssp"]

if len(sys.argv) <= 1:
	print("Error: Please enter the folder path!")
data_path = sys.argv[1]
data_name = data_path.split('/')[-1]
output = open(data_name + ".prof","w")

def write_framework(lst,name,abbr,inalgs=algs):
	output.write(name + "\t")
	try:
		for alg in inalgs:
			output.write(str(lst[abbr][alg]) + "\t\t")
	except:
		pass
	output.write("\n")

def write_table(lst,head,inalgs=algs):
	output.write(head + "\n")
	output.write("\t\t")
	for alg in inalgs:
		output.write(str(alg) + "\t\t")
	output.write("\n")
	write_framework(lst,"Krill",'k',inalgs)
	write_framework(lst,"Ligra-S",'s',inalgs)
	write_framework(lst,"Ligra-P",'p',inalgs)
	write_framework(lst,"Krill (w/o kerf)",'f',inalgs)
	write_framework(lst,"Krill (w/o opt)",'t',inalgs)
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

if len(sys.argv) < 3:
	for filename in os.listdir(data_path):
		if filename[:5] in algs:
			alg = filename[:5]
		elif filename[:4] in algs:
			alg = filename[:4] # bfs
		else:
			continue
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
					if "L1-dcache-loads" in line:
						llc_load[name[-1]][alg] = line.lstrip().split()[0]
					elif "L1-dcache-load-misses" in line:
						llc_miss[name[-1]][alg] = line.split("#")[1].lstrip().split()[0]
					elif "instructions" in line:
						instructions[name[-1]][alg] = line.lstrip().split()[0]
	write_table(realtime,"Real time / Wall clock time (s)")
	write_table(peakmem,"Peak memory (KB)")
	write_table(llc_load,"Memory access")
	write_table(llc_miss,"L1 Data Cache Hits rate")
	write_table(instructions,"# of Instructions")
else:
	for filename in os.listdir(data_path):
		if "multibfs" in filename:
			name = filename[8]
			num = int(filename.split(".time")[0][9:])
			with open(data_path + "/" + filename,"r") as file:
				for line in file:
					if "wall clock" in line:
						tmp = line.split()[-1]
						realtime[name][num] = "{:.2f}".format(int(tmp.split(':')[0]) * 60 + float(tmp.split(':')[1]))
	write_table(realtime,"Real time (s)",[1,2,4,8,16,32,64,128])

print("Finished output!")