#include <filesystem>
#include <string.h>
#include <sys/xattr.h>

#include <unordered_map>
#include <vector>

#include "config.hpp"


/*
g++ -std=c++17 -o lsfastafs src/lsfastafs.cpp
*/


/*
 * What this function does:

 * parses /proc/mounts
 * find those that run fastafs
 * from those, request the xattr 'fastafs-file' :) propasgated by the fuse code
*/
std::unordered_multimap<std::string, std::string > get_fastafs_processes()
{
    std::unordered_multimap<std::string, std::string > out = {  };// this structure should be able to have multiple values per key [https://stackoverflow.com/questions/45142799/same-key-multiple-entries-for-stdunordered-map]

    char xattr_fastafs[256];

    const char *arg = "fastafs";

    FILE *f;
    char mount_dev[256];
    char mount_dir[256];
    char mount_type[256];
    char mount_opts[256];
    int mount_freq;
    int mount_passno;

    int match;

    f = fopen("/proc/mounts", "r");

    if(!f) {
        fprintf(stdout, "Could not open /proc/mounts - are you sure this is running on linux?\n");
    }
    do {

        match = fscanf(f, "%255s %255s %255s %255s %d %d\n", mount_dev, mount_dir, mount_type, mount_opts, &mount_freq, &mount_passno);
        mount_dev[255] = 0;
        mount_dir[255] = 0;
        mount_type[255] = 0;
        mount_opts[255] = 0;

        if(match == 6 &&
           (strstr(mount_dev, arg) != NULL ||
            strstr(mount_dir, arg) != NULL)) {

            std::string fn = std::string(mount_dir);
            std::string basename = std::filesystem::path(fn).filename();
            std::string dict_fn = std::string(mount_dir)  + "/" +  basename +  ".dict";

            if(getxattr(mount_dir, FASTAFS_FILE_XATTR_NAME.c_str(), xattr_fastafs, 255) != -1) {
                out.insert({std::string(xattr_fastafs), std::string(mount_dir)});
            }
        }
    } while(match != EOF);

    fclose(f);

    return out;
}


