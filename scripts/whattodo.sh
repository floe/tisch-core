#!/bin/bash
cd $(dirname $0)/..
egrep -r --exclude-dir=.git --exclude-dir=build --exclude=whattodo.sh "TODO|FIXME" .
