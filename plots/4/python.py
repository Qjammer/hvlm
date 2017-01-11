#!/usr/bin/env python
import subprocess
from scipy import stats
from math import pi
from numpy import arange

AR=[4,8,10]
div=100

for ar in AR:
	f=open("outAR"+str(ar)+".txt","w")
	for tr in arange(0,1.025,0.025):
		CLarr=[];
		CL2arr=[];
		CDarr=[];
		for angle in range(-5, 10, 1):
			val=subprocess.check_output(['../../hvm','-d','-p','1',str(ar),str(tr),'0','0','0','0','0','0','0','0','0',str(div),'1',str(angle)])
			CL=0;
			CD=0;
			for item in val.split("\n"):
				if "CL:" in item:
					CL=float(item.split(":")[1])
					CLarr.append(CL)
					CL2arr.append(CL*CL)
				if "CDi:" in item:
					CD=float(item.split(":")[1])
					CDarr.append(CD)
		(slope,yint,r,tt,stderr)=stats.linregress(CL2arr,CDarr)
		oswald=1/(pi*ar*slope)
		print [ar,tr,oswald]
		f.write(str(tr)+"\t"+str(oswald)+"\n")
	f.close()

subprocess.check_output(['gnuplot','plotAR.gp'])

SWEEP=[0,30,60]

for sweep in SWEEP:
	f=open("outSW"+str(sweep)+".txt","w")
	for tr in arange(0,1.025,0.025):
		CLarr=[];
		CL2arr=[];
		CDarr=[];
		for angle in range(-5, 10, 1):
			val=subprocess.check_output(['../../hvm','-d','-p','1','8',str(tr),str(sweep),'0','0','0','0','0','0','0','0',str(div),'1',str(angle)])
			CL=0;
			CD=0;
			for item in val.split("\n"):
				if "CL:" in item:
					CL=float(item.split(":")[1])
					CLarr.append(CL)
					CL2arr.append(CL*CL)
				if "CDi:" in item:
					CD=float(item.split(":")[1])
					CDarr.append(CD)
		(slope,yint,r,tt,stderr)=stats.linregress(CL2arr,CDarr)
		# print slope
		# print yint
		oswald=1/(pi*8*slope)
		print [sweep,tr,oswald]
		f.write(str(tr)+"\t"+str(oswald)+"\n")
	f.close()

subprocess.check_output(['gnuplot','plotSW.gp'])
