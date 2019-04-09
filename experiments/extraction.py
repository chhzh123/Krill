import sys
import os

algs = ["homo1","homo2","heter","mbfs","msssp"]
data = ["cit-Patents","soc-LiveJournal1","road_usa","twitter7","com-friendster"]

if len(sys.argv) <= 1:
	print("Error: Please enter the folder path!")
data_path = sys.argv[1]
data_name = data_path.split('/')[-1]
output = open("summary.prof","w")

print("Extracting profiling results from {} ...".format(data_path))

def time_to_second(time):
	s = float(time.split(':')[-1])
	h = float(time.split(':')[0])
	return h * 60 + s

def extract_time(framework,dataset,time):
	framework[dataset].append(time_to_second(time))

krill = {}
ligras = {}
ligrap = {}
for f in [krill,ligras,ligrap]:
	for d in data:
		f[d] = []
output.write("Profiling results of " + data_name + "\n")
for filename in os.listdir(data_path):
	if len(filename.split('.')) != 2 or filename.split('.')[1] != "prof":
		continue
	with open(data_path + "/" + filename,"r") as file:
		dataset = filename.split(".")[0]
		for (i,line) in enumerate(file,1):
			if (i <= 3 or i > 6):
				continue
			else:
				for (j,word) in enumerate(line.split('\t'),1):
					if j == 1:
						framework = word
					elif word != "" and word != "\n" and word != "\t":
						if framework == "Krill":
							extract_time(krill,dataset,word)
						elif framework == "Ligra-S":
							extract_time(ligras,dataset,word)
						else:
							extract_time(ligrap,dataset,word)
res = [[],[],[]]
for (i,f) in enumerate([krill,ligras,ligrap]):
	if i == 0:
		output.write("Krill")
	elif i == 1:
		output.write("Ligra-S")
	else:
		output.write("Ligra-P")
	output.write('\n')
	for d in data:
		output.write(d + "\t")
		res[i].append(f[d])
		for time in f[d]:
			output.write(("%.2f" % time) + "\t")
		output.write("\n")
	output.write("\n")

for j in range(5):
	for k in range(5):
		if j < 3:
			mini = min(res[0][j][k],res[1][j][k],res[2][j][k])
		else:
			mini = min(res[0][j][k],res[1][j][k])
		res[0][j][k] /= mini
		res[1][j][k] /= mini
		res[2][j][k] /= mini

for (i,f) in enumerate([krill,ligras,ligrap]):
	if i == 0:
		output.write("Krill")
	elif i == 1:
		output.write("Ligra-S")
	else:
		output.write("Ligra-P")
	output.write("\n")
	for (j,d) in enumerate(data):
		output.write(d + "\t")
		if i < 2 or (i == 2 and j < 3):
			for time in res[i][j]:
				output.write(("%.2f" % time) + "\t")
		output.write("\n")
	output.write('\n')

for i in range(3):
	for j in range(5):
		if (j == 0):
			output.write("[[")
		else:
			output.write("[")
		for k in range(5):
			output.write(("%.2f" % res[i][j][k]) + ("," if k < 4 else ("],\n" if j < 4 else "]]\n")))
	output.write("\n")

print("Finished output!")