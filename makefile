# Variables
CXX = g++
CXXFLAGS = -Wall -std=c++11
TARGET = cc
OBJS = main.o shell.o cd.o echo.o pwd.o history.o search.o ls.o pinfo.o

# Default target
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

# Compiling main
main.o: main.cpp shell.h
	$(CXX) $(CXXFLAGS) -c main.cpp

# Compiling shell
shell.o: shell.cpp shell.h cd.h
	$(CXX) $(CXXFLAGS) -c shell.cpp

# Compiling cd
cd.o: cd.cpp cd.h
	$(CXX) $(CXXFLAGS) -c cd.cpp

# Compiling echo
echo.o: echo.cpp echo.h
	$(CXX) $(CXXFLAGS) -c echo.cpp

# Compiling pwd
pwd.o: pwd.cpp pwd.h
	$(CXX) $(CXXFLAGS) -c pwd.cpp

# Compiling history
history.o: history.cpp history.h
	$(CXX) $(CXXFLAGS) -c history.cpp

# Compiling search
search.o: search.cpp search.h
	$(CXX) $(CXXFLAGS) -c search.cpp

# Compiling ls
ls.o: ls.cpp ls.h
	$(CXX) $(CXXFLAGS) -c ls.cpp

# Compiling pinfo
pinfo.o: pinfo.cpp pinfo.h
	$(CXX) $(CXXFLAGS) -c pinfo.cpp

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)
