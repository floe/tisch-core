#!/bin/bash

if [ "$HOSTNAME" = "sivit" ] ; then
	v4lctl setinput Composite2
	v4lctl setnorm PAL
fi

pgrep -x touchd > /dev/null && exit

until pgrep -x touchd > /dev/null; do
	touchd -V "$@" &
	sleep 2
done

calibtool "$@"

killall touchd
