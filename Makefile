
OBJDIR = obj
MODULES := server client

OBJDIRS := $(patsubst %, $(OBJDIR)/%, $(MODULES))

all: client serverManagerTest

client: obj/client/notificationmanagerclient.o obj/client/interface.o obj/client/mainclient.o 
	g++ -o client obj/client/mainclient.o obj/client/interface.o obj/client/notificationmanagerclient.o -pthread

obj/client/notificationmanagerclient.o: builddirs src/client/notificationmanagerclient.cpp 
	g++ -c -o obj/client/notificationmanagerclient.o src/client/notificationmanagerclient.cpp

obj/client/interface.o: builddirs src/client/interface.cpp
	g++ -c -o obj/client/interface.o src/client/interface.cpp

obj/client/mainclient.o: builddirs src/client/mainclient.cpp
	g++ -c -o obj/client/mainclient.o src/client/mainclient.cpp -pthread

serverManagerTest: obj/server/notification.o obj/server/profile.o obj/server/profilemanager.o obj/server/mainserver.o 
	g++ -o server obj/server/mainserver.o obj/server/profilemanager.o obj/server/notification.o obj/server/profile.o -pthread

obj/server/notification.o: builddirs src/server/notification.cpp 
	g++ -c -o obj/server/notification.o src/server/notification.cpp

obj/server/profile.o: builddirs src/server/profile.cpp
	g++ -c -o obj/server/profile.o src/server/profile.cpp

obj/server/profilemanager.o: builddirs src/server/profilemanager.cpp
	g++ -c -o obj/server/profilemanager.o src/server/profilemanager.cpp

obj/server/mainserver.o: builddirs src/server/mainserver.cpp
	g++ -c -o obj/server/mainserver.o src/server/mainserver.cpp -pthread

builddirs: $(OBJDIRS)

$(OBJDIRS):
	mkdir -p $@ 

clean:
	rm obj client server -r

