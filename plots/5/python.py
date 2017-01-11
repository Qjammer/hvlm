#!/usr/bin/env python
import subprocess
from scipy import stats
from math import pi
from numpy import arange
div=50
out=0
curr=0
prev=0
delta=1
dir=1
target=0.0
params=['../../hvm','-d','-p','1','8','0.5','9.5','-2','-2','5','0','0','0','0','0',str(div),'1',str(curr)]

xacvals=[]
alphanoflap=[]
alphaflap=[]

val=subprocess.check_output(params)
for item in val.split("\n"):
	if "CL:" in item:
		out=float(item.split(":")[1])
if (out>target):
	dir=-1;
while ( dir*(out-target)<0 ):
	curr+=dir*delta
	params[17]=str(curr)
	val=subprocess.check_output(params)
	for item in val.split("\n"):
		if "CL:" in item:
			out=float(item.split(":")[1])

for j in range(50):
	delta*=0.5
	if (dir*(out-target)>0 ):
		dir*=-1
	curr+=dir*delta
	params[17]=str(curr)
	val=subprocess.check_output(params)
	for item in val.split("\n"):
		if "CL:" in item:
			out=float(item.split(":")[1])

for item in val.split("\n"):
	if "CL:" in item:
		xacvals.append(float(item.split(":")[1]))
	if "CMle:" in item:
		xacvals.append(float(item.split(":")[1]))
alphanoflap.append(curr)

f=open("outnoflap0.txt","w")
f.write(val)
f.close()

out=0
curr=0
prev=0
delta=1
dir=1
target=1.0

for item in val.split("\n"):
	if "CL:" in item:
		out=float(item.split(":")[1])
if (out>target):
	dir=-1;
while ( dir*(out-target)<0 ):
	curr+=dir*delta
	params[17]=str(curr)
	val=subprocess.check_output(params)
	for item in val.split("\n"):
		if "CL:" in item:
			out=float(item.split(":")[1])


for j in range(50):
	delta*=0.5
	if (dir*(out-target)>0 ):
		dir*=-1
	curr+=dir*delta
	params[17]=str(curr)
	val=subprocess.check_output(params)
	for item in val.split("\n"):
		if "CL:" in item:
			out=float(item.split(":")[1])

for item in val.split("\n"):
	if "CL:" in item:
		xacvals.append(float(item.split(":")[1]))
	if "CMle:" in item:
		xacvals.append(float(item.split(":")[1]))
alphanoflap.append(curr)

f=open("outnoflap1.txt","w")
f.write(val)
f.close()


out=0
curr=0
prev=0
delta=1
dir=1
target=0.0
params=['../../hvm','-d','-p','1','8','0.5','9.5','-2','-2','5','0','0.8','0.0','0.3','10',str(div),'1',str(curr)]

for item in val.split("\n"):
	if "CL:" in item:
		out=float(item.split(":")[1])
if (out>target):
	dir=-1;
while ( dir*(out-target)<0 ):
	curr+=dir*delta
	params[17]=str(curr)
	val=subprocess.check_output(params)
	for item in val.split("\n"):
		if "CL:" in item:
			out=float(item.split(":")[1])


for j in range(50):
	delta*=0.5
	if (dir*(out-target)>0 ):
		dir*=-1
	curr+=dir*delta
	params[17]=str(curr)
	val=subprocess.check_output(params)
	for item in val.split("\n"):
		if "CL:" in item:
			out=float(item.split(":")[1])
alphaflap.append(curr)

f=open("outflap0.txt","w")
f.write(val)
f.close()

out=0
curr=0
prev=0
delta=1
dir=1
target=1.0

for item in val.split("\n"):
	if "CL:" in item:
		out=float(item.split(":")[1])
if (out>target):
	dir=-1;
while ( dir*(out-target)<0 ):
	curr+=dir*delta
	params[17]=str(curr)
	val=subprocess.check_output(params)
	for item in val.split("\n"):
		if "CL:" in item:
			out=float(item.split(":")[1])


for j in range(50):
	delta*=0.5
	if (dir*(out-target)>0 ):
		dir*=-1
	curr+=dir*delta
	params[17]=str(curr)
	val=subprocess.check_output(params)
	for item in val.split("\n"):
		if "CL:" in item:
			out=float(item.split(":")[1])
alphaflap.append(curr)

f=open("outflap1.txt","w")
f.write(val)
f.close()


val=subprocess.check_output(["paste","outnoflap0.txt","outnoflap1.txt","outflap0.txt","outflap1.txt"])
f=open("out.txt","w")
val=val[val.find("\n")+1:val.rfind("\n")]
val=val[0:val.rfind("\n")]
val=val[0:val.rfind("\n")]
val=val[0:val.rfind("\n")]
f.write(val)
f.close()

x_ac=-(xacvals[3]-xacvals[1])/(xacvals[2]-xacvals[0])
f=open("outdata.txt","w")
f.write("#C_M0\tx_ac\talphaCl=0noflap\talphaCl=1noflap\talphaCl=0flap\talphaCl=1flap\n")
f.write(str(xacvals[1]+xacvals[0]*x_ac)+"\t")
f.write(str(x_ac)+"\t")
f.write(str(alphanoflap[0])+"\t")
f.write(str(alphanoflap[1])+"\t")
f.write(str(alphaflap[0])+"\t")
f.write(str(alphaflap[1])+"\t")
f.close()

subprocess.check_output(['gnuplot','plot1.gp'])
subprocess.check_output(['gnuplot','plot2.gp'])
