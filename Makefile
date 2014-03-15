SRCS = \
    snake.cc \
    debug_display.cc \
    vt100_display.cc

CXXFLAGS = -Wall
CPPFLAGS = -MMD
snake: snake.o debug_display.o vt100_display.o
	g++ -o $@ $^

DEPS = $(SRCS:.cc=.d)

-include $(DEPS)
