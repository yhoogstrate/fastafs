
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include "database.hpp"

void database::force_db_exists() {
    DIR* dir = opendir(this->path.c_str());
    if (dir)
    {
        closedir(dir);
    }
    else
    {
        if(mkdir(this->path.c_str(), S_IRWXU ) == 0) {
        }
        else {
            throw std::runtime_error("could not access and not create database as directory: " + path);
        }
    }

    if(FILE *file = fopen(this->idx.c_str(), "r")) {
        fclose(file);
    } else {
        if(FILE *file2 = fopen(this->idx.c_str(), "w")) {
            fclose(file2);
        } else {
            throw std::runtime_error("could not access and not create database file: " + this->idx);
        }
    }
}



database::database() :
    path(std::string(getenv("HOME")) + "/.local/share/fastafs"),
    idx(std::string(getenv("HOME")) + "/.local/share/fastafs/index")
{
    this->load();
}

void database::load() {
    this->force_db_exists();
}

void database::list() {
    std::cout << "FASTAFS NAME\t\tFASTFS ID\t\tFASTAFS\t\tIMAGE\t\t\tSEQUENCES\t\tBASES\t\tDISK SIZE" << std::endl;
    //FASTAFS=v1_x32_2bit, version ,architechture (32 bit = max 4Gb files..., but can be elaborated to max 4gb per sequence line, then compression types, currently only 2bit)
}


// @todo return a filestream to a particular file one day?
std::string database::add(char* name) {
    std::ofstream outputFile;
    outputFile.open(this->idx);
    
    outputFile << name << std::endl;
    outputFile.close();
    
    return this->path + "/" + name;
}

