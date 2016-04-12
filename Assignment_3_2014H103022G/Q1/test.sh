#! /bin/bash

make --silent

echo -e "1) in1\n2) in2\n3) in3\n4) in4\n"
read a

if [ $a -eq 1 ]; then
	./prog in1
fi

if [ $a -eq 2 ]; then
	./prog in2
fi

if [ $a -eq 3 ]; then
	./prog in3
fi

if [ $a -eq 4 ]; then
	./prog in4
fi
