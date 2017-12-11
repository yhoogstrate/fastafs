
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

#include "database.hpp"

void force_db_exists(std::string &path) {
    DIR* dir = opendir(path.c_str());
    if (dir)
    {
        closedir(dir);
    
    }
    else
    {
        if(mkdir(path.c_str(), S_IRWXU ) == 0) {
        }
        else {
            throw std::runtime_error("could not access and not create database as directory: " + path);
        }
    }

    std::string db_file = path + "/index";

    if(FILE *file = fopen(db_file.c_str(), "r")) {
        fclose(file);
    } else {
        if(FILE *file2 = fopen(db_file.c_str(), "w")) {
            fclose(file2);
        } else {
            throw std::runtime_error("could not access and not create database file: " + db_file);
        }
    }
}



database::database() :
    path(std::string(getenv("HOME")) + "/.local/share/fastafs")
{
    this->load();
}

void database::load() {
    force_db_exists(this->path);
    
}

void database::list() {
    std::cout << "FASTAFS NAME\t\tFASTFS ID\t\tFASTAFS\t\tIMAGE\t\t\tSEQUENCES\t\tBASES\t\tDISK SIZE" << std::endl;
    //FASTAFS=v1_x32_2bit, version ,architechture (32 bit = max 4Gb files..., but can be elaborated to max 4gb per sequence line, then compression types, currently only 2bit)
}
