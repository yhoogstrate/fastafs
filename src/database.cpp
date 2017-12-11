
#include <dirent.h>
#include <stdio.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <stdlib.h>



#include "database.hpp"

bool dir_exists(std::string &path) {
    //path = "/home/youri/.local/share/fastafs";
    bool isdir;
    
    printf(" [%s]\n",path.c_str());
    char *full_path = realpath(path.c_str(), NULL);
    if(full_path == NULL) {
        printf(" NULL \n");
    }
    else {
        printf(" %s\n", full_path);
    }
    free(full_path);
    
    DIR* dir = opendir(path.c_str());
    if (dir)
    {
        /* Directory exists. */
        closedir(dir);
        isdir = true;
    
    }
    else
    {
        
        throw std::runtime_error("database seem to exist already, but not as directory: " + path);
        isdir = false;
    }

    return isdir;
}



database::database() :
    path(std::string(getenv("HOME")) + "/.local/share/fastafs")
{
    this->load();
}

void database::load() {
    //if not dir exists,
    //  mkdir
    if(!dir_exists(this->path)) {
        printf("needs to make dirr!!!\n");
    }
    
}

void database::list() {
    std::cout << "FASTAFS NAME\t\tFASTFS ID\t\tIMAGE\t\t\tSEQUENCES\t\tBASES\t\tDISK SIZE" << std::endl;
}
