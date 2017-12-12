

#include "config.hpp"

#include <iostream>
#include <array>
#include <vector>
#include <string.h>


class database
{
    std::string path;
    std::string idx;// current default: ~/.local/share/fastafs/
    //hash_map<string, unsigned int> idx;// "test": path + "/" + tostr(i) + ".fastafs"

public:
    database();

    void force_db_exists();
    std::string add(char* );
    void load();// reads path + "/" + info.txt, only containing N
    void list();// 'ls'
    std::string get(std::string);
};
