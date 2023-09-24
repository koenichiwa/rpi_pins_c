#!/bin/bash
file=$1
cores=$2

max=`cat $file | grep "Max Latencies" | tr " " "\n" | sort -n | tail -1 | sed s/^0*//` || max=`cat $file | grep "Max Latencies" | tr " " "\n" | sort -n | tail -1`
grep -v -e "^#" -e "^$" $file | tr " " "\t" | sed -E 's/[[:space:]]+/\t/g' > histogram

for i in `seq 1 $cores`
do
  column=`expr $i + 1`
  cut -f1,$column histogram >histogram$i
done

echo -n -e "set title \"Latency plot\"\n\
set terminal png\n\
set xlabel \"Latency (us), max $max us\"\n\
set logscale y\n\
set xrange [0:400]\n\
set yrange [0.8:*]\n\
set ylabel \"Number of latency samples\"\n\
set output \"plot.png\"\n\
plot " >plotcmd

for i in `seq 1 $cores`
do
  if test $i != 1
  then
    echo -n ", " >>plotcmd
  fi
  cpuno=`expr $i - 1`
  if test $cpuno -lt 10
  then
    title=" CPU$cpuno"
   else
    title="CPU$cpuno"
  fi
  echo -n "\"histogram$i\" using 1:2 title \"$title\" with histeps" >> plotcmd
done

gnuplot -persist < plotcmd
