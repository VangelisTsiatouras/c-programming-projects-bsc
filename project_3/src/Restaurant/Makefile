######################################################################
OBJS 	= main.o restaurant_routines.o shared_memory_functions.o
SOURCE	= main.c restaurant_routines.c shared_memory_functions.c
HEADER  = handler.h restaurant_routines.h shared_memory_functions.h global_data_structures.h
OUT  	= restaurant
CC	= gcc
FLAGS   = -g -c 
# -g option enables debugging mode 
# -c flag generates object code for separate files

all: $(OBJS)
	cd ../Customer && make
	cd ../Doorman && make
	cd ../Waiter && make
	$(CC) -g $(OBJS) -pthread -o $(OUT)

# clean house
clean:
	cd ../Customer && make clean
	cd ../Doorman && make clean
	cd ../Waiter && make clean
	rm -f $(OBJS) $(OUT)

# do a bit of accounting
count:
	cd ../Customer && make count
	cd ../Doorman && make count
	cd ../Waiter && make count
	wc $(SOURCE) $(HEADER)
#########################################################################