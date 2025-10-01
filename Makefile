TARGET = bin/dbview
SRC = $(wildcard src/*.c)
OBJ = $(patsubst src/%.c, obj/%.o, $(SRC))

run: clean default
	./$(TARGET) -n -f db.meow -a "Mogger Jr., 443 true stress.,6789"
	./$(TARGET) -f db.meow -a "BOGGER Max., 1337 false street.,6000"
	./$(TARGET) -f db.meow -a "";
	./$(TARGET) -f db.meow ;

all: run

default: $(TARGET)

clean:
	rm -f obj/*.o
	rm -f bin/*
	rm -f *.db
	rm -f *.meow

$(TARGET): $(OBJ)
	gcc -fsanitize=address -fsanitize=undefined -o $@ $?

obj/%.o: src/%.c
	gcc -fsanitize=address -fsanitize=undefined -c $< -o $@ -Iinclude