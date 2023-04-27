make:
	g++ -Werror -g -O3 -std=c++11 -o huff_codec huff_codec.cpp decode.cpp code.cpp
clean:
	rm huff_codec