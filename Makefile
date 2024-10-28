CC = g++
CFLAGS = -std=c++17


SOURCES = main.cpp functions.cpp
OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE = myshell

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXECUTABLE)

main.o: main.cpp header.hpp
	$(CC) $(CFLAGS) -c main.cpp

functions.o: functions.cpp header.hpp
	$(CC) $(CFLAGS) -c functions.cpp

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

