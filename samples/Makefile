WO_CONCEPTS = eq functor constrained show show_unshowable default super 
WITH_CONCEPTS = concept show_concept super_concept constrained_concept
NAMES = ${WO_CONCEPTS} ${WITH_CONCEPTS}

TC_HEADER = ../tc.hpp
FLAGS = -std=c++14
CXX = g++

CONCEPT_HEADER = ../tc_concept.hpp
CONCEPT_FLAGS = -std=c++20
CONCEPT_CXX = clang++

## by default we build only programs not using concepts
wo_concepts: ${WO_CONCEPTS}  

## use make all to build all the programs
all: ${NAMES}

${WO_CONCEPTS}: %: %.cpp ${TC_HEADER}
	${CXX} ${FLAGS} $@.cpp -o $@

${WITH_CONCEPTS}: %: %.cpp ${TC_HEADER} ${CONCEPT_HEADER}
	${CONCEPT_CXX} ${CONCEPT_FLAGS} $@.cpp -o $@

clean:
	rm -vf ${NAMES}

.PHONY: clean

