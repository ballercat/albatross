SHELL = /bin/sh
cc = g++

FLAGS			=
CCFLAGS			= -m64 -pedantic -Wall -Wextra -MMD -MP -MF
CCFLAGSLAX		= -m64 -Wall -MMD -MP -MF
DEBUGFLAGS		= -O0 -D _DEBUG -ggdb3 -g -p
RELEASEFLAGS	= -02 -D -combine -fwhole-program \
				  -mfpmath=sse -ffast-math -ftree-vectorize
#include paths
CLIENTPATH		= src/client
GRAPHICSPATH	= src/graphics
GUIPATH			= src/gui
MAPPATH			= src/map
MAPMAKER		= src/mapmaker
NETWORKPATH		= src/network
OBJECTSPATH		= src/objects
PHYSICSPATH		= src/physics
SERVERPATH		= src/server
UTILITYPATH		= src/utility
GAMEPATH		= src/game
INCLUDE_RAW		= $(CLIENTPATH) $(GRAPHICSPATH) $(GUIPATH) $(MAPPATH)
INCLUDE_RAW		+= $(NETWORKPATH) $(OBJECTSPATH) $(PHYSICSPATH) $(SERVERPATH)
INCLUDE_RAW		+= $(UTILITYPATH) $(GAMEPATH)
INCLUDE			= $(foreach d, $(INCLUDE_RAW), -I$d)

#library paths


TARGET	= albatross_client
MMTARGET = mapmaker
SOURCES = main.cpp
SOURCES	+= $(shell echo src/game/*.cpp src/client/*.cpp src/graphics/*.cpp \
			src/gui/*.cpp src/map/*.cpp src/network/*.cpp src/objects/*.cpp \
			src/physics/*.cpp )
HEADERS = $(shell echo src/game/*.h src/client/*.h src/graphics/*.h src/gui/*.h \
				src/map/*.h src/network/*.h src/objects/*.h src/physics/*.h \
				src/server/*.h src/utility/*.h)
#Map Maker
MMSOURCES = $(shell echo src/graphics/fixed_pipeline.cpp src/graphics/assets.cpp \
			 src/graphics/sprite.cpp src/mapmaker/*.cpp src/map/*.cpp src/gui/*.cpp)
				
COMMON	= -Icommon
OBJECTS = $(SOURCES:.cpp=.o)
#libraries
LIBGLPATH	= usr/lib
LIBSFML = -lsfml-graphics -lsfml-window -lsfml-network -lsfml-system
LIBCHIPMUNK = -lchipmunk
LIBGL	= -lGLEW -lGL
LIBS	= -lm -lstdc++ $(LIBCHIPMUNK) $(LIBSFML) $(LIBGL)
MMLIBS	= -lm -lstdc++ $(LIBSFML) $(LIBGL)

PREFIX = $(DESTDIR)/usr/local
BINDIR = $(PREFIX)/bin


all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(FLAGS) $(CCFLAGS) $(DEBUGFLAGS) $(INCLUDE) $(COMMON) -o $(TARGET) $(LIBS) $(OBJECTS)

Debug: $(TARGET)
release: $(SOURCE) $(HEADERS)
	$(CC) $(FLAGS) $(CCFLAGS) $(RELEASEFLAGS) $(INCLUDE) $(COMMON) -o $(TARGET) $(LIBS) $(SOURCES)

mapmaker: $(MMSOURCES) $(HEADERS)
	$(CC) $(FLAGS) $(CCFLAGSLAX) $(DEBUGFLAGS) $(INCLUDE) $(COMMON) -o $(MMTARGET) $(MMLIBS) $(MMSOURCES)

profile: CFLAGS += -pg
profile: $(TARGET)


install: release
	install -D $(TARGET) $(BINDIR)/$(TARGET)

install-strip: release
	install -D -s $(TARGET) $(BINDIR)/$(TARGET)

uninstall:
	-rm $(BINDIR)/$(TARGET)


clean:
	-rm -f $(OBJECTS)
	-rm -f gmon.out

cleanDebug: clean

distclean: clean
	-rm -f $(TARGET)

.SECONDEXPANSION:

%.o : %.cpp $(HEADERS)
	$(CC) $(FLAGS) $(CFLAGS) $(DEBUGFLAGS) $(INCLUDE) $(COMMON) -L$(LIBGLPATH) -c -o $@ $<

.PHONY: all profile release \
	install install-strip uninstall clean distclean


