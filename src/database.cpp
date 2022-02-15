
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "database.hpp"
#include "fastafs.hpp"
#include "lsfastafs.hpp"


    
const std::string database::get_default_dir()
{
    const char* home_c = getenv("HOME");
    if(home_c == nullptr)
    {
        struct passwd *pw = getpwuid(getuid());
        home_c = pw->pw_dir;

        if(home_c == nullptr)
        {
            throw std::runtime_error("Could not deterimine home dir. Also, no $HOME environment variable is set.");
        }
    }
    std::string home_s = std::string(home_c);
    return home_s + "/.local/share/fastafs";
}



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



database::database(const std::string &path_arg) :
    path(path_arg),
    idx(path_arg + "/index")
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
    std::string version;
    
    while(std::getline(infile, line)) {
        std::string fname = this->path + "/" + line + ".fastafs";
        bool zstd_seek = false;
        
        if(!file_exist(fname.c_str())) {
            fname = this->path + "/" + line + ".fastafs.zst";
            zstd_seek = true;
        }

        if(file_exist(fname.c_str())) {
            fastafs f = fastafs(line);
            f.load(fname);

            uint32_t size;
            {
                std::ifstream file(fname, std::ios::in | std::ios::binary | std::ios::ate);
                size = (uint32_t) file.tellg();
                file.close();
            }


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

            if(zstd_seek) {
                version = "v0-x32+Z";
            } else {
                version = "v0-x32";
            }

            printf("%-16s%-16s%-16u%-16u%-16u%-8.1f%s\n",//double %% escapes the
                   line.c_str(),
                   version.c_str(),// version ,architechture (32 bit = max 4Gb files..., but can be elaborated to max 4gb per sequence line, then compression types, currently only 2bit)
                   (uint32_t) f.data.size(),
                   f.n(),
                   size,
                   (float) 100.0 * (float) size / (float) f.fasta_filesize(50), // @todo fastafs file size!
                   mountpoints.c_str()
                  );
        } else {
            // print error invalid file?
        }
    }
}




// @todo return a filestream to a particular file one day?
std::string database::add(char *name)
{
    if(this->get(name) != "") 
    {
        throw std::runtime_error("Trying to add duplicate entry to database.");
    }
    
    std::ofstream outputFile;

    outputFile.open(this->idx, std::fstream::app);
    outputFile << name << std::endl;
    outputFile.close();

    return this->path + "/" + name + ".fastafs";
}



/**
 * @brief searches for a filename that corresponds to the uid
 */
std::string database::get(char *fastafs_name_or_id)
{
    std::string fname = "";
    std::ifstream infile(this->idx);
    std::string line;

    while(std::getline(infile, line, '\n')) {
        if(line.compare(fastafs_name_or_id) == 0) {
            fname = this->path + "/" + line + ".fastafs";
            
            if(!file_exist(fname.c_str())) {
                fname = this->path + "/" + line + ".fastafs.zst";
            }

        }
    }

    return fname;
}

