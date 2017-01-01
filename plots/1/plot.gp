set terminal png size 707,500
set output './plot1.png'
set title 'C_L slope as a function of Aspect Ratio'
set xlabel 'Aspect Ratio'
set ylabel 'C_L/α'
set key inside right bottom font ",13"
set xrange [0:]
set yrange [0:]
set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 1 pointtype 1 pointsize 1
set style line 2 linecolor rgb '#AA1111' linetype 1 linewidth 1
set style line 3 linecolor rgb '#11DD11' linetype 1 linewidth 2
plot 	'outputs.txt' using ($1):($3-$2)*180/((15+5)*3.1415) with points ls 1 title 'HVM',\
	'katz.dat' using ($1):($2) with points ls 2 title 'Katz & Plotkin'
