#OBJS specifies which files to compile as part of the project
OBJS = src/test1.cpp

#CC specifies which compiler we're using
CC = g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I../../sdl2_devel_mingw/i686-w64-mingw32/include -I../../irrKlang/include

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS =  -L/usr/local/lib -L../../sdl2_devel_mingw/i686-w64-mingw32/lib -L../../irrKlang/lib/Win32-gcc -L../../sdl2_net_devel_mingw/i686-w64-mingw32/lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window ;  -Wl,-subsystem,windows
COMPILER_FLAGS = -w -Os -Wl,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -lSDL2 -lirrKlang -lSDL2_net

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = bin/AudioConnectFour

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(LINKER_FLAGS) $(COMPILER_FLAGS) -o $(OBJ_NAME)