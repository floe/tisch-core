#!/bin/bash

if [ "$HOSTNAME" = "sivit" ] ; then
	v4lctl setinput Composite2
	v4lctl setnorm PAL
fi

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
	( sleep 2 && echo -e 'region 1 0 0 2 scale 5 1 MultiBlobScale 0 31 0 0 rotate 5 1 MultiBlobRotation 0 31 0 0 \n bye 0' | nc -i 1 -w 1 -q 0 localhost 31410 ) &
	#( sleep 2 && echo -e 'region 1 0 0 1 scale 5 1 MultiBlobScale 0 31 0 0 \n bye 0' | nc -i 1 -w 1 -q 0 localhost 31410 ) &
	./gestured -v
done

