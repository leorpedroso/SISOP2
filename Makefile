


all: client server

client: obj/notificationmanagerclient.o obj/interface.o obj/mainclient.o 
	g++ -o client obj/mainclient.o obj/interface.o obj/notificationmanagerclient.o -pthread

obj/notificationmanagerclient.o: src/notificationmanagerclient.cpp 
	g++ -c -o obj/notificationmanagerclient.o src/notificationmanagerclient.cpp

obj/interface.o: src/interface.cpp
	g++ -c -o obj/interface.o src/interface.cpp

obj/mainclient.o: src/mainclient.cpp
	g++ -c -o obj/mainclient.o src/mainclient.cpp -pthread

server: obj/notificationmanagerserver.o obj/profilemanager.o obj/mainserver.o 
	g++ -o server obj/mainserver.o obj/profilemanager.o obj/notificationmanagerserver.o -pthread

obj/notificationmanagerserver.o: src/notificationmanagerserver.cpp 
	g++ -c -o obj/notificationmanagerserver.o src/notificationmanagerserver.cpp

obj/profilemanager.o: src/profilemanager.cpp
	g++ -c -o obj/profilemanager.o src/profilemanager.cpp

obj/mainserver.o: src/mainserver.cpp
	g++ -c -o obj/mainserver.o src/mainserver.cpp -pthread

clean:
	rm obj/* client

