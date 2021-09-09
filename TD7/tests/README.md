# README for the tests directory

This directory contains two kind of tests: 
        * correct programs (files ok-*.toy) 
        * incorrect programs which cannot be compiled by the Toy compiler
      (files fail-*.toy)

To launch all the tests simply run `make tests`.

## Correct programs (ok-*.toy)

The compiler can produce an executable for these files. During its execution, the program can perform various tests. It must print the string `SUCCESS` or `FAILURE` depending of the result of its test(s). If a failure is detected (or if no success is found), the `make tests` is aborted. 

## Incorrect programs (fail-*.toy)

Incorrect programs are Toy programs which contains compilation errors. The number of errors expected for a given file must be present in a comment of the file under the following format:

        // ERRORS: n

if `n` errors are expected when compiling the file. The `make tests` command ensures that the correct number of errors are found when compiling this file and aborts if this not the case. 

