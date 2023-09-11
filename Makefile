main: src/lock.cpp src/main.cpp
	g++ -Wall -lpthread -std=c++17 -O3 -o main src/lock.cpp src/main.cpp

clean:
	rm main
