make:
	g++ -Werror -g -O2 -std=c++11 -o huff_codec huff_codec.cpp decode.cpp code.cpp code_adapt.cpp decode_adapt.cpp
clean:
	rm huff_codec