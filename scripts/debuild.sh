#!/bin/bash

cd $(dirname $0)/../..

cd libtisch3-3.0.0
make clean
rm -r build

# first for precise
debuild -k11AF6AD8 -S -sa

# then for oneiric 
sed -i -e '1s/precise/oneiric/g' debian/changelog 
debuild -k11AF6AD8 -S -sa

# then for natty 
sed -i -e '1s/oneiric/natty/g' debian/changelog 
debuild -k11AF6AD8 -S -sa

# then for maverick
sed -i -e '1s/natty/maverick/g' debian/changelog 
debuild -k11AF6AD8 -S -sa

# then for lucid
sed -i -e '1s/maverick/lucid/g' debian/changelog 
debuild -k11AF6AD8 -S -sa

# undo
sed -i -e '1s/lucid/oneiric/g' debian/changelog 

