CC = g++-3.3
CPPFLAGS = -ggdb -I../sarl3 -Wall
LDFLAGS = -L../sarl3 -lsarl -lgc -lm

all: \
  packages_to_cnf package_analyser_dump_all \
  convert_to_bcnf package_analyser package_analyser_test package_solver

clean:
	rm -rf package_analyser *.o

%.o: %.cpp
	$(CC) -c -o $@ $< $(CPPFLAGS) 

package_analyser: package_analyser_main.o package_analyser.o
	$(CC) -o $@ $^ $(LDFLAGS)

package_analyser_test: package_analyser_test.o package_analyser.o
	$(CC) -o $@ $^ $(LDFLAGS)

package_solver: package_analyser_solver.o package_analyser.o
	$(CC) -o $@ $^ $(LDFLAGS)

package_analyser_dump_all: package_analyser_dump_all.o package_analyser.o
	$(CC) -o $@ $^ $(LDFLAGS)

packages_to_cnf: packages_to_cnf.o package_analyser.o
	$(CC) -o $@ $^ $(LDFLAGS)

convert_to_bncf: convert_to_bncf.o
	$(CC) -o $@ $^ $(LDFLAGS)

%.deps: %.cpp
	$(CC) $(CPPFLAGS) -M -o $@ $<

include package_analyser.deps 
include package_analyser_main.deps 
include package_analyser_test.deps
include package_analyser_solver.deps
