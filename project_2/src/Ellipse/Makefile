######################################################################
OBJS 	= main.o ellipse.o
SOURCE	= main.c ellipse.c
HEADER  = ellipse.h
OUT  	= ellipse
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