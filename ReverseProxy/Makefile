CXX = g++

UNAME := $(shell uname)

CXXFLAGS = -std=c++17 -pthread -Iinclude
LDFLAGS =
LIBS = -lcrypto

ifeq ($(UNAME),Darwin)
    # macOS + Homebrew
    CXXFLAGS += -I$(shell brew --prefix openssl@3)/include
    LDFLAGS += -L$(shell brew --prefix openssl@3)/lib
endif

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
      src/TokenBucket.cpp \
      src/PerUserRateLimiter.cpp

TARGET = reverse_proxy

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)