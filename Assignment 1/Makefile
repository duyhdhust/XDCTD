CC = gcc

CFLAGS = -Wall

TARGET = indexer

SRC = indexer.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)