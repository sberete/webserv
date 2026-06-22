#ifndef PRINT_HPP
    #define PRINT_HPP

#include "webserv.hpp"

template <typename K, typename V>
void printMap(const std::map<K, V> &map)
{
    for (typename std::map<K, V>::const_iterator it = map.begin(); it != map.end(); ++it)
        std::cout << it->first << ": " << it->second << std::endl;
}

#endif
