import struct, sys, time

assert(len(sys.argv) == 3)
infile_name = sys.argv[1]
out_folder = sys.argv[2]
infile = open(infile_name,"r")
outfile_name = infile_name.split("/")[-1]
lines = infile.read().splitlines()
weighted_flag = True if lines[0] == "WeightedAdjacencyGraph" else False
lines = list(map(int,lines[1:]))
# print(len(lines),lines[-1])
v_num = lines.pop(0)
e_num = lines.pop(0)
offset = lines[:v_num]
offset.append(e_num)
lines = lines[v_num:]
# print(lines)
# sys.exit()
i, j = 0, 0
checkpoint = 0
edges = []
start_time = time.time()
if not weighted_flag:
    while j < e_num:
        if int(float(j+1) / float(e_num) * 100) >= checkpoint:
            print("Done {}/{} ({}%, Time: {}s)".format(j+1,e_num,checkpoint,time.time()-start_time),flush=True)
            checkpoint += 10
        if offset[i] <= j < offset[i+1]:
            edges.append((i,lines[j]))
            j += 1
        else:
            i += 1
else:
    while j < e_num:
        if int(float(j+1) / float(e_num) * 100) >= checkpoint:
            print("Done {}/{} ({}%, Time: {}s)".format(j+1,e_num,checkpoint,time.time()-start_time),flush=True)
            checkpoint += 10
        if offset[i] <= j < offset[i+1]:
            edges.append((i,lines[j],lines[j+e_num]))
            j += 1
        else:
            i += 1
# print(edges)
# sys.exit()
with open("{}/{}.config".format(out_folder,outfile_name),"w") as num_file:
    num_file.write("Weighted: {}\n{}\n{}\n".format(weighted_flag,v_num,e_num))

if not weighted_flag:
    with open("{}/{}.in".format(out_folder,outfile_name),"wb") as file:
        for edge in edges:
            file.write(struct.pack("<II",edge[0],edge[1])) # little-endian
else:
    with open("{}/{}.in".format(out_folder,outfile_name),"wb") as file:
        for edge in edges:
            file.write(struct.pack("<II",edge[0],edge[1])) # little-endian
            file.write(struct.pack("<f",edge[2]))