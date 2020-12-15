# dragino lora testing
# Single lora testing app

CC=g++
CFLAGS=-c -Wall
LIBS=-lwiringPi

all: dragino_lora_app

dragino_lora_app: main.o
	$(CC) main.o LM80_MT3339.o $(LIBS) -o dragino_lora_app

main.o: main.c
	$(CC) $(CFLAGS) main.c ./dragino_gps/LM80_MT3339.c

#lm80_mt3339.o: ./dragino_gps/LM80_MT3339.c
#	$(CC) $(CFLAGS) ./dragino_gps/LM80_MT3339.c


clean:
	rm ./dragino_gps/*.o *.o dragino_lora_app	
