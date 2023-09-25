main: src/lock.cpp src/main.cpp
	g++ -Wall -lpthread -std=c++20 -O3 -o main src/lock.cpp src/main.cpp

debug: src/lock.cpp src/main.cpp
	g++ -Wall -lpthread -std=c++20 -g -fsanitize=address -o main src/lock.cpp src/main.cpp

clean:
	rm main
