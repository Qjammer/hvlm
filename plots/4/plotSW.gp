set terminal png size 707,500
set output './plot4SW.png'
set title 'Oswald efficiency factor as a function of taper and sweep angle'
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
plot 	'outSW0.txt' using ($1):($2) with points ls 1 title 'Sweep=0.0°', \
		'outSW30.txt' using ($1):($2) with points ls 2 title 'Sweep=30.0°', \
		'outSW60.txt' using ($1):($2) with points ls 3 title 'Sweep=60.0°'
