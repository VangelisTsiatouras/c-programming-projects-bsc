######################################################################
OBJS 	= main.o customer_routines.o
SOURCE	= main.c customer_routines.c
HEADER  = customer_routines.h global_data_structures.h
OUT  	= customer
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	$(CC) -g $(OBJS) -pthread -o $(OUT)

# clean house
clean:
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	wc $(SOURCE) $(HEADER)
#########################################################################