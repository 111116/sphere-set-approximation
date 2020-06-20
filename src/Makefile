CXXFLAGS = -std=c++11 -I. -O3

sotv_test: sotv_test.cpp math/vecmath.cpp sotv.hpp sotv_ref.hpp
	$(CXX) $(CXXFLAGS) $< math/vecmath.cpp -o $@
