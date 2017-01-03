set terminal png size 707,500
set output './plot5a.png'
set title 'Basic and Additional Lift Distributions with and without Flaps'
set xlabel 'x'
set ylabel 'Cl'
set key inside right center font ",13"
set xrange [0:0.5]
set yrange [:]
set xzeroaxis
stats 'outdata.txt' using 0:($0==0?(x1=$1):$1)
print x1
stats 'outdata.txt' using 0:($0==0?(x2=$2):$2)
print x2
LABEL =sprintf("%s%f%s%f", "C_M0=",x1,"\nx_ac=",x2)
set obj 10 rect at 0.1,0.6 size char strlen("C_M0=0.012334"), char 3
set obj 10 fillstyle empty border -1 front
set label 10 at 0.1,0.64 LABEL front center

set style line 1 linecolor rgb '#CC0000' linetype 1 linewidth 1 pointtype 1 pointsize 1
set style line 2 linecolor rgb '#660066' linetype 1 linewidth 1
set style line 3 linecolor rgb '#0000CC' linetype 1 linewidth 1
set style line 4 linecolor rgb '#006666' linetype 1 linewidth 1
set style line 5 linecolor rgb '#00CC00' linetype 1 linewidth 1
set style line 6 linecolor rgb '#666600' linetype 1 linewidth 1
plot 	'out.txt' using ($1):($2) with points ls 1 title 'Basic Cl, No Flaps', \
		'out.txt' using ($1):($4-$2) with points ls 2 title 'Add. Cl, No Flaps', \
		'out.txt' using ($1):($6) with points ls 3 title 'Basic Cl, Flaps', \
		'out.txt' using ($1):($8-$6) with points ls 4 title 'Add. Cl, Flaps'
