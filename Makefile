main: src/lock.cpp src/main.cpp
	g++ -Wall -std=c++17 -g -o main src/lock.cpp src/main.cpp

clean:
	rm main