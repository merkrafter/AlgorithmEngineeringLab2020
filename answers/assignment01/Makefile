CC = g++
FLAGS = -fopenmp
ALL = $(subst .cpp,,$(wildcard *.cpp))

all: $(ALL)

%: %.cpp
	$(CC) $(FLAGS) $< -o $@

%_opt: %.cpp
	$(CC) $(FLAGS) -Ofast $< -o $@
