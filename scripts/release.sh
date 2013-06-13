#!/bin/bash

cd $(dirname $0)/../..

RELEASE=libtisch3-3.0.0
rm ${RELEASE}
ln -s tisch-core.git ${RELEASE}

cd ${RELEASE}
make clean
cd ..

zip -r ${RELEASE}.zip ${RELEASE}/ -x "${RELEASE}/debian/*" "${RELEASE}/.pc/*" "${RELEASE}/build/*" "${RELEASE}/.git/*"
tar -hczvf ${RELEASE}.tar.gz --exclude-vcs --exclude=${RELEASE}/debian --exclude=${RELEASE}/build --exclude=${RELEASE}/.pc ${RELEASE}

cp ${RELEASE}.tar.gz ${RELEASE/-/_}.orig.tar.gz

