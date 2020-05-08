#!/bin/bash

bench=$1
outfile=$2
seed=$3



shift 3
TotalEvaluations=1000
alpha=2.3688
beta=5.6
ph_max=10.3
ph_min=0.01
rho=0.01
TotalAnts=50


while [ $# != 0 ]; do
    flag="$1"
    case "$flag" in
        -alpha) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              alpha=$arg
            fi
            ;;
        -beta) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              beta=$arg
            fi
	    ;;
        -rho) if [ $# -gt 1 ]; then
              arg="$2"
              shift
              rho=$arg
            fi
	    ;;
        *) echo "Unrecognized flag or argument: $flag"
            ;;
        esac
    shift
done

rm -rf ${outfile}


echo "./AntKnapsack ${bench} ${seed} ${TotalAnts} ${TotalEvaluations} ${alpha} ${beta} ${ph_max} ${ph_min} ${rho} >> ${outfile}"
./AntKnapsack ${bench} ${seed} ${TotalAnts} ${TotalEvaluations} ${alpha} ${beta} ${ph_max} ${ph_min} ${rho} >> ${outfile}
#done
