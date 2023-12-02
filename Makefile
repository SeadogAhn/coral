#Directory declaration
MAINDIR = $(USER_HOME)
PGMDIR = $(MAINDIR)/bin
LIBDIR = $(MAINDIR)/lib
INCDIR = $(MAINDIR)/include

#library version
include LIBVERSION

#Compiler
CC  = gcc
CXX = g++
#compilers standard
CSTD   = -std=c1x
CXXSTD = -std=c++1y

#Excutable type
EXECUTE =
#Compiler optimize level option
ifeq ($(EXECUTE), DEBUG)
    DEBUGFLAG = -g -D_DEBUG -D__linux__ -D_GLIBCXX_USE_CXX11_ABI
else
    DEBUGFLAG = -O2 -D__linux__ -D_GLIBCXX_USE_CXX11_ABI
endif

#gprof Profiler option
PROFILE=
# if PROFILE is Y then add gnu profiler option
ifeq ($(PROFILE), Y)
    PROFOPT = -pg
else
    PROFOPT =
endif

# Library type
LIBTYPE = SHARED
# Compiler option
ifeq ($(LIBTYPE), STATIC)
    LINKER   = ar rs
    OFLAGS   = -Wall -W -c
else
    LINKER   = $(CXX) -shared -fPIC -Wall -W -Wno-unused-variable -Wno-unused-parameter $(CXXSTD) $(DEBUGFLAG) $(PROFOPT)
    OFLAGS   = -fPIC -Wall -W -Wno-unused-variable -Wno-unused-parameter -c
endif

# include & lib path
#IFLAGS   = -I. -I$(INCDIR) -I$(USER_HOME)/include -I$(ORACLE_INC) -I$(II_API_INC)
IFLAGS   = -I. -I$(INCDIR) -I$(USER_HOME)/include -I$(ORACLE_INC)
#LFLAGS   = -L. -L$(LIBDIR) -L$(USER_HOME)/lib -L$(ORACLE_LIB) -L$(II_API_LIB) -lpthread -lm -lc -lcrypt -ldl -lingres -lrt -lgcc_s -lclntsh -locci -lcurl
LFLAGS   = -L. -L$(LIBDIR) -L$(USER_HOME)/lib -L$(ORACLE_LIB) -lpthread -lm -lc -lclntsh -locci -lcurl

#orther option
REMOVE    = rm -rf
COPY      = cp -pPf
MKDIR     = mkdir -p -m 755
RMDIR     = rmdir

#Target Library Name
LIBNAME   = coral
ifeq ($(LIBTYPE), STATIC)
    EXE   = a
else
    EXE   = so
endif

#target name
ifeq ($(EXECUTE), DEBUG)
    TARGETNAME = lib$(LIBNAME)_dbg
else
    TARGETNAME = lib$(LIBNAME)
endif

#Target
ifeq ($(LIBTYPE), STATIC)
    TARGET   = $(TARGETNAME).$(EXE)
else
    TARGET   = $(TARGETNAME).$(EXE).$(MAJORLIBVER).$(MINORLIBVER).$(PATCHLIBVER)
    SUBTGT1  = $(TARGETNAME).$(EXE).$(MAJORLIBVER).$(MINORLIBVER)
    SUBTGT2  = $(TARGETNAME).$(EXE).$(MAJORLIBVER)
endif

#Source file list
SRCS  = config.cpp \
	defines.cpp \
	types.cpp \
	file_trans.cpp \
	utility.cpp \
	thread_lock.cpp \
	log_manager.cpp \
	ora_dbm.cpp \
	net_client.cpp \
	net_server.cpp

#Object file list
OBJS  = $(SRCS:.cpp=.o)

.c.o :
	$(CC) $(CSTD) $(DEBUGFLAG) $(PROFOPT) $(OFLAGS) $(IFLAGS) $<

.cpp.o:
	$(CXX) $(CXXSTD) $(DEBUGFLAG) $(PROFOPT) $(OFLAGS) $(IFLAGS) $<

all : $(OBJS)
	$(REMOVE) $(TARGETNAME)*
ifeq ($(LIBTYPE), STATIC)
	$(LINKER) $(TARGET) $(OBJS)
else
	$(LINKER) -o $(TARGET) $(OBJS) $(IFLAGS) $(LFLAGS)
endif

install : all
	$(MKDIR) $(INCDIR)/$(LIBNAME)
	$(COPY) *.h $(INCDIR)/$(LIBNAME)
	$(REMOVE) $(LIBDIR)/$(TARGETNAME).$(EXE)
ifeq ($(LIBTYPE), STATIC)
	$(COPY) $(TARGET) $(LIBDIR)
else
	$(REMOVE) $(LIBDIR)/$(SUBTGT2)
	$(REMOVE) $(LIBDIR)/$(SUBTGT1)
	$(REMOVE) $(LIBDIR)/$(TARGET)
	ln -sf ./$(TARGET) $(SUBTGT1)
	ln -sf ./$(SUBTGT1) $(SUBTGT2)
	ln -sf ./$(SUBTGT2) $(TARGETNAME).$(EXE)
	$(COPY) $(TARGETNAME)* $(LIBDIR)
endif

clean :
	$(REMOVE) core
	$(REMOVE) $(OBJS)
	$(REMOVE) $(TARGETNAME).$(EXE)*

uninstall : clean
	$(REMOVE) $(LIBDIR)/$(TARGETNAME)*
	$(REMOVE) $(INCDIR)/$(LIBNAME)/*.h
	$(RMDIR) $(INCDIR)/$(LIBNAME)
