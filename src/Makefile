CXXFLAGS = -std=c++17 -I. -O3

main: main.cpp *.hpp */*.hpp
	$(CXX) $(CXXFLAGS) $< rtcore/aabox.cpp math/vecmath.cpp -o $@

#sotv_test: sotv_test.cpp math/vecmath.cpp sotv.hpp sotv_ref.hpp
#	$(CXX) $(CXXFLAGS) $< math/vecmath.cpp -o $@
