set boxwidth 0.2
set style fill solid
set xtics nomirror rotate by -45
set yrange [-0.5:]

set style fill solid 0.1 border
set title "Connection feasibility testing"
set xlabel "Number of agents (with 4 male and 2 female interfaces)"
set ylabel "Computation time in seconds"

plot "p2_100_10_epochs_updated.log" using 1:4:5 with yerrorbars notitle, \
     "p2_100_10_epochs_updated.log" using 1:4 with lines title "single agent type", \
     "with_agent_idx.log" using 1:8 with lines title "b3c3p25s5 reference"

set terminal pngcairo size 800,800 enhanced font 'Verdana,14'
set output "organization_model-time.png"
replot


set ylabel "Number of propagations"
plot "p2_100_10_epochs_updated.log" using 1:8:9 with yerrorbars notitle, \
     "p2_100_10_epochs_updated.log" using 1:8 with lines title "propagations", \
     "with_agent_idx.log" using 1:12 with lines title "b3c3p25s5 reference"

set terminal pngcairo size 800,800 enhanced font 'Verdana,14'
set output "organization_model-propagate.png"
replot

set ylabel "Number of failed nodes"
plot "p2_100_10_epochs_updated.log" using 1:10:11 with yerrorbars notitle, \
     "p2_100_10_epochs_updated.log" using 1:10 with lines title "failed nodes"

set terminal pngcairo size 800,800 enhanced font 'Verdana,14'
set output "organization_model-failed_nodes.png"
replot

set ylabel "Number of restarts"
plot "p2_100_10_epochs_updated.log" using 1:16:17 with yerrorbars notitle, \
     "p2_100_10_epochs_updated.log" using 1:16 with lines title "restart", \
     "with_agent_idx.log" using 1:20 with lines title "b3c3p25s5 reference"

set terminal pngcairo size 800,800 enhanced font 'Verdana,14'
set output "organization_model-restarts.png"
replot
