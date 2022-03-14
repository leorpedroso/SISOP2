OBJDIR = obj
SRCDIR = src
INCDIR = include
FLAGS  = -pthread -std=c++11

MODULES := server client common
OBJDIRS := $(patsubst %, $(OBJDIR)/%, $(MODULES)) data

STRUCTURE 	:= $(shell find $(SRCDIR) -type d)
CODES 		:= $(addsuffix /*, $(STRUCTURE))
CODES		:= $(wildcard $(CODES))
SRCFILES 	:= $(filter %.cpp,$(CODES))
OBJFILES 	:= $(subst $(SRCDIR),$(OBJDIR), $(SRCFILES:%.cpp=%.o))

SERVEROBJ	:= $(filter $(OBJDIR)/server/%, $(OBJFILES))
CLIENTOBJ	:= $(filter $(OBJDIR)/client/%, $(OBJFILES))
COMMONOBJ	:= $(filter $(OBJDIR)/common/%, $(OBJFILES))

all: client server

client: commoncompile clientcompile 
	g++ -o client $(CLIENTOBJ) $(COMMONOBJ) $(FLAGS)

server: commoncompile servercompile
	g++ -o server $(SERVEROBJ) $(COMMONOBJ) $(FLAGS)

clientcompile: builddirs $(CLIENTOBJ)

servercompile: builddirs $(SERVEROBJ)

commoncompile: builddirs $(COMMONOBJ)

builddirs: $(OBJDIRS)

$(OBJDIRS):
	mkdir -p $@ 

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	g++ -c $< -o $@ $(FLAGS)

clean:
	rm -r obj server client
