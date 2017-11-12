#!/bin/sh

RESULTDIR=result/
h=`hostname`

if [ "$h" = "mba-i1.uncc.edu"  ];
then
    echo Do not run this on the headnode of the cluster, use qsub!
    exit 1
fi

if [ ! -d ${RESULTDIR} ];
then
    mkdir ${RESULTDIR}
fi


N="100000000"
THREADS="1 2 4 8 12 16"

make reduce

for n in $N;
do
    for t in $THREADS;
    do
	./reduce $n $t >/dev/null 2> ${RESULTDIR}/reduction_${n}_${t}
    done
done
	     
for n in $N;
do
    for t in $THREADS;
    do
	#output in format "thread seq par"
	echo ${t} \
	     $(cat ${RESULTDIR}/reduction_${n}_1) \
	     $(cat ${RESULTDIR}/reduction_${n}_${t})
    done   > ${RESULTDIR}/speedup_reduction_${n}_static_-1

done

gnuplot <<EOF

set terminal pdf
set output 'reduce_plots.pdf'

set style data linespoints


set key top left;
set xlabel 'threads'; 
set ylabel 'speedup';
set xrange [1:20];
set yrange [0:20];
set title 'n=$N';
plot '${RESULTDIR}/speedup_reduction_${N}_static_-1' u 1:(\$2/\$3) t 'static' lc 1;

EOF
