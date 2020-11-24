CC = g++
FLAGS = -fopenmp
ALL = $(subst .cpp,,$(wildcard *.cpp))

all: $(ALL)

%: %.cpp
	$(CC) $(FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -rf $(ALL)
