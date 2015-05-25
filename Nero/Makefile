GCC=g++
CXXFLAGS=`root-config --libs --cflags` -O2 -fPIC -I../../
SOFLAGS=-shared

BARESRC=$(wildcard src/Bare*.cpp)
BAREOBJ=$(patsubst src/%.cpp, bin/%.o , $(BARESRC)  )
BAREHPPLINKDEF=$(patsubst src/%.cpp, ../interface/%.hpp , $(BARESRC)  )


libBare.so: $(BAREOBJ) Dict | bin
	$(GCC) $(CXXFLAGS) $(SOFLAGS) -o bin/$@ $(BAREOBJ) bin/dict.o

$(BAREOBJ) : bin/%.o : src/%.cpp interface/%.hpp | bin
	$(GCC) $(CXXFLAGS) -c -o bin/$*.o $<

.PHONY: Dict
Dict: bin/dict.o
bin/dict.o:
	cd bin && rootcint -v4 -f dict.cc -c $(BAREHPPLINKDEF) ../interface/BareLinkDef.hpp
	cd bin && $(GCC) -c -o dict.o $(CXXFLAGS) -I../../../ dict.cc
bin:
	mkdir -p bin

.PHONY: clean
clean:
	-rm $(BAREOBJ)
	-rm libBare.so
