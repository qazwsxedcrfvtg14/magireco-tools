# -*- coding: utf-8 -*-
import json
from os import listdir
from pprint import pprint
from os.path import isfile, join
fix_folder="fix"
#fil=["510051-0.json","510051-1.json","510051-2.json","510051-3.json","510051-4.json","510051-5.json","510051-6.json","510051-7.json","510051-8.json","510051-9.json","510051-10.json","510051-11.json","510051-12.json","510051-13.json"]
fil = [f for f in listdir(fix_folder) if isfile(join(fix_folder, f))]
final_fil="101501-1_A1rur.json"
result={}
magic={"bg": "black","bgm": "stop","autoTurnLast": 2,"narrationEffect": "out","turnEffect": "stop"}
id=0
now_group="group_"+str(id)
end_group=[]
for f_name in fil:
    f=open(join(fix_folder, f_name),"rb")
    data=f.read()
    f.close()
    jso=json.loads(data)
    id=id+1
    now_group="group_"+str(id)
    for grp in end_group:
        result["story"][grp].append({"changeGroup":now_group})
    end_group=[]
    group_set={}
    group_set["group_1"]=now_group
    if result=={}:
        result=jso
        result["story"][now_group].insert(0,magic)
    else:
        result["story"][now_group]=[magic]
        result["story"][now_group].extend(jso["story"]["group_1"])
    group_queue=[now_group]
    #print "OK"
    while len(group_queue) :
        now=group_queue[0]
        print now
        group_queue.pop(0)
        if "select" in result["story"][now][-1]:
            for i in range(len(result["story"][now][-1]["select"])):
                if result["story"][now][-1]["select"][i]["group"] not in group_set:
                    id=id+1
                    new_group="group_"+str(id)
                    next=result["story"][now][-1]["select"][i]["group"]
                    result["story"][now][-1]["select"][i]["group"]=new_group
                    result["story"][new_group]=jso["story"][next]
                    group_set[next]=new_group
                    group_queue.append(new_group)
                else:
                    result["story"][now][-1]["select"][i]["group"]=group_set[result["story"][now][-1]["select"][i]["group"]]
        elif "changeGroup" in result["story"][now][-1]:
            if result["story"][now][-1]["changeGroup"] not in group_set:
                #print("XD")
                id=id+1
                new_group="group_"+str(id)
                next=result["story"][now][-1]["changeGroup"]
                result["story"][now][-1]["changeGroup"]=new_group
                result["story"][new_group]=jso["story"][next]
                group_set[next]=new_group
                group_queue.append(new_group)
            else:
                result["story"][now][-1]["changeGroup"]=group_set[result["story"][now][-1]["changeGroup"]]
        else:
            end_group.append(now)

for grp in end_group:
    result["story"][grp].append(magic)
#result["story"][now_group].insert(0,magic)
f=open(final_fil,"wb")
f.write(json.dumps(result))
f.close()