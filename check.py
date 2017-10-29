# -*- coding: utf-8 -*-
import json
from os import listdir
from pprint import pprint
from os.path import isfile, join
fix_mark = True
fix_mark_level_2 = False
fix_trans = False
write_to_database = False
org_folder="org"
trans_folder="trans"
fix_folder="fix"
database="database.json"
trans_files = [f for f in listdir(trans_folder) if isfile(join(trans_folder, f))]
print(trans_files)
try:
    f=open(database,"rb")
    magics=f.read()
    magic=json.loads(magics)
    f.close()
except:
    magic={}
def check(s):
    if s.find(u"...")!=-1:
        return True
    elif s.find(u"?")!=-1:
        return True
    elif s.find(u"!")!=-1:
        return True
    elif s.find(u",")!=-1:
        return True
    elif s.find(u"(")!=-1:
        return True
    elif s.find(u")")!=-1:
        return True
    return False
def change_mark(s):
    s=s.replace(u"...",u"…")
    s=s.replace(u"…..",u"……")
    s=s.replace(u"..…",u"……")
    s=s.replace(u".…",u"…")
    s=s.replace(u"….",u"…")
    s=s.replace(u"?",u"？")
    s=s.replace(u"!",u"！")
    #s=s.replace(",","，")
    s=s.replace(u"(",u"（")
    s=s.replace(u")",u"）")
    s=s.replace(u"！？",u"!?")
    s=s.replace(u"？！",u"?!")
    s=s.replace(u"！！",u"!!")
    return s
not_match_list={}
def walk(dict_var,dict_var2):
    if isinstance(dict_var,list):
        for k in range(len(dict_var)):
            walk(dict_var[k],dict_var2[k])
    elif isinstance(dict_var,dict):
        for k, v in dict_var.items():
            if k=="nameLeft" or k=="nameRight" or k=="nameCenter" or k=="textRight" or k=="textRight" or k=="textCenter" or k=="nameNarration" or k=="narration" or k=="textSelect":
                v1=dict_var[k].strip()
                v2=dict_var2[k].strip()
                # if check(v2)==True and check(v1)==False:
                #     print("  mark error:")
                #     print("    Org: "+v1.encode('utf-8'))
                #     print("    Now: "+v2.encode('utf-8'))
                if fix_mark:
                    v2=change_mark(v2)
                #v11=v1.strip(u"，（）…？！“”?!『』")
                #v22=v2.strip(u"，（）…？！“”?!『』")
                v11=v1.strip(u"，（）…？！?!『』@")
                v22=v2.strip(u"，（）…？！?!『』@")
                if v11!="":
                    p1=v1.find(v11)
                    p2=v2.find(v22)
                    if v11 in magic and magic[v11]!=v22:
                        print("  not match error:")
                        print("    Org: "+v11.encode('utf-8'))
                        print("    Pst: "+magic[v11].encode('utf-8'))
                        print("    Now: "+v22.encode('utf-8'))
                        not_match_list[v11]=magic[v11]
                        if fix_trans and fix_mark_level_2:
                            v2=v1[:p1]+magic[v11]+v1[p1+len(v11):]
                        elif fix_mark_level_2:
                            v2=v1[:p1]+v22+v1[p1+len(v11):]
                    elif v1[:p1]+v11+v1[p1+len(v11):] != v2[:p2]+v11+v2[p2+len(v22):]:
                        print("  mark not match error:")
                        print("    Org: "+v1.encode('utf-8'))
                        print("    Now: "+v2.encode('utf-8'))
                        if fix_mark_level_2:
                            v2=v1[:p1]+v22+v1[p1+len(v11):]
                        magic[v11]=v22
                    else:
                        magic[v11]=v22
                else:
                    if fix_mark_level_2:
                        v2=v1
                dict_var2[k]=v2
            else:
                walk(dict_var[k],dict_var2[k])
    else:
        pass

for f_name in trans_files:
    print("Check: "+f_name)
    f=open(join(trans_folder, f_name),"rb")
    data=f.read()
    f.close()
    try:
        jso=json.loads(data)
    except:
        jso=json.loads(data[3:])
    f=open(join(org_folder, f_name),"rb")
    data=f.read()
    f.close()
    try:
        jso2=json.loads(data)
    except:
        jso2=json.loads(data[3:])
    walk(jso2,jso)
    f=open(join(fix_folder, f_name),"wb")
    data=json.dumps(jso)
    f.write(data)
    f.close()

if write_to_database:
    f=open(database,"wb")
    magics=json.dumps(magic)
    f.write(magics)
    f.close()

print("done!")
print(json.dumps(not_match_list))
exit()
for k,v in magic.items():
    print k
    print v
exit()