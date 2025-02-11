#!/bin/bash

start=$(date +%s.%N)

echo -e "\e[35m"

echo "Compiling latest version..."
g++ main.cpp -o main -lpigpio -lrt

end=$(date +%s.%N)
elapsed=$(echo "$end - $start" | bc)

echo "Done compiling. Took $elapsed seconds."
echo "Starting program..."

echo -e "\e[0m"

sudo ./main