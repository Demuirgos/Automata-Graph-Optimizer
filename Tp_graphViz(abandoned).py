import re
import os
from graphviz import *
os.system("cls")
print("Please enter the Path of your graphViz (gv/dot) file : ")
fileName=input()
while not os.path.exists(fileName):
    os.system("cls")
    print("File not found or File not Valid!\nPlease TRY AGAIN!")
    fileName=input()
fileName2="output2.dot"
inFile=open(fileName,"r")
outFile=open(fileName2,"w")
g={}
eps_clos={}
traversed={}
processing=[]
staring_nodes=[]
ending_nodes=[]
def extract_0_clos(i):
    if i in eps_clos:
        return eps_clos[i]
    processing.append(i)
    print("     Extracting epsilon closes of node : ",i)
    eps=[i]
    for k in g[i]:
        if k[0]=='0':
            l=[]
            if k[1] in g and k[1] not in processing :
                l=extract_0_clos(k[1])
            else:
                l=[k[1]]
            for j in l:
                if j not in eps:
                    eps.append(j)
    eps_clos[i]=eps
    for n in eps_clos[i]:
        if n in ending_nodes:
            if not i in ending_nodes:
                ending_nodes.append(i)
    processing.remove(i)
    return eps
    print("     DONE")
def extract_clos():
    print("Determining the Closes of graph nodes:")
    for i in g:
        extract_0_clos(i)
    print("DONE")
def insert(g,s,f,w):
    print("     Inserting Relation",s,"->",f,"lable=",w)
    if s in g:
        if [w,f] not in g[int(s)]:
            g[int(s)].append([w,f])
    else :
        g[int(s)]=[[w,f]]
    traversed[int(s)]=False
def read(inFile):
    print("Reading FILE :")
    gv_content=inFile.read()
    #graph_name=re.findall("digraph (\w+)\n?{",gv_content)[0]
    lines=re.findall("(?:(\d+) *-> *(\d+).*\"([a-zA-Z0 ]+)\")",gv_content)
    end=re.findall("(\d+) *-> *(?:f|F)",gv_content)
    start=re.findall("(?:s|S) *-> *(\d+)",gv_content)
    for i in lines:
        if not(i[0]==i[1] and i[2]=='0'):
            insert(g,int(i[0]),int(i[1]),i[2])
    for i in end:
        ending_nodes.append(int(i))
    print(ending_nodes)
    for i in start:
        staring_nodes.append(int(i))
    if len(staring_nodes)==0:
        print("Please enter the number of starting node :")
        for _ in range(int(input())):
            staring_nodes.append(int(input()))
    if len(ending_nodes)==0:
        print("Please enter the number of ending node :")
        for _ in range(int(input())):
            ending_nodes.append(int(input()))
    print("Done")
def duplicate(i):
    print("     Processing Node :",i)
    for j in eps_clos[i]:
        print(j)
        if(j in g):
            for k in g[j][0:]:
                if not k in g[i] and k[0]!='0':
                    g[i].append([k[0],k[1]])
def optimize():
    print("Optimizing :")
    for i in g:
        duplicate(i)
        remove(i)
    print("DONE")
def traverse(f,i):
    if i in g:
        compress(i)
        traversed[i]=True
        for k in g[i]:
            if k[0]!='0':
                if k[1] in ending_nodes:
                    f.write(str(k[1])+"[shape=doublecircle];\n")
                print("     Writing relation :",end=" ")
                relation=str(i)+"->"+str(k[1])+"[label=\""+k[0]+"\"]"
                print(relation)
                f.write(relation+";\n")
                if k[1] in traversed and traversed[k[1]]==False:
                    traverse(f,k[1])
def write():
    print("Writing to Output File:")
    outFile.write("digraph "+ "result" +"{\n")
    traverse(outFile,0)
    outFile.write("}")
    outFile.close()
    print("Done")
def remove(i):
    e=0
    while e < len(g[i]):
        if g[i][e][0]=='0':
            g[i].remove(g[i][e])
            e-=1
        e+=1
def determine():
    global staring_nodes
    global ending_nodes
    global g
    verified=[]
    unverified=[]
    DFA_table={}
    unverified.append(staring_nodes)
    new_ends=[]
    while len(unverified)>0:
        current=unverified.pop()
        verified.append(current)
        signature={}
        for i in current:
            if i in ending_nodes:
                    new_ends.append(verified.index(current))
            if i in g:
                for e in g[i] :
                    if e[0] in signature :
                        signature[e[0]].append(e[1])
                    else :
                        signature[e[0]]=[e[1]]
        for e in signature:
            if not signature[e] in unverified and not signature[e] in verified:
                unverified.append(signature[e])
        DFA_table[verified.index(current)]=signature
    DFA={}
    print("Creating New DFA graph :")
    for i in DFA_table:
        for j in DFA_table[i]:
            insert(DFA,i,verified.index(DFA_table[i][j]),j)
    print("Done")
    staring_nodes=[0]
    ending_nodes=new_ends
    g=DFA 
def reverse():
    global staring_nodes
    global ending_nodes
    global g
    rev_g={}
    print("Reversing Graph :")
    for i in g:
        for e in g[i]:
            insert(rev_g,e[1],i,e[0])
    staring_nodes=ending_nodes
    ending_nodes=[0]
    print("Done")
    g=rev_g
def minimize():
    reverse()
    determine()
    reverse()
    determine()
    return
def compress(i):
    remove(i)
    g[i].sort(key=lambda elem:elem[1])
    e=0
    while e < len(g[i])-1:
        if g[i][e][1]==g[i][e+1][1]:
            g[i][e][0]+=', '+g[i][e+1][0]
            g[i].remove(g[i][e+1])
        e+=1
if __name__ == "__main__" :
    read(inFile)
    extract_clos()
    optimize()
    determine()
    minimize()
    write()
    out_src=Source(open(fileName2,"r").read())
    int_src=Source(open(fileName,"r").read())
    Done=False
    ans=True
    while not Done:
        try:
            print("Rendering Graphically the Graphs:")
            int_src.render(fileName,format="png",view=False)#view=True to open file automatically
            out_src.render(fileName2,format="png",view=False)
            Done=True
        except:
            ans=False
            print("please close the opened previously generated PDFs!")
            print("When DONE please enter 1:continue")
            ans=input()
            while ans!='1':
                print("invalid input, please retry!")
                ans=input()
    print("Done!\nProcess Completed!")
