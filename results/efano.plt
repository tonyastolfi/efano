#set terminal pdfcairo size 10cm, 7.5cm
set terminal png 
set term png font "arial,14"

set output "../images/efano.png"
set datafile separator ","
#set logscale x 2
set logscale y 10
set key autotitle columnheader
set xrange [1:60]

plot "efano_1m_expected.csv" using 1:2 with linespoints, \
     "efano_1m_actual.csv" using 1:2 with lines, \
     "efano_1m_actual.csv" using 3:4 with lines, \
     "efano_1m_actual.csv" using 5:6 with lines, \
     "efano_1m_actual.csv" using 7:8 with lines, \
     "efano_1m_actual.csv" using 9:10 with lines, \
     "efano_1m_actual.csv" using 11:12 with lines, \
     "efano_1m_actual.csv" using 13:14 with lines, \
     "efano_1m_actual.csv" using 15:16 with lines, \
     "efano_1m_actual.csv" using 17:18 with lines, \
     "efano_1m_actual.csv" using 19:20 with lines, \
     "efano_1m_actual.csv" using 21:22 with lines, \
     "efano_1m_actual.csv" using 23:24 with lines, \
     "efano_1m_actual.csv" using 25:26 with lines, \
     "efano_1m_actual.csv" using 27:28 with lines, \
     "efano_1m_actual.csv" using 29:30 with lines, \
     "efano_1m_actual.csv" using 31:32 with lines, \
     "efano_1m_actual.csv" using 33:34 with lines 
