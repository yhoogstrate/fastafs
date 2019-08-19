#include <stdio.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <filesystem>


/*
g++ -std=c++17 -o lsfastafs src/lsfastafs.cpp
*/

void get_fastafs_processes()
{
    // parse /proc/mounts
    // find those that run fastafs
    // from those, read the dict file [/../../basename/basename.dict]
    // in there, parse the line containing UR://[/...fastafs]

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
        fprintf(stdout, "could not open /proc/mounts - are you sure this is running on linux?\n");
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

            printf("possible fastafs mount at : %s\n", dict_fn.c_str());

            fclose(f);
        }
    } while(match != EOF);

    fclose(f);


}

int main(int argc, char *argv[])
{
    get_fastafs_processes();

    return 0;
}
