######################################################################
OBJS 	= main.o ring.o
SOURCE	= main.c ring.c
HEADER  = ring.h
OUT  	= ring
CC	= gcc
FLAGS   = -g -c
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -lm -o $(OUT)

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
#########################################################################