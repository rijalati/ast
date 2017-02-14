#!/bin/sh

for i in $(seq 1 65536); do
    echo "VAR$i="'"/a/çfoo/abãc"'
done
