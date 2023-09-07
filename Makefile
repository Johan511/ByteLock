main: src/lock.cpp src/main.cpp
	g++ -std=c++17 -pthread  -O3 -o main src/lock.cpp src/main.cpp