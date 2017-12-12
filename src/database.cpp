
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <string>

#include "database.hpp"
#include "fastafs.hpp"

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
    std::cout << "FASTAFS NAME\t\tFASTFS ID\t\tFASTAFS\t\tSEQUENCES\t\tBASES\t\tDISK SIZE" << std::endl;

    
    std::ifstream infile(this->idx);
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);

        std::string fname = this->path + "/" + line + ".fastafs";

        fastafs f = fastafs();
        f.load(fname);
        
        std::ifstream file (fname, std::ios::in|std::ios::binary|std::ios::ate);
        unsigned int size = (unsigned int) file.tellg();
        file.close();
        
        printf("%-24s%-24s%-16s%-24u%-16u%u\n", 
            line.c_str(),
            std::string("uid").c_str(),
            std::string("v0-x32-2bit").c_str(),// version ,architechture (32 bit = max 4Gb files..., but can be elaborated to max 4gb per sequence line, then compression types, currently only 2bit)
            (unsigned int) f.data.size(),
            f.n(),
            size
            );
    }
}


// @todo return a filestream to a particular file one day?
std::string database::add(char* name) {
    std::ofstream outputFile;
    outputFile.open(this->idx);
    
    outputFile << name << std::endl;
    outputFile.close();
    
    return this->path + "/" + name + ".fastafs";
}



/**
 * @brief searches for a filename that corresponds to the uid
 */
std::string database::get(std::string fastafs_name_or_id) {
    std::string fname;
    
    std::ifstream infile(this->idx);
    std::string line;
    while (std::getline(infile, line))
    {
        std::istringstream iss(line);

        if(line.compare(fastafs_name_or_id) == 0) {
            fname = this->path + "/" + line + ".fastafs";
        }
    }
    
    return fname;
}

