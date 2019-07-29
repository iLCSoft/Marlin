#!/bin/bash


inputfile="/home/eteremi/afs/ddsim-sv02-00-01-GILD_l5_v02-Zuds200_00_000.slcio"
# cores=`seq 2 3`
# crunchtimes=`seq 500 500 2500`
# crunchtimes="6000"
cores="2"
crunchtimes="500"
output="scaling_output.txt"
# output="scaling_output_ILD_chain.txt"

rm ${output}

for c in ${cores}
do
  for cr in ${crunchtimes}
  do
    ./bin/MarlinMT \
    ./examples/scaling_mt.xml \
    --datasource.LCIOInputFiles="${inputfile} ${inputfile} ${inputfile} ${inputfile} ${inputfile} ${inputfile} ${inputfile} ${inputfile} ${inputfile} ${inputfile} ${inputfile} " \
    --datasource.MaxRecordNumber=500 \
    --CPUCrunch.CrunchTime=${cr} \
    --CPUCrunch.CrunchSigma=100 \
    --global.Verbosity=MESSAGE \
    --global.Concurrency=${c} > scaling_temp.log 
    serial=$( cat scaling_temp.log | grep serial | awk '{print $7}' )
    parallel=$( cat scaling_temp.log | grep serial | awk '{print $9}' )
    scaling=$( cat scaling_temp.log | grep serial| awk '{print $11}' )
    echo "-- N cores:        ${c}"
    echo "-- Crunch time:    ${cr}"
    echo "-- Serial time:    ${serial}"
    echo "-- Parallel time:  ${parallel}"
    echo "-- Scaling:        ${scaling}"
    echo ""
    echo "${c} ${cr} ${serial} ${parallel} ${scaling}" >> ${output}
  done
done
