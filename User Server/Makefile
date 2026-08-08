CXX = g++
CXXFLAGS = -std=c++17 -pthread -Iinclude

SRC = src/main.cpp \
      src/Server.cpp \
      src/ThreadPool.cpp \
      src/TaskQueue.cpp \
      src/ClientHandler.cpp

TARGET = user_service

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)