import sys
import os

algs = ["heter","homo1","homo2","mbfs","msssp"]

if len(sys.argv) <= 1:
	print("Error: Please enter the folder path!")
	sys.exit()
data_path = sys.argv[1]
data_name = data_path.split('/')[-1]
if len(sys.argv) < 3:
	output = open(data_name + ".prof","w")
elif int(sys.argv[2]) == 1:
	output = open(data_name + "-multibfs.prof","w")
elif int(sys.argv[2]) == 2:
	output = open(data_name + "-multicore.prof","w")
else:
	output = open(data_name + "-grid.prof","w")

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
	write_framework(lst,"GraphM",'m',inalgs)
	write_framework(lst,"Krill (w/o pb)","t",inalgs)
	write_framework(lst,"Krill (w/o kerf)",'f',inalgs)
	output.write("\n")

print("Extracting profiling results from {} ...".format(data_path))

abbreviation = ['k','s','p','m','t','b','f','d','w','g']
cpus, realtime, peakmem, l1_load_cnt, l1_miss_cnt, l1_miss, llc_load_cnt, llc_miss_cnt, llc_miss, instructions = {}, {}, {}, {}, {}, {}, {}, {}, {}, {}
for abbr in abbreviation:
	cpus[abbr] = {}
	realtime[abbr] = {}
	peakmem[abbr] = {}
	l1_load_cnt[abbr] = {}
	l1_miss[abbr] = {}
	l1_miss_cnt[abbr] = {}
	llc_load_cnt[abbr] = {}
	llc_miss_cnt[abbr] = {}
	llc_miss[abbr] = {}
	instructions[abbr] = {}

output.write("Profiling results of " + data_name + "\n")

def parse(filename,alg):
	if ".time" in filename:
		with open(data_path + "/" + filename,"r") as file:
			for line in file:
				name = filename.split('.')[0]
				if "wall clock" in line: # not user time!
					tmp = line.split()[-1]
					if alg in realtime[name[-1]].keys():
						realtime[name[-1]][alg] = "{:.2f}".format(min(realtime[name[-1]][alg],float(int(tmp.split(':')[0]) * 60 + float(tmp.split(':')[1]))))
					else:
						realtime[name[-1]][alg] = float(int(tmp.split(':')[0]) * 60 + float(tmp.split(':')[1]))
				elif "Maximum resident set size" in line:
					peakmem[name[-1]][alg] = line.split()[-1]
	elif ".perf" in filename:
		with open(data_path + "/" + filename,"r") as file:
			for line in file:
				name = filename.split('.')[0]
				if "task-clock" in line or "cpu-clock" in line:
					cpus[name[-1]][alg] = float(line.split("#")[1].lstrip().split()[0])
				elif "L1-dcache-loads" in line:
					l1_load_cnt[name[-1]][alg] = format(round(float(line.lstrip().split()[0].replace(",","")) / cpus[name[-1]][alg]),",")
				elif "L1-dcache-load-misses" in line:
					l1_miss[name[-1]][alg] = line.split("#")[1].lstrip().split()[0]
					l1_miss_cnt[name[-1]][alg] = line.lstrip().split()[0]
				elif "LLC-loads" in line:
					llc_load_cnt[name[-1]][alg] = line.lstrip().split()[0]
				elif "LLC-load-misses" in line:
					llc_miss[name[-1]][alg] = line.split("#")[1].lstrip().split()[0]
					llc_miss_cnt[name[-1]][alg] = line.lstrip().split()[0]
				elif "instructions" in line:
					instructions[name[-1]][alg] = line.lstrip().split()[0]
				elif "seconds time elapsed" in line:
					if alg in realtime[name[-1]].keys():
						realtime[name[-1]][alg] = "{:.2f}".format(min(realtime[name[-1]][alg],float(line.lstrip().split()[0])))
					else:
						realtime[name[-1]][alg] = float(line.lstrip().split()[0])

