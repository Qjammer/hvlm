set terminal png size 707,500
set output './plot2.png'
set title 'C_L slope and position of aerodynamic center as a function of Sweep Angle'
set xlabel 'Sweep Angle'
set ylabel 'C_L/α'
set y2label 'x_ac'
set key inside left center font ",13"
set xrange [:]
set yrange [:]
set ytics nomirror
set y2tics
set style line 1 linecolor rgb '#0060ad' linetype 1 linewidth 1 pointtype 1 pointsize 1
set style line 2 linecolor rgb '#AA1111' linetype 1 linewidth 1
set style line 3 linecolor rgb '#11DD11' linetype 1 linewidth 2
plot 	'outputs.txt' using ($1):($3-$2)*180/((15+5)*3.14159265359) with points ls 1 title 'C_L/α (HVM)' axes x1y1, \
	'outputs.txt' using ($1):(-$5+$4)/($3-$2) with points ls 2 title 'x_ac' axes x1y2, \
	'katz.dat' using ($1):($2) with points ls 3 title 'C_L/α (Katz & Plotkin)' axes x1y1
