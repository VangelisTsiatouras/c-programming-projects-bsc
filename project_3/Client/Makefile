######################################################################
OBJS 	= main.o client.o
SOURCE	= main.c client.c graph.c hashtable.c
HEADER  = client.h
OUT  	= bankclient
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT)

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
#########################################################################