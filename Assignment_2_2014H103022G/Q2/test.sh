#!/bin/bash

make --silent

echo -e "Select a sample input.\n1)Custom arg printing program.\n2)Echo commands printing <file_number> <line_number>\n"
read a

if [ $a -eq 1 ]; then
	./prog SampleInput1/0.txt SampleInput1/1.txt SampleInput1/2.txt SampleInput1/3.txt SampleInput1/4.txt SampleInput1/5.txt SampleInput1/6.txt
fi

if [ $a -eq 2 ]; then
	./prog SampleInput2/0.txt SampleInput2/1.txt SampleInput2/2.txt SampleInput2/3.txt SampleInput2/4.txt SampleInput2/5.txt SampleInput2/6.txt SampleInput2/7.txt
fi
