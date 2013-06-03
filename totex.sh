#!/bin/bash -e
for x in $(find | grep \.json);
do
    y=$(basename $x .json);
    z=$(dirname $x);
    nx=$z/$y.tex
    echo $nx
    ./tex_table.py < $x > $nx
done
