for num in [1,2,4,8,16,32,64,128]:
	output = open("./multibfs/multibfss" + str(num) + ".sh","w")
	output.write("#!/usr/bin/env bash\n")
	for i in range(1,num+1):
		output.write("./$1/BFS -r " + str(10*i) + " $2")
		output.write("\n" if i != num else "")
	output.close()

for num in [1,2,4,8,16,32,64,128]:
	output = open("./multibfs/multibfsp" + str(num) + ".sh","w")
	output.write("#!/usr/bin/env bash\n")
	for i in range(1,num+1):
		output.write("./$1/BFS -r " + str(10*i) + " $2 &")
		output.write("\n" if i != num else "")
	output.write("wait")
	output.close()