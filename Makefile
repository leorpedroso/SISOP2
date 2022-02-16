


all: client

client: obj/notificationmanagerclient.o obj/interface.o obj/mainclient.o 
	g++ -o client obj/mainclient.o obj/interface.o obj/notificationmanagerclient.o -pthread

obj/notificationmanagerclient.o: src/notificationmanagerclient.cpp 
	g++ -c -o obj/notificationmanagerclient.o src/notificationmanagerclient.cpp

obj/interface.o: src/interface.cpp
	g++ -c -o obj/interface.o src/interface.cpp

obj/mainclient.o: src/mainclient.cpp
	g++ -c -o obj/mainclient.o src/mainclient.cpp -pthread

clean:
	rm obj/* client

