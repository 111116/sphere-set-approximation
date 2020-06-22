#pragma once

#include <algorithm>
#include <utility>

// https://stackoverflow.com/a/14200316/7884249
template<typename Container, typename Fn>
auto argmax(const Container& c, Fn&& key) -> decltype(std::begin(c))
{  
    if ( std::begin(c) == std::end(c) ) 
       throw std::invalid_argument("empty container is not allowed.");

    typedef decltype(*std::begin(c)) V;
    auto cmp = [&](V a, V b){ return key(a) < key(b); };
    return std::max_element(std::begin(c), std::end(c), cmp);
}

// template<typename Container, typename Fn>
// auto valmax(const Container& c, Fn&& f) -> decltype(f(*std::begin(c)))
// {  
//     if ( std::begin(c) == std::end(c) ) 
//        throw std::invalid_argument("empty container is not allowed.");

//     decltype(f(*std::begin(c))) m = f(*std::begin(c));
//     for (auto e: c)
//         m = std::max(m, f(e));
//     return m;
// }

template<typename Container, typename Fn>
auto map(const Container& c, Fn&& f) -> std::vector<decltype(Fn(*std::begin(c)))> 
{
    std::vector<decltype(Fn(*std::begin(c)))> a;
    for (auto e: c)
        a.push_back(f(e));
    return a;
}