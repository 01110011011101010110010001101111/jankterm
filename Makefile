CC = gcc
CFLAGS = -O2 -Wall $(shell pkg-config --cflags vte-2.91)
LDFLAGS = $(shell pkg-config --libs vte-2.91)

SRC = term.c
TARGET = term

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

# Ubuntu specific
install-vte:
	sudo apt-get install -y libvte-2.91-dev

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean install-vte run
