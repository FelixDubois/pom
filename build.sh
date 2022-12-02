#!/bin/bash

# Build the project
# Add cmath librairie
gcc -Wall -Wextra -o ./build/pom ./src/main.cpp -lm -lpng

# Run the project
./build/pom