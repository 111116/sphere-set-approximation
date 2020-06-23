#pragma once

#include <algorithm>
#include <vector>
#include <utility>

// returns iterator of the max element according to key
// https://stackoverflow.com/a/14200316/7884249
template<typename Container, typename Fn>
auto argmax(Container& c, Fn&& key) -> decltype(std::begin(c))
{  
    if ( std::begin(c) == std::end(c) ) 
       throw std::invalid_argument("empty container is not allowed.");

    typedef decltype(*std::begin(c)) V;
    auto cmp = [&](V a, V b){ return key(a) < key(b); };
    return std::max_element(std::begin(c), std::end(c), cmp);
}

template<typename Container>
auto argmax(Container& c) -> decltype(std::begin(c))
{  
    if ( std::begin(c) == std::end(c) ) 
       throw std::invalid_argument("empty container is not allowed.");
    return std::max_element(std::begin(c), std::end(c));
}

template<typename T>
std::vector<T> concat(const std::vector<T>& a, const std::vector<T>& b)
{
    std::vector<T> ab = a;
    ab.insert(ab.end(), b.begin(), b.end());
    return ab;
}

template<typename T>
T average(const std::vector<T>& a)
{
	if (a.empty()) throw "cannot average empty container";
	T sum = 0;
	for (T x: a)
		sum += x;
	return sum / a.size();
}