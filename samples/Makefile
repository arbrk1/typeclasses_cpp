NAMES = eq functor constrained show default
HEADER = ../tc.hpp
FLAGS = -std=c++14

all: ${NAMES}

eq: eq.cpp ${HEADER}
	g++ ${FLAGS} eq.cpp -o eq

functor: functor.cpp ${HEADER}
	g++ ${FLAGS} functor.cpp -o functor

constrained: constrained.cpp ${HEADER}
	g++ ${FLAGS} constrained.cpp -o constrained

show: show.cpp ${HEADER}
	g++ ${FLAGS} show.cpp -o show

default: default.cpp ${HEADER}
	g++ ${FLAGS} default.cpp -o default

clean:
	rm ${NAMES}

.PHONY: clean

