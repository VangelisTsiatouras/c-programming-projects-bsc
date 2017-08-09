######################################################################################################
OBJS 	= main.o auxiliary_functions.o board_client.o board_server.o
SOURCE	= main.c auxiliary_functions.c board_client.c board_server.c
HEADER  = auxiliary_functions.h board_channels.h board_client.h board_server.h channel.h definitions.h
OUT  	= Board
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
######################################################################################################