#!/bin/bash

# add `-O2` for release build
lesson_num=6 # in [1...9]
printf "–– compiling and running lesson #${lesson_num}:\n\n"
#g++ -std=c++11 -Wall -Wextra ./main_lesson_${lesson_num}.cpp && time ./a.out
g++ -std=c++17 -Wall -Wextra ./lesson_${lesson_num}.cpp && ./a.out
