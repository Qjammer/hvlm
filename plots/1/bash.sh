#!/bin/sh
output="#AR\tCL_-5\tCL_15\n";



for i in $(seq 6 1 50)
do
	output="$output$i\t";
	output="$output $(../../hvm -d -p 1 $i 1 0 0 0 0 0 0 0 0 0 200 1 -5 | grep "CL:"| cut -d":" -f 2;)"
	output="$output\t";
	output="$output $(../../hvm -d -p 1 $i 1 0 0 0 0 0 0 0 0 0 200 1 15 | grep "CL:"| cut -d":" -f 2;)"
	output="$output\n";
done

echo $output>out.txt

$(gnuplot plot.gp)
