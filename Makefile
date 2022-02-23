all: client server

server: obj/server/test_class.o obj/server/connection_manager.o obj/server/session_manager.o obj/server/maintest.o
	g++ -o server obj/server/test_class.o obj/server/connection_manager.o obj/server/session_manager.o obj/server/maintest.o -pthread

obj/server/test_class.o: src/server/test_class.cpp
	g++ -c -o obj/server/test_class.o src/server/test_class.cpp 

obj/server/connection_manager.o: src/server/profile_management/connection_manager.cpp
	g++ -c -o obj/server/connection_manager.o src/server/profile_management/connection_manager.cpp -pthread

obj/server/session_manager.o: src/server/profile_management/session_manager.cpp
	g++ -c -o obj/server/session_manager.o src/server/profile_management/session_manager.cpp -pthread

obj/server/maintest.o: src/server/maintest.cpp
	g++ -c -o obj/server/maintest.o src/server/maintest.cpp


client: obj/notificationmanagerclient.o obj/interface.o obj/mainclient.o 
	g++ -o client obj/mainclient.o obj/interface.o obj/notificationmanagerclient.o -pthread

obj/notificationmanagerclient.o: src/notificationmanagerclient.cpp 
	g++ -c -o obj/notificationmanagerclient.o src/notificationmanagerclient.cpp

obj/interface.o: src/interface.cpp
	g++ -c -o obj/interface.o src/interface.cpp

obj/mainclient.o: src/mainclient.cpp
	g++ -c -o obj/mainclient.o src/mainclient.cpp -pthread


cleanserver:
	rm obj/server/* server

clean:
	rm obj/* client
