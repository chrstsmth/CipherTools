CC = gcc
CFLAGS = -std=c99 -g -Wall -I.
TARGET = language-model
MODELER = language-modeler
DEPS = language-model.h
OBJ = language-model.o

$(MODELER): $(MODELER).c $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	$(RM) $(TARGET)
