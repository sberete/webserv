#ifndef PRINT_HPP
    #define PRINT_HPP

#include "webserv.hpp"

template <typename K, typename V>
void printMap(const std::map<K, V> &map)
{
    for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
}

template <typename T>
void printVector(const std::vector<T> &values)
{
    for (typename std::vector<T>::const_iterator it = values.begin(); it != values.end(); ++it)
        std::cout << *it << std::endl;
}

#endif
