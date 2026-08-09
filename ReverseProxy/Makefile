CXX = g++

CXXFLAGS = -std=c++17 -pthread -Iinclude

SRC = src/main.cpp \
      src/Server.cpp \
      src/ThreadPool.cpp \
      src/TaskQueue.cpp \
      src/ClientHandler.cpp \
      src/Router.cpp \
      src/ServerPool.cpp \
      src/RoundRobin.cpp \
      src/LeastConnections.cpp \
      src/LeastResponseTime.cpp \
      src/WeightedRoundRobin.cpp \
      src/HealthChecker.cpp \
      src/JwtVerifier.cpp \
      src/SlidingWindow.cpp \
      src/TokenBucket.cpp

TARGET = reverse_proxy

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) -lcrypto

clean:
	rm -f $(TARGET)