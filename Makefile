CXX = g++
src = $(wildcard *.cpp)
deps = $(wildcard *.h)
obj = $(src:.cpp=.o)

%.o: %.cpp $(deps)
	$(CXX) -c -o $@ $<
	
Project: $(obj) 
	$(CXX) -o $@ $^
	
clean:
	rm -f $(obj) Project