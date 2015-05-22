GCC=g++
CXXFLAGS=`root-config --libs --cflags` -O2 -fPIC -I../../
SOFLAGS=-shared

BARESRC=$(wildcard src/Bare*.cpp)
BAREOBJ=$(patsubst src/%.cpp, bin/%.o , $(BARESRC)  )


libBare.so: $(BAREOBJ) | bin
	$(GCC) $(CXXFLAGS) $(SOFLAGS) -o $@ $(BAREOBJ)

$(BAREOBJ) : bin/%.o : src/%.cpp interface/%.hpp | bin
	$(GCC) $(CXXFLAGS) -c -o bin/$*.o $<

bin:
	mkdir -p bin

.PHONY: clean
clean:
	-rm $(BAREOBJ)
	-rm libBare.so
