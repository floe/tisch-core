#!/bin/bash
cd $(dirname $0)/../build/bin
export LD_LIBRARY_PATH=$(pwd)/../lib/

touch mypipe

gnome-terminal -e '/bin/bash -c "./gispl -m > mypipe"' &
gnome-terminal -e 'tail -f mypipe' 

