#!/bin/sh
output="#AR\tCL_-5\tCL_15\tCM_-5\tCM_15\n";

for i in $(seq 0 2.5 80)
do
	output="$output$i\t";
	output="$output $(../../hvm -d -p 1 6 1 $i 0 0 0 0 0 0 0 0 200 1 -5 | grep "CL:"| cut -d":" -f 2;)"
	output="$output\t";
	output="$output $(../../hvm -d -p 1 6 1 $i 0 0 0 0 0 0 0 0 200 1 15 | grep "CL:"| cut -d":" -f 2;)"
	output="$output\t";
	output="$output $(../../hvm -d -p 1 6 1 $i 0 0 0 0 0 0 0 0 200 1 -5 | grep "CMle:"| cut -d":" -f 2;)"
	output="$output\t";
	output="$output $(../../hvm -d -p 1 6 1 $i 0 0 0 0 0 0 0 0 200 1 15 | grep "CMle:"| cut -d":" -f 2;)"
	output="$output \n";
done

echo $output>outputs.txt

$(gnuplot plot.gp)
