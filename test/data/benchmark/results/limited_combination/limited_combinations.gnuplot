set key left top width -4
set title "Computing (limited) combinations: composite agent types"
set xlabel "Maximum number of agents in combinations"
set ylabel "# of combinations"
plot "exact_size_b3c3p25s5.log" using 1:2 with lines title "exact # of combinations", \
     "max_size_b3c3p25s5.log" using 1:2 with lines title "sum of combinations up to this size"

set terminal pngcairo size 800,800 enhanced font 'Verdana,14'
set output "b3c3p25s5-limited combinations.png"
replot

