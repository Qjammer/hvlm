set terminal png size 707,500
set output './plot5b.png'
set title 'CL of wing when stall Cl is achieved in each section'

set xlabel 'x'
set ylabel 'CL(x)'
set key inside center top font ",13"
set xrange [0:0.5]
set yrange [0:]
set xzeroaxis

max=1.6
flappos=0.3
fpos=flappos/2

alpha0noflap=system("head -2 data.dat|tail -1|cut -d\"\t\" -f 3")
alpha1noflap=system("head -2 data.dat|tail -1|cut -d\"\t\" -f 4")

alpha0flap=system("head -2 data.dat|tail -1|cut -d\"\t\" -f 5")
alpha1flap=system("head -2 data.dat|tail -1|cut -d\"\t\" -f 6")

flapdelta=-(alpha0flap-alpha0noflap)/(alpha1flap-alpha0flap)

stats 'out.txt' using ($1):(max-$2)/($4-$2) name "A"
cl1=A_min_y
ind1=A_index_min_y
amax1=cl1*(alpha1noflap-alpha0noflap)+alpha0noflap

stats 'out.txt' using ($1):(max+($1<fpos?flapdelta:0)-$6)/($8-$6) name "B"
cl2=B_min_y
ind2=B_index_min_y
amax2=cl2*(alpha1flap-alpha0flap)+alpha0flap

LABEL =sprintf("%s%f%s%f", "α_max, no flap=",amax1,"\nα_max, flap=",amax2)
set obj 10 rect at 0.2,0.6 size char strlen("α_max, no flap==0.04"), char 3
set obj 10 fillstyle empty border -1 front
set label 10 at 0.2,0.68 LABEL front center
print flapdelta

set style line 1 linecolor rgb '#CC0000' linetype 1 linewidth 1 pointtype 1 pointsize 1
set style line 2 linecolor rgb '#660066' linetype 1 linewidth 1
set style line 3 linecolor rgb '#0000CC' linetype 1 linewidth 1
set style line 4 linecolor rgb '#006666' linetype 1 linewidth 1
set style line 5 linecolor rgb '#00CC00' linetype 1 linewidth 1
set style line 6 linecolor rgb '#666600' linetype 1 linewidth 1
plot 	'out.txt' using ($1):(max-$2)/($4-$2) with points ls 1 title 'CL(x), No Flaps', \
		'out.txt' using ($1):(max+($1<fpos?flapdelta:0)-$6)/($8-$6) with points ls 2 title 'CL(x), Flaps', \
		cl1 ls 1 title sprintf("%s%f","CL_max=",cl1), \
		cl2 ls 2 title sprintf("%s%f","CL_max=",cl2)
