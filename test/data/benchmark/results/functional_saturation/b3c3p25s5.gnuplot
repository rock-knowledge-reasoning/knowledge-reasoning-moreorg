set boxwidth 0.2
set style fill solid
#set xtics nomirror rotate by -45
set yrange [0:]

set boxwidth 0.2
set style fill
#set xtics nomirror rotate by -25
set xtics ( "w/o fsb" 1, "with fsb" 2)


set style fill solid 0.1 border
set title "Computing the functional mapping\ncomposite agent: b3c3p25s5"
#set xlabel "Number of agents"
set ylabel "Computation time in seconds"

plot "b3c3p25s5.log" using 1:6 with boxes notitle, \
     "b3c3p25s5.log" using 1:6:7 with yerrorbars notitle

set terminal pngcairo size 400,800 enhanced font 'Verdana,14'
set output "b3c3p25s5-functional-saturation-bound.png"
#set terminal eps enhanced color size 3.2in,1in
#set output "b3c3p25s5-functional-saturation-bound.eps"
replot


