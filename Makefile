main: src/lock.cpp src/main.cpp
	g++ -Wall -std=c++17 -O3 -o main src/lock.cpp src/main.cpp

clean:
	rm main