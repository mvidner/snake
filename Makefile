SRCS = \
    snake.cc \
    debug_display.cc \
    vt100_display.cc \
    vt100_input.cc

CXXFLAGS = -Wall -g
CPPFLAGS = -MMD
OBJS = $(SRCS:.cc=.o)
snake: $(OBJS)
	g++ -o $@ $^

DEPS = $(SRCS:.cc=.d)

all: snake
clean:
	rm -f $(OBJS)
	rm -f snake

-include $(DEPS)
