CC := g++
CFLAGS := -O0 -std=c++17 -Wall -g -DDEBUG
TARGET := main

# $(wildcard *.cc /xxx/xxx/*.cc): get all .cc files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard *.cc)
# $(patsubst %.cc,%.o,$(SRCS)): substitute all ".cc" file name strings to ".o" file name strings
OBJS := $(patsubst %.cc,%.o,$(SRCS))

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $@ $^
%.o: %.cc
	$(CC) $(CFLAGS) -c $<
clean:
	rm -rf $(TARGET) *.o
	
.PHONY: all clean