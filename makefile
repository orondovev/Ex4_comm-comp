CC = gcc
FLAGS= -Wall -g
SS = strace

all: sender Measure

add_sender:
	./sender

add_measure:
	./Measure

sender: sender.c
	$(CC) $(FLAGS) -o sender sender.c

Measure: Measure.c
	$(CC) $(FLAGS) -o Measure Measure.c

clean:
	rm -f *.o sender Measure file_send

s_strace:
	($SS) -f ./sender

M_strace:
	($SS) -f ./Measure