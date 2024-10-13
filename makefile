CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

TARGET = server
OBJS = main.o server.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

main.o: main.cpp server.hpp
	$(CXX) $(CXXFLAGS) -c main.cpp

server.o: server.cpp server.hpp
	$(CXX) $(CXXFLAGS) -c server.cpp

clean:
	rm -f $(OBJS) $(TARGET)

fclean: clean

re: fclean all
