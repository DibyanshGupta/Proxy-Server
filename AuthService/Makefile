UNAME := $(shell uname)

CXX = g++
CXXFLAGS = -std=c++17 -pthread -Iinclude

ifeq ($(UNAME),Darwin)

CXXFLAGS += \
    -I$(shell brew --prefix openssl@3)/include \
    -I$(shell brew --prefix libpqxx)/include \
    -I$(shell brew --prefix libpq)/include \
    -I$(shell brew --prefix argon2)/include

LDFLAGS += \
    -L$(shell brew --prefix openssl@3)/lib \
    -L$(shell brew --prefix libpqxx)/lib \
    -L$(shell brew --prefix libpq)/lib \
    -L$(shell brew --prefix argon2)/lib

endif

LIBS = -lpqxx -lpq -largon2 -lcrypto

all:
	$(CXX) $(CXXFLAGS) \
	src/main.cpp \
	src/Server.cpp \
	src/ThreadPool.cpp \
	src/TaskQueue.cpp \
	src/ClientHandler.cpp \
	src/Jwt.cpp \
	src/Database.cpp \
	src/PasswordHasher.cpp \
	-o auth_service \
	$(LDFLAGS) \
	$(LIBS)