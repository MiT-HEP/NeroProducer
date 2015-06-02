GCC=g++
CXXFLAGS=`root-config --libs --cflags` -O2 -fPIC -I../../ -D TREE_STREAMER
SOFLAGS=-shared

SRCDIR=src
BINDIR=bin
HPPDIR=interface

BARESRC=$(wildcard $(SRCDIR)/*.cpp)
BAREOBJ=$(patsubst $(SRCDIR)/%.cpp, $(BINDIR)/%.o , $(BARESRC)  )
BAREHPPLINKDEF=$(patsubst $(SRCDIR)/%.cpp, ../interface/%.hpp , $(BARESRC)  )



libBare.so: $(BAREOBJ) Dict | $(BINDIR)
	$(GCC) $(CXXFLAGS) $(SOFLAGS) -o $(BINDIR)/$@ $(BAREOBJ) $(BINDIR)/dict.o

$(BAREOBJ) : $(BINDIR)/%.o : $(SRCDIR)/%.cpp interface/%.hpp | $(BINDIR)
	$(GCC) $(CXXFLAGS) -c -o $(BINDIR)/$*.o $<

.PHONY: Dict
Dict: $(BINDIR)/dict.o

$(BINDIR)/dict.o:
	cd $(BINDIR) && rootcint -v4 -f dict.cc -c -I../../.. $(BAREHPPLINKDEF)  ../interface/BareLinkDef.hpp 
	cd $(BINDIR) && $(GCC) -c -o dict.o $(CXXFLAGS) -I../../../ dict.cc
$(BINDIR):
	mkdir -p $(BINDIR)

.PHONY: clean
clean:
	-rm $(BAREOBJ)
	-rm $(BINDIR)/dict*
	-rm $(BINDIR)/libBare.so
	-rmdir $(BINDIR)
