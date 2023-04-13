make:
	g++ -g -O3 -std=c++11 -o huff_codec huff_codec.cpp
clean:
	rm huff_codec