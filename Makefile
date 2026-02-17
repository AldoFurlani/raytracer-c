
all: MS1 MS2 FS
	rm -f *.o
	
# Executables
MS1: assg_ms1 vector.o spheres.o color.o
	gcc -Wall -std=c99 -DMS1 -o MS1_assg assg.o vector.o spheres.o color.o -lm
	
MS2: assg_ms2 vector.o spheres.o color.o
	gcc -Wall -std=c99 -DMS2 -o MS2_assg assg.o vector.o spheres.o color.o -lm

FS: assg_fs vector.o spheres.o color.o
	gcc -Wall -std=c99 -DFS -o FS_assg assg.o vector.o spheres.o color.o -lm

# Object files
assg_fs: src/assg.c src/vector.h src/spheres.h src/color.h
	gcc -Wall -std=c99 -DFS -c src/assg.c

assg_ms2: src/assg.c src/vector.h src/spheres.h src/color.h
	gcc -Wall -std=c99 -DMS2 -c src/assg.c

assg_ms1: src/assg.c src/vector.h src/spheres.h src/color.h
	gcc -Wall -std=c99 -DMS1 -c src/assg.c

vector.o: src/vector.c src/vector.h
	gcc -Wall -std=c99 -c src/vector.c

spheres.o: src/spheres.c src/spheres.h
	gcc -Wall -std=c99 -c src/spheres.c

color.o: src/color.c src/color.h
	gcc -Wall -std=c99 -c src/color.c

clean:
	rm -f *.o MS1_assg MS2_assg FS_assg

