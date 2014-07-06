CC=gcc
CFLAGS=-g -Wall -D_GNU_SOURCE=1 -Dmain=SDL_main -std=c99
INCLUDE=-I/MinGW/include/SDL -I/MinGW/include/GL
LIBS=-L/MinGW/lib -lmingw32 -lSDLmain -lSDL -mwindows -lopengl32 -lglu32
LDFLAGS=
SRCS=mandelbrot.c
OBJS=$(SRCS:.c=.o)
MAIN=mandelbrot


.PHONY: depend clean

all:    $(MAIN)
	@echo $(MAIN) has been compiled

$(MAIN): $(OBJS) 
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
