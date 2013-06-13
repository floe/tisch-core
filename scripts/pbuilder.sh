#!/bin/bash
RELEASE=$(lsb_release -cs)
sudo pbuilder create --distribution $RELEASE --othermirror "deb http://archive.ubuntu.com/ubuntu $RELEASE main restricted universe multiverse" --othermirror "deb http://ppa.launchpad.net/floe/libtisch/ubuntu $RELEASE main" --debootstrapopts --variant=buildd
