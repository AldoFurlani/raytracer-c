CC = gcc
CFLAGS = -Wall -std=c99
LIBS = -lm
BIN = bin

all: $(BIN)/raytracer $(BIN)/vector_tests
	rm -f *.o

# Executables

raytracer: $(BIN)/raytracer

$(BIN)/raytracer: assg_fs.o vector.o spheres.o color.o
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -DFS -o $@ assg_fs.o vector.o spheres.o color.o $(LIBS)

tests: $(BIN)/vector_tests

$(BIN)/vector_tests: assg_test.o vector.o spheres.o color.o
	@mkdir -p $(BIN)
	$(CC) $(CFLAGS) -DMS1 -o $@ assg_test.o vector.o spheres.o color.o $(LIBS)

# Object files

assg_fs.o: src/assg.c src/vector.h src/spheres.h src/color.h
	$(CC) $(CFLAGS) -DFS -c src/assg.c -o assg_fs.o

assg_test.o: src/assg.c src/vector.h src/spheres.h src/color.h
	$(CC) $(CFLAGS) -DMS1 -c src/assg.c -o assg_test.o

# Helper object files
vector.o: src/vector.c src/vector.h
	$(CC) $(CFLAGS) -c src/vector.c

spheres.o: src/spheres.c src/spheres.h
	$(CC) $(CFLAGS) -c src/spheres.c

color.o: src/color.c src/color.h
	$(CC) $(CFLAGS) -c src/color.c

# cleanup
clean:
	rm -rf $(BIN) *.o assg_fs.o assg_test.o