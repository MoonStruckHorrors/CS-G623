#! /bin/bash

make --silent

echo -e "1) 6x4\n2) 111x111\n3) 999x999\n"
read a

if [ $a -eq 1 ]; then
	./prog 6 4
fi

if [ $a -eq 2 ]; then
	./prog 111 111
fi

if [ $a -eq 3 ]; then
	./prog 999 999
fi
