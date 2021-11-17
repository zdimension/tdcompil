#!/usr/bin/env bash
rm Resources/*.am
for x in Resources/*.td5
do
  echo $x
  export M=$(basename ${x%.*})
  (
    ./cmake-build-debug/turing Resources/$M.td5 -O > Resources/$M.am \
    && cd ../../Automatic-Machines-Simulator/ \
    && python.exe -m am sim ../tdcompil/TD5_extended/Resources/$M.am -r
  )
done