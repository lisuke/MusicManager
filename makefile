CC = g++
LIBS = -lsqlite3
objects = main.o Search.o Loader.o MusicItem.o
targetName = Main

$(targetName):$(objects)
	$(CC) -o $(targetName) $(objects)  $(LIBS) 

main.o:main.cpp 
	$(CC) -c main.cpp 

Loader.o:Loader.h Loader.cpp 
	$(CC) -c Loader.h Loader.cpp  $(LIBS) 

Search.o:Search.h Search.cpp 
	$(CC) -c Search.h Search.cpp 

MusicItem.o:MusicItem.h MusicItem.cpp
	$(CC) -c MusicItem.h MusicItem.cpp

clean:
	rm $(objects) *.gch