CURRENT_PATH=$(dirname $(readlink -f $0))

socIP=$(ifconfig | grep "inet" | head -n 1 | awk '{print $2}')
socIP=${socIP#*:}

mkdir -p ${CURRENT_PATH}/result
TimeStamp=$(date '+%Y%m%d_%H%M%S')
ResultLog=${CURRENT_PATH}/result/top_${socIP}_${TimeStamp}.txt

top -o %CPU -b -c >> ${ResultLog} &