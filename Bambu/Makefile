INC = -I$(CMSSW_BASE)/src -I$(CMSSW_RELEASE_BASE)/src -I$(shell root-config --incdir)
ROOTLIBS = $(shell root-config --libs)
DEPENDENCIES = source/dependencies

LIBDIR = $(CMSSW_BASE)/lib/$(SCRAM_ARCH)
TMPDIR = $(CMSSW_BASE)/tmp/$(SCRAM_ARCH)/src/NeroProducer/Bambu

CXX           = g++
CXXFLAGS      = -O2 -Wall -fPIC -std=c++11
LD            = g++
LDFLAGS       = -O2 -L$(LIBDIR)
SOFLAGS       = -shared

LINKDEF = source/LinkDef.h
DICT = source/MitNeroLinkDefDict.cc
PCM = $(patsubst source/%.cc,%_rdict.pcm,$(DICT))
DICTOBJ = $(patsubst source/%.cc,$(TMPDIR)/%.o,$(DICT))
DICTHDR = interface/classes.h

LIBRARY = libMitNero.so
SRC = $(filter-out $(DICT),$(wildcard source/*.cc))
HDRS = $(patsubst source/%.cc,interface/%.h,$(SRC))
OBJS = $(patsubst source/%.cc,$(TMPDIR)/%.o,$(SRC))

main: $(LIBDIR)/$(LIBRARY) $(LIBDIR)/$(PCM)

$(LIBDIR)/$(LIBRARY): $(TMPDIR)/$(LIBRARY)
	cp $(TMPDIR)/$(LIBRARY) $(LIBDIR)

$(TMPDIR)/$(LIBRARY): $(OBJS) $(DICTOBJ) $(DEPENDENCIES)
	$(LD) $(LDFLAGS) $(SOFLAGS) -o $(TMPDIR)/$(LIBRARY) $(shell cat $(DEPENDENCIES)) $(ROOTLIBS) $(OBJS) $(DICTOBJ)

$(LIBDIR)/$(PCM): $(TMPDIR)/$(PCM)
	cp $(TMPDIR)/$(PCM) $(LIBDIR)

$(TMPDIR)/$(PCM): $(DICT)

$(DICTOBJ): $(DICT)
	mkdir -p $(TMPDIR) 2> /dev/null
	$(CXX) $(CXXFLAGS) $(INC) -o $@ -c $<

$(DICT): $(LINKDEF) $(DICTHDR)
	rootcling -f $(DICT) -I$(CMSSW_BASE)/src NeroProducer/Bambu/$(DICTHDR) $(LINKDEF)
	mv source/$(PCM) $(TMPDIR)/$(PCM)

$(TMPDIR)/%.o: source/%.cc interface/%.h
	mkdir -p $(TMPDIR) 2> /dev/null
	$(CXX) $(CXXFLAGS) $(INC) -o $@ -c $<

.PHONY: clean

clean:
	rm -f $(TMPDIR)/* $(DICT)
