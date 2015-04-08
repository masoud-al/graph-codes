CXX = g++ 
#CXXFLAGS += -fopenmp
CXXFLAGS +=  -g -std=c++0x 
CXXFLAGS +=  -pg -Wall  -pedantic $(CFLAGS)
#CXXFLAGS +=  -O2 -funroll-loops  -mtune=native -march=native $(CFLAGS)
CXXFLAGS += -D__STDC_LIMIT_MACROS 
#CXXFLAGS += -DUSE_BOOST_POOL
#CXXFLAGS += -DUSE_PRERAND
CXXFLAGS += -DUSE_RANDOMLIB
#CXXFLAGS += -DUSE_BOOST_RAND
CXXFLAGS += -I$(HOME)/include/m4ri -I$(HOME)/include/m4rie
#LDFLAGS+= -lRandom -lm4ri -lm4rie -lgivaro -lgmp -lgomp
LDFLAGS+= -lRandom -lm4ri -lm4rie -lgivaro -lgmp 
LDFLAGS+= -litpp 
#LDFLAGS+= -static 
#LDFLAGS+= -lgomp -ltcmalloc



OBJ_EBCTEST = utils.o ebc.o graph.o main.o 
OBJ_SIMCODES = utils.o ebc.o graph.o simcodes.o getopt_pp.o PreRandom.o
OBJ_GPCRATE = utils.o graph.o ebc.o gpcrate.o
OBJ_RSGEN = rsgen.o
OBJ_ALLRATES = utils.o graph.o allrates.o ebc.o
OBJ_SIMALL = simall.o ebc.o graph.o
OBJ_ENUM = enum.o
OBJ_GLDPCSIM = sse_mathfun.o utils.o graph.o getopt_pp.o channel.o gldpcsim.o gldpc.o
OBJ_EXTENDEDPEELING =  utils.o graph.o getopt_pp.o  dists.o extendedpeeling.o
OBJ_SIMCOVERAGE =  utils.o getopt_pp.o  simcoverage.o

all: ebctest simcodes gpcrate rsgen allrates enum gldpcsim extendedpeeling  simcoverage


#$(OBJ_EBCTEST): Makefile ebc.h
#$(OBJ_SIMCODES): Makefile ebc.h



ebctest: $(OBJ_EBCTEST)
	$(CXX) -o $@ $(OBJ_EBCTEST) $(LDFLAGS)

simcodes:  $(OBJ_SIMCODES)
	$(CXX)  -o $@ $(OBJ_SIMCODES) $(LDFLAGS) 
	
gldpcsim: $(OBJ_GLDPCSIM)	
	$(CXX)  -o $@ $(OBJ_GLDPCSIM) $(LDFLAGS) 
	
gpcrate: $(OBJ_GPCRATE)
	$(CXX) -o $@ $(OBJ_GPCRATE) $(LDFLAGS)

rsgen: $(OBJ_RSGEN)
	$(CXX) -o $@ $(OBJ_RSGEN) $(LDFLAGS)

allrates: $(OBJ_ALLRATES)
	$(CXX) -o $@ $(OBJ_ALLRATES) $(LDFLAGS)

enum: $(OBJ_ENUM)
	$(CXX) -o $@ $(OBJ_ENUM) $(LDFLAGS)


simall: $(OBJ_SIMALL)
	$(CXX) -o $@ $(OBJ_SIMALL) $(LDFLAGS)
	
extendedpeeling: $(OBJ_EXTENDEDPEELING)
	$(CXX) -o $@ $(OBJ_EXTENDEDPEELING) $(LDFLAGS)
	

simcoverage: $(OBJ_SIMCOVERAGE)
	$(CXX) -o $@ $(OBJ_SIMCOVERAGE) $(LDFLAGS)

clean:
	-rm -f ebctest $(OBJ_EBCTEST) simcodes $(OBJ_SIMCODES) rsgen $(OBJ_RSGEN) gpcrate $(OBJ_GPCRATE) allrates $(OBJ_ALLRATES) enum $(OBJ_ENUM) simall $(OBJ_SIMALL)  triform $(OBJ_EXTENDEDPEELING)
# DO NOT DELETE