if len(sys.argv) < 3:
	for filename in os.listdir(data_path):
		if filename[:5] in algs:
			alg = filename[:5]
		elif filename[:4] in algs:
			alg = filename[:4] # bfs
		else:
			continue
		parse(filename,alg)
	write_table(realtime,"Real time / Wall clock time (s)")
	write_table(peakmem,"Peak memory (KB)")
	write_table(l1_load_cnt,"L1 load") # load instruction
	write_table(l1_miss,"L1 Data Cache miss rate")
	write_table(l1_miss_cnt,"L1 Data Cache miss count")
	write_table(llc_load_cnt,"LLC load")
	write_table(llc_miss_cnt,"LLC Data Cache miss count")
	write_table(llc_miss,"LLC miss rate")
	write_table(instructions,"# of Instructions")
elif int(sys.argv[2]) == 1: # multibfs
	for filename in os.listdir(data_path):
		if "multibfs" in filename:
			name = filename[8]
			if name == "g":
				name = 'm'
				num = int(filename.split(".time")[0][10:])
			else:
				num = int(filename.split(".time")[0][9:])
			with open(data_path + "/" + filename,"r") as file:
				for line in file:
					if "wall clock" in line:
						tmp = line.split()[-1]
						realtime[name][num] = "{:.2f}".format(int(tmp.split(':')[0]) * 60 + float(tmp.split(':')[1]))
	write_table(realtime,"Real time (s)",[1,2,4,8,16,32,64,128])
elif int(sys.argv[2]) == 2: # multicore
	for filename in os.listdir(data_path):
		if "multicore" in filename:
			name = filename[9]
			if name == "g":
				name = 'm'
				num = int(filename.split(".time")[0][11:])
			else:
				num = int(filename.split(".time")[0][10:])
			with open(data_path + "/" + filename,"r") as file:
				for line in file:
					if "wall clock" in line:
						tmp = line.split()[-1]
						realtime[name][num] = "{:.2f}".format(int(tmp.split(':')[0]) * 60 + float(tmp.split(':')[1]))
	write_table(realtime,"Real time (s)",[1,2,4,8,16,32,48])
else: # preprocess
	for filename in os.listdir(data_path):
		if "evalgrid" in filename:
			parse(filename,"Preprocess")
	output.write("Preprocess" + "\t\t")
	output.write("\n")
	write_framework(realtime,"Real time (s)","d",["Preprocess"])
	write_framework(realtime,"Real time w (s)","w",["Preprocess"])
	write_framework(peakmem,"Peak memory (KB)","d",["Preprocess"])
	write_framework(peakmem,"Peak memory w (KB)","w",["Preprocess"])
	write_framework(l1_load_cnt,"L1 load","d",["Preprocess"]) # from memory to processor (L1)
	write_framework(l1_load_cnt,"L1 load w","w",["Preprocess"]) # from memory to processor (L1)
	write_framework(llc_load_cnt,"LLC load","d",["Preprocess"])
	write_framework(llc_load_cnt,"LLC load w","w",["Preprocess"])
	write_framework(l1_miss,"L1 Data Cache miss rate","d",["Preprocess"])
	write_framework(l1_miss,"L1 Data Cache miss rate w","w",["Preprocess"])
	write_framework(l1_miss_cnt,"L1 Data Cache miss count","d",["Preprocess"])
	write_framework(l1_miss_cnt,"L1 Data Cache miss count w","w",["Preprocess"])
	write_framework(llc_miss_cnt,"LLC Data Cache miss count","d",["Preprocess"])
	write_framework(llc_miss_cnt,"LLC Data Cache miss count w","w",["Preprocess"])
	write_framework(llc_miss,"LLC miss rate","d",["Preprocess"])
	write_framework(llc_miss,"LLC miss rate w","w",["Preprocess"])
	write_framework(instructions,"# of Instructions","d",["Preprocess"])
	write_framework(instructions,"# of Instructions w","w",["Preprocess"])

print("Finished output!")