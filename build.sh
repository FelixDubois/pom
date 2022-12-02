#!/bin/bash

# Build the project
# Add cmath librairie
gcc -Wall -Wextra -o ./build/output ./src/main.cpp -lm 

# Run the project
./build/output