set boxwidth 0.2
set style fill solid
set xtics nomirror rotate by -45
set yrange [0:]

set style fill solid 0.1 border
set title "Computing the functional mapping"
set xlabel "Number of agents"
set ylabel "Computation time in seconds"

plot "p2_100_10.log" using 1:2:3 with yerrorbars notitle, \
     "p2_100_10.log" using 1:2 with lines title "without functional saturation bound", \
     "p2_100_10.log" using 1:4:5 with yerrorbars notitle, \
     "p2_100_10.log" using 1:4 with lines title "with functional saturation bound"

set terminal pngcairo size 800,800 enhanced font 'Verdana,14'
set output "p2_100_10-functional-saturation-bound.png"
#set terminal eps enhanced color# size 3.2in,5in
#set output "p2_100_10-functional-saturation-bound.eps"
replot


