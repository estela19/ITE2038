.SUFFIXES: .cpp .o

CXX=g++

SRCDIR=src/
INC=include/
LIBS=lib/

# main source file
TARGET_SRC:=$(SRCDIR)main.cpp
TARGET_OBJ:=$(SRCDIR)main.o
STATIC_LIB:=$(LIBS)libbpt.a

# Include more files if you write another source file.
#SRCS_FOR_LIB:=$(wildcard $(SRCDIR)*.cpp)
SRCS_FOR_LIB:=$(SRCDIR)bpt.cpp $(SRCDIR)buffer.cpp $(SRCDIR)db.cpp $(SRCDIR)dbapi.cpp $(SRCDIR)fileAPI.cpp $(SRCDIR)page.cpp $(SRCDIR)lock_manager.cpp $(SRCDIR)trxAPI.cpp
OBJS_FOR_LIB:=$(SRCS_FOR_LIB:.cpp=.o)

CFLAGS+= -g -fPIC -I $(INC) -std=c++17

TARGET=main

all: $(TARGET)


$(TARGET): $(TARGET_OBJ) $(STATIC_LIB)
	# make static_library
	$(CXX) $(CFLAGS) -pthread -o $@ $< -L $(LIBS) -lbpt

$(SRCDIR)%.o: $(SRCDIR)%.cpp
	$(CXX) $(CFLAGS) -o $@ -c $^

clean:
	rm $(TARGET) $(TARGET_OBJ) $(OBJS_FOR_LIB) $(LIBS)*

$(STATIC_LIB): $(OBJS_FOR_LIB)
	ar cr $@ $^

