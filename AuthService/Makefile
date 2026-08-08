CXX = g++

CXXFLAGS = -std=c++17 -pthread -Iinclude

LIBS = -lpqxx -lpq -largon2 -lcrypto

SRC = src/main.cpp \
      src/Server.cpp \
      src/ThreadPool.cpp \
      src/TaskQueue.cpp \
      src/ClientHandler.cpp \
      src/Jwt.cpp \
      src/Database.cpp \
      src/PasswordHasher.cpp

TARGET = auth_service

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)