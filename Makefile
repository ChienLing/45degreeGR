CXX		=	g++ 
EXE		=   degGR.o
CXXFLAGS = -O3 -std=c++11

all: $(EXE)

$(EXE): main.o BLMR.o Parser.o Structure.o
	$(CXX) $^ $(CXXFLAGS) -o $@ 
	
$main.o: main.cpp  Structure.h Parser.h BLMR.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$BLMR.o: BLMR.cpp BLMR.h Structure.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
$Parser.o: Parser.cpp Parser.h BLMR.h Structure.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
$Structure.o: Structure.cpp Structure.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
clean:
	rm -rf *.gch *.o 