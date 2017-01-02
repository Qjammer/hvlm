#!/bin/bash
output="#x\tCL_-5...\n";

vals="";

curr=0;
prev=0;
delta=5;
dir=1;
div=50;

sequ=$(seq 1 -0.2 0)

for i in $sequ
do
	curr=0;
	prev=0;
	delta=5;
	dir=1;

	out="$(../../hvm -d -p 1 10 $i 0 0 0 0 0 0 0 0 0 $div 1 $curr | grep "CL:"| cut -d":" -f 2;)";
	if [ 1 -eq "$(echo "$curr<0.25"|bc)" ]
	then
		dir=1;
	else
		dir=-1;
	fi

	while [ 1 -eq "$(echo "$dir*($out-0.25)<0"|bc)" ]
	do
		prev=$curr;
		curr=$(echo "$curr+$dir*$delta"|bc);
		out="$(../../hvm -d -p 1 10 $i 0 0 0 0 0 0 0 0 0 $div 1 $curr | grep "CL:"| cut -d":" -f 2;)";
	done


	for j in $(seq 0 1 50)
	do
		delta=$(echo "0.50000000*$delta"|bc);
		if [ 1 -eq "$(echo "$dir*($out-0.25)>0"|bc)" ]
		then
			dir=$(echo "-1*$dir"|bc);
		fi
		curr=$(echo "$curr+$dir*$delta"|bc);
		out="$(../../hvm -d -p 1 10 $i 0 0 0 0 0 0 0 0 0 $div 1 $curr | grep "CL:"| cut -d":" -f 2;)";

	done
	vals="$vals $curr";
done

arrvals=($vals);

arr=($sequ);
result[0]="";

for i in "${!arr[@]}"
do
	result[$i]="$(../../hvm -d -p 1 10 ${arr[$i]} 0 0 0 0 0 0 0 0 0 $div 1 ${arrvals[$i]})"
	result[$i]="$(printf "${result[$i]}"|cut -f1 -d"C"|sed 1d|sed 1d)";
	printf "${result[$i]}" > "out$i.txt";
done

$(gnuplot plot.gp)
