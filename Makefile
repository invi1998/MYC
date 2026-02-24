CXX := g++
CXXFLAGS := -std=c++23 -O2 -Wall -Wextra
LDFLAGS :=

TARGET := main.exe
SRCS := main.cpp solution1/Complex.cpp
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -Isolution1 -c $< -o $@

clean:
	del /Q $(OBJS) $(TARGET) 2>NUL
