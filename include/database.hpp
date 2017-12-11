

#include "config.hpp"

#include <iostream>
#include <array>
#include <vector>
#include <string.h>


class database
{
    std::string path;// current default: ~/.local/share/fastafs/
    //hash_map<string, unsigned int> idx;// "test": path + "/" + tostr(i) + ".fastafs"

public:
    database();
    void load();// reads path + "/" + info.txt, only containing N
    void list();// 'ls'
};
