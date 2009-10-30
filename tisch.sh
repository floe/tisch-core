#!/bin/bash

cd $(dirname "$0")
PROJECT=$(pwd)/build/

export LD_LIBRARY_PATH=${PROJECT}/lib/
cd ${PROJECT}/bin/

pgrep -x touchd > /dev/null && exit

until pgrep -x touchd > /dev/null; do
	./touchd "$@" &
	sleep 2
done

./calibd &

ulimit -c unlimited

while true ; do
	sleep 1
	( sleep 2 && echo 'region 1 0 0 2 scale 5 1 MultiBlobScale 0 31 0 0 rotate 5 1 MultiBlobRotation 0 31 0 0' | nc -u localhost 31410 ) &
	./gestured 
done

