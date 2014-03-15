SRCS = \
    snake.cc \
    debug_display.cc \
    vt100_display.cc \
    vt100_input.cc

CXXFLAGS = -Wall
CPPFLAGS = -MMD
snake: $(SRCS:.cc=.o)
	g++ -o $@ $^

DEPS = $(SRCS:.cc=.d)

-include $(DEPS)
