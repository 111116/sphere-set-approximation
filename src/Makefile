CXXFLAGS = -std=c++17 -I. -O3

main: main.cpp *.hpp */*.hpp lib/argparse.o
	$(CXX) $(CXXFLAGS) $< rtcore/aabox.cpp math/vecmath.cpp lib/argparse.o -o $@

lib/argparse.o: lib/argparse.c
	$(CC) $(CFLAGS) $< -c -o $@

#sotv_test: sotv_test.cpp math/vecmath.cpp sotv.hpp sotv_ref.hpp
#	$(CXX) $(CXXFLAGS) $< math/vecmath.cpp -o $@
