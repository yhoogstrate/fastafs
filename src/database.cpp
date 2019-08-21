
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <string>

#include "database.hpp"
#include "fastafs.hpp"
#include "lsfastafs.hpp"

void database::force_db_exists()
{
    DIR *dir = opendir(this->path.c_str());
    if(dir) {
        closedir(dir);
    } else {
        if(mkdir(this->path.c_str(), S_IRWXU) == 0) {
        } else {
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

void database::load()
{
    this->force_db_exists();
}

void database::list()
{
    std::unordered_multimap<std::string, std::pair<std::string, std::string> > fastafs_fuse_mounts = get_fastafs_processes();

    std::cout << "FASTAFS NAME\tFASTAFS\t\tSEQUENCES\tBASES\t\tDISK SIZE\tCOMPR-%\tMOUNT POINT(S)" << std::endl;
    std::ifstream infile(this->idx);
    std::string line;
    while(std::getline(infile, line)) {
        //std::istringstream iss(line);
        std::string fname = this->path + "/" + line + ".fastafs";

        fastafs f = fastafs(line);
        f.load(fname);

        std::ifstream file(fname, std::ios::in | std::ios::binary | std::ios::ate);
        uint32_t size = (uint32_t) file.tellg();
        file.close();


        std::string mountpoints = "-";
        size_t n_mountpoints = fastafs_fuse_mounts.count(fname);

        if(n_mountpoints > 0) {
            mountpoints == "";
            bool is_first = true;

            auto it = fastafs_fuse_mounts.find(fname);
            for(; it != fastafs_fuse_mounts.end() ; it++) {
                if(is_first) {
                    mountpoints = it->second.second;
                    is_first = false;
                } else {
                    mountpoints = mountpoints + "," + it->second.second;
                }
            }
        }


        printf("%-16s%-16s%-16u%-16u%-16u%-8.1f%s\n",//double %% escapes the
               line.c_str(),
               std::string("v0-x32-2bit").c_str(),// version ,architechture (32 bit = max 4Gb files..., but can be elaborated to max 4gb per sequence line, then compression types, currently only 2bit)
               (uint32_t) f.data.size(),
               f.n(),
               size,
               (float) 100.0 * (float) size / (float) f.fasta_filesize(50), // @todo fastafs file size!
               mountpoints.c_str()
              );
    }
}


// @todo return a filestream to a particular file one day?
std::string database::add(char *name)
{
    std::ofstream outputFile;
    outputFile.open(this->idx, std::fstream::app);
    outputFile << name << std::endl;
    outputFile.close();
    return this->path + "/" + name + ".fastafs";
}



/**
 * @brief searches for a filename that corresponds to the uid
 */
std::string database::get(std::string fastafs_name_or_id)
{
    std::string fname;
    std::ifstream infile(this->idx);
    std::string line;
    while(std::getline(infile, line)) {
        //std::istringstream iss(line);
        if(line.compare(fastafs_name_or_id) == 0) {
            fname = this->path + "/" + line + ".fastafs";
        }
    }
    return fname;
}

