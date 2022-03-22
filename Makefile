CXX		=	g++ 
EXE		=   degGR.o
CXXFLAGS = -O3 -std=c++11

all: $(EXE)

$(EXE): Structure.o BLMR.o Parser.o Func.o IO_interface.o main.o 
	$(CXX) $^ $(CXXFLAGS) -o $@ 
	
$main.o: main.cpp  
	$(CXX) $(CXXFLAGS) -c $< -o $@

$BLMR.o: BLMR.cpp BLMR.h Func.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
$Parser.o: Parser.cpp Parser.h 
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
$Structure.o: Structure.cpp Structure.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
$Func.o: Func.cpp Func.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

$IO_interface.o: IO_interface.cpp IO_interface.h
	$(CXX) $(CXXFLAGS) -c $< -o $@
	
clean:
	rm -rf *.gch *.o 