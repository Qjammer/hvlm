set terminal png size 707,500
set output './plot4AR.png'
set title 'Oswald efficiency factor as a function of taper and aspect ratio'
set xlabel 'λ'
set ylabel 'e_o'
set key inside left top font ",13"
set xrange [0:1]
set yrange [:]
set style line 1 linecolor rgb '#CC0000' linetype 1 linewidth 1 pointtype 1 pointsize 1
set style line 2 linecolor rgb '#660066' linetype 1 linewidth 1
set style line 3 linecolor rgb '#0000CC' linetype 1 linewidth 1
set style line 4 linecolor rgb '#006666' linetype 1 linewidth 1
set style line 5 linecolor rgb '#00CC00' linetype 1 linewidth 1
set style line 6 linecolor rgb '#666600' linetype 1 linewidth 1
plot 	'outAR4.txt' using ($1):($2) with points ls 1 title 'AR=4.0', \
		'outAR8.txt' using ($1):($2) with points ls 2 title 'AR=8.0', \
		'outAR10.txt' using ($1):($2) with points ls 3 title 'AR=10.0'
