SRCDIR = src
OBJDIR = obj
FLAGS  = -pthread 

STRUCTURE 		:= $(shell find $(SRCDIR) -type d)
CODES 			:= $(addsuffix /*, $(STRUCTURE))
CODES			:= $(wildcard $(CODES))
SRCFILES 		:= $(filter %.cpp,$(CODES))
OBJFILES 		:= $(subst $(SRCDIR),$(OBJDIR), $(SRCFILES:%.cpp=%.o))

SERVEROBJ		:= $(filter $(OBJDIR)/server/%, $(OBJFILES))

all: client server

server: servercompile
	g++ -o server $(SERVEROBJ) $(FLAGS)

servercompile: builddirs $(SERVEROBJ)


client: obj/notificationmanagerclient.o obj/interface.o obj/mainclient.o 
	g++ -o client obj/mainclient.o obj/interface.o obj/notificationmanagerclient.o -pthread

obj/notificationmanagerclient.o: src/notificationmanagerclient.cpp 
	g++ -c -o obj/notificationmanagerclient.o src/notificationmanagerclient.cpp

obj/interface.o: src/interface.cpp
	g++ -c -o obj/interface.o src/interface.cpp

obj/mainclient.o: src/mainclient.cpp
	g++ -c -o obj/mainclient.o src/mainclient.cpp -pthread


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	g++ -c $< -o $@ $(FLAGS)

builddirs:
	mkdir -p $(subst $(SRCDIR),$(OBJDIR), $(STRUCTURE))

cleanserver:
	rm -r $(OBJDIR)/server server

clean:
	rm -r obj server client
