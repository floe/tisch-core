#!/bin/bash
for FILE in $(find .. -name '*.h' -or -name '*.cc') ; do
	sed -f addlicense.sed -i "$FILE"
done

