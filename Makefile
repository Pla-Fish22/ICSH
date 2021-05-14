#
# 
# 
CC = gcc
CFLAGS = -pedantic -Wall 
LIBS = -lm

all: icsh

icsh: icsh.c
	$(CC) $(CFLAGS) -o icsh icsh.c

clean:
	$(RM) icsh
code: 
	code icsh.c
