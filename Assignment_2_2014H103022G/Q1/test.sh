#!/bin/bash

make --silent
echo -e "1) 3 2\n2) 4 2\n3) 5 3\n4) 4 3\n5) Something else\n"
read a

if [ $a -eq 1 ]; then
	./prog 3 2
fi

if [ $a -eq 2 ]; then
	./prog 4 2
fi

if [ $a -eq 3 ]; then
	./prog 5 3
fi

if [ $a -eq 4 ]; then
	./prog 4 3
fi

if [ $a -eq 5 ]; then
	echo "Max level: "
	read ml
	echo "Max children: "
	read mc
	./prog $ml $mc
fi
