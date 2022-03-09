
SRCDIR = src
OBJDIR = obj
FLAGS  = -pthread 
MODULES := server client common
OBJDIRS := $(patsubst %, $(OBJDIR)/%, $(MODULES))

STRUCTURE 		:= $(shell find $(SRCDIR) -type d)
CODES 			:= $(addsuffix /*, $(STRUCTURE))
CODES			:= $(wildcard $(CODES))
SRCFILES 		:= $(filter %.cpp,$(CODES))
OBJFILES 		:= $(subst $(SRCDIR),$(OBJDIR), $(SRCFILES:%.cpp=%.o))

SERVEROBJ		:= $(filter $(OBJDIR)/server/%, $(OBJFILES))

all: client server

server2: servercompile
	g++ -o server $(SERVEROBJ) $(FLAGS)

servercompile: builddirs $(SERVEROBJ)

client: obj/common/socket.o obj/client/notificationmanagerclient.o obj/client/interface.o obj/client/mainclient.o 
	g++ -o client obj/client/mainclient.o obj/client/interface.o obj/client/notificationmanagerclient.o obj/common/socket.o -pthread -std=c++11

obj/client/notificationmanagerclient.o: builddirs src/client/notificationmanagerclient.cpp 
	g++ -c -o obj/client/notificationmanagerclient.o src/client/notificationmanagerclient.cpp -std=c++11

obj/common/socket.o: builddirs src/common/socket.cpp
	g++ -c -o obj/common/socket.o src/common/socket.cpp -std=c++11

obj/client/interface.o: builddirs src/client/interface.cpp
	g++ -c -o obj/client/interface.o src/client/interface.cpp -std=c++11

obj/client/mainclient.o: builddirs src/client/mainclient.cpp
	g++ -c -o obj/client/mainclient.o src/client/mainclient.cpp -pthread -std=c++11

server: obj/common/socket.o obj/server/notification.o obj/server/profile.o obj/server/profilemanager.o obj/server/mainserver.o obj/server/sessionManager.o
	g++ -o server obj/server/mainserver.o obj/server/profilemanager.o obj/server/notification.o obj/server/profile.o obj/server/sessionManager.o obj/common/socket.o -pthread -std=c++11

server/sessionManager.o: builddirs src/server/sessionManager.cpp 
	g++ -c -o obj/server/sessionManager.o src/server/sessionManager.cpp -pthread -std=c++11

obj/server/notification.o: builddirs src/server/notification.cpp 
	g++ -c -o obj/server/notification.o src/server/notification.cpp -std=c++11

obj/server/profile.o: builddirs src/server/profile.cpp
	g++ -c -o obj/server/profile.o src/server/profile.cpp -std=c++11

obj/server/profilemanager.o: builddirs src/server/profilemanager.cpp
	g++ -c -o obj/server/profilemanager.o src/server/profilemanager.cpp -std=c++11

obj/server/mainserver.o: builddirs src/server/mainserver.cpp
	g++ -c -o obj/server/mainserver.o src/server/mainserver.cpp -pthread -std=c++11

builddirs: $(OBJDIRS)

$(OBJDIRS):
	mkdir -p $@ 

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	g++ -c $< -o $@ $(FLAGS)

builddirs:
	mkdir -p $(subst $(SRCDIR),$(OBJDIR), $(STRUCTURE))

cleanserver:
	rm -r $(OBJDIR)/server server

clean:
	rm -r obj server client
