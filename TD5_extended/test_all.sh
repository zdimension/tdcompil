#!/usr/bin/env bash
rm Resources/*.am
for x in Resources/*.in
do
  echo $x
  export M=$(basename ${x%.*})
  (
    ./cmake-build-debug/turing Resources/$M.in -O > Resources/$M.am \
    && cd ../../Automatic-Machines-Simulator/ \
    && python.exe -m am sim ../tdcompil/TD5_extended/Resources/$M.am -r
  )
done