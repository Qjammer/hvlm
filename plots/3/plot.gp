set terminal png size 707,500
set output './plot3.png'
set title 'C_l distribution as a function of taper ratio for a constant CL=0.25'
set xlabel 'x'
set ylabel 'C_l'
set key inside left top font ",13"
set xrange [:]
set yrange [:]
set style line 1 linecolor rgb '#CC0000' linetype 1 linewidth 1 pointtype 1 pointsize 1
set style line 2 linecolor rgb '#660066' linetype 1 linewidth 1
set style line 3 linecolor rgb '#0000CC' linetype 1 linewidth 1
set style line 4 linecolor rgb '#006666' linetype 1 linewidth 1
set style line 5 linecolor rgb '#00CC00' linetype 1 linewidth 1
set style line 6 linecolor rgb '#666600' linetype 1 linewidth 1
plot 	'out0.txt' using ($1):($2) with points ls 1 title 'λ=1.00', \
		'out1.txt' using ($1):($2) with points ls 2 title 'λ=0.80', \
		'out2.txt' using ($1):($2) with points ls 3 title 'λ=0.60', \
		'out3.txt' using ($1):($2) with points ls 4 title 'λ=0.40', \
		'out4.txt' using ($1):($2) with points ls 5 title 'λ=0.20', \
		'out5.txt' using ($1):($2) with points ls 6 title 'λ=0.00'
