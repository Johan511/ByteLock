main: src/lock2.cpp src/main.cpp
	g++ -Wall -lpthread -std=c++20 -O3 -o main src/lock2.cpp src/main.cpp

debug: src/lock2.cpp src/main.cpp
	g++ -Wall -lpthread -std=c++20 -g -o main src/lock2.cpp src/main.cpp

clean:
	rm main
