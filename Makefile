CC = gcc
CFLAGS = -std=c99 -g -Wall -I. -D_POSIX_C_SOURCE=200809L
TARGET = language-model
ANALYSER = analyser
MODELER = language-modeler
MAIN = main
DEPS = language-model.h alphabet.h cipher.h candidates.h key.h util.h analyse.h
OBJ = language-model.o alphabet.o cipher.o candidates.o key.o util.o analyse.o

all: $(MAIN) $(MODELER) $(ANALYSER)

$(MAIN): $(MAIN).c $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(MODELER): $(MODELER).c $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

$(ANALYSER): $(ANALYSER).c $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) $(TARGET) *.o
