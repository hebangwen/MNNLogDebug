CFLAGS = -O2 -Wall -Wextra -Werror -std=c++11
LD_FLAGS = -lm -lOpenCL

all: log_debug

log_debug: log_debug.o
	g++ log_debug.o -o log_debug $(LD_FLAGS)

log_debug.o: log_debug.cpp
	g++ $(CFLAGS) -c log_debug.cpp -o log_debug.o $(LD_FLAGS)

clean:
	rm -f *.o log_debug

