
#define FUSE_USE_VERSION 30

#include <dirent.h>
#include <fstream>
#include <fuse.h>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
//#include <filesystem>


#include "fuse.hpp"
#include "database.hpp"
#include "fastafs.hpp"
#include "ucsc2bit.hpp"


// http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
// more examples: https://libfuse.github.io/doxygen/hello_8c.html



struct fuse_instance {
    //fastasfs
    fastafs *f;
    ffs2f_init *cache;
    ffs2f_init *cache_p0;// cache with padding of 0; used by API '/seq/chr1:123:456'

    bool from_fastafs; // if false, from 2bit

    // ucsc2bit
    ucsc2bit *u2b;

    // generic
    uint32_t padding;
    int argc_fuse;
};



static int do_getattr(const char *path, struct stat *st)
{
    fuse_instance *ffi = static_cast<fuse_instance *>(fuse_get_context()->private_data);

    char cur_time[100];
    time_t now = time(0);
    strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime(&now));

    // GNU's definitions of the attributes (http://www.gnu.org/software/libc/manual/html_node/Attribute-Meanings.html):
    // 		st_uid: 	The user ID of the file’s owner.
    //		st_gid: 	The group ID of the file.
    //		st_atime: 	This is the last access time for the file.
    //		st_mtime: 	This is the time of the last modification to the padding of the file.
    //		st_mode: 	Specifies the mode of the file. This includes file type information (see Testing File Type) and the file permission bits (see Permission Bits).
    //		st_nlink: 	The number of hard links to the file. This count keeps track of how many directories have entries for this file. If the count is ever decremented to zero, then the file itself is discarded as soon
    //						as no process still holds it open. Symbolic links are not counted in the total.
    //		st_size:	This specifies the size of a regular file in bytes. For files that are really devices this field isn’t usually meaningful. For symbolic links this specifies the length of the file name the link refers to.
    st->st_uid = getuid(); // The owner of the file/directory is the user who mounted the filesystem
    st->st_gid = getgid(); // The group of the file/directory is the same as the group of the user who mounted the filesystem
    st->st_atime = time(NULL); // The last "a"ccess of the file/directory is right now
    st->st_mtime = time(NULL); // The last "m"odification of the file/directory is right now

    printf("[%s]\n" , path);
    if(strcmp(path, "/") == 0) {
        //st->st_mode = S_IFREG | 0644;
        //st->st_nlink = 1;
        //st->st_size = 1024;
        //directory
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
    } else if(strlen(path) == 4 && strncmp(path, "/seq", 4) == 0) {
        //directory
        printf("setting to DIR because /seq\n");
        st->st_mode = S_IFDIR | 0755;
        st->st_nlink = 1;
    } else if(strlen(path) > 4 &&  strncmp(path, "/seq/", 5) == 0) {
        // API: "/seq/chr1:123-456"
        printf("setting to FILE [%s] because /seq/...\n", path);
        // @ todo - run a check on wether the chr exists and return err otherwise
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;

        //@todo this needs to be defined with some api stuff:!!
        st->st_size = 4096;
    } else {
        st->st_mode = S_IFREG | 0644;
        st->st_nlink = 1;

        if(ffi->from_fastafs) {
            if(ffi->f != nullptr) {
                printf("\033[0;32m[%s]\033[0;33m do_getattr:\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, path, ffi->f->name.c_str(), ffi->padding);

                std::string virtual_fasta_filename = "/" + ffi->f->name + ".fa";
                std::string virtual_faidx_filename = "/" + ffi->f->name + ".fa.fai";
                std::string virtual_ucsc2bit_filename = "/" + ffi->f->name + ".2bit";
                std::string virtual_dict_filename = "/" + ffi->f->name + ".dict";

                if(strcmp(path, virtual_fasta_filename.c_str()) == 0) {
                    st->st_size = ffi->f->fasta_filesize(ffi->padding);
                } else if(strcmp(path, virtual_faidx_filename.c_str()) == 0) {
                    st->st_size = ffi->f->get_faidx(ffi->padding).size();
                } else if(strcmp(path, virtual_ucsc2bit_filename.c_str()) == 0) {
                    st->st_size = ffi->f->ucsc2bit_filesize();
                } else if(strcmp(path, virtual_dict_filename.c_str()) == 0) {
                    st->st_size = ffi->f->dict_filesize();
                } else if(strncmp(path, "/seq/", 5) == 0) { // api access
                    printf("filesize: set to 4096\n");
                }
            }
        } else {
            if(ffi->u2b != nullptr) {
                printf("\033[0;32m[%s]\033[0;33m do_getattr:\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, path, ffi->u2b->name.c_str(), ffi->padding);

                std::string virtual_fasta_filename = "/" + ffi->u2b->name + ".fa";
                std::string virtual_faidx_filename = "/" + ffi->u2b->name + ".fa.fai";

                if(strcmp(path, virtual_fasta_filename.c_str()) == 0) {
                    st->st_size = ffi->u2b->fasta_filesize(ffi->padding);
                } else if(strcmp(path, virtual_faidx_filename.c_str()) == 0) {
                    st->st_size = ffi->u2b->get_faidx(ffi->padding).size();
                }
            }
        }
    }

    return 0;
}



static int do_readdir(const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    fuse_instance *ffi = static_cast<fuse_instance *>(fuse_get_context()->private_data);

    char cur_time[100];
    time_t now = time(0);
    strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime(&now));

    filler(buffer, ".", NULL, 0); // Current Directory
    filler(buffer, "..", NULL, 0); // Parent Directory

    if(ffi->from_fastafs) {
        printf("\033[0;32m[%s]\033[0;33m fastafs::do_readdir(\033[0moffset=%u\033[0;33m):\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, (uint32_t) offset, path, ffi->f->name.c_str(), ffi->padding);

        std::string virtual_fasta_filename = ffi->f->name + ".fa";
        std::string virtual_faidx_filename = ffi->f->name + ".fa.fai";
        std::string virtual_ucsc2bit_filename = ffi->f->name + ".2bit";
        std::string virtual_dict_filename = ffi->f->name + ".dict";

        if(strcmp(path, "/") == 0) {    // If the user is trying to show the files/directories of the root directory show the following
            filler(buffer, virtual_fasta_filename.c_str(), NULL, 0);
            filler(buffer, virtual_faidx_filename.c_str(), NULL, 0);
            filler(buffer, virtual_ucsc2bit_filename.c_str(), NULL, 0);
            filler(buffer, virtual_dict_filename.c_str(), NULL, 0);
        }
    } else {
        if(ffi->u2b != nullptr) {
            printf("\033[0;32m[%s]\033[0;33m 2bit::do_readdir(\033[0moffset=%u\033[0;33m):\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, (uint32_t) offset, path, ffi->u2b->name.c_str(), ffi->padding);

            std::string virtual_fasta_filename = ffi->u2b->name + ".fa";
            std::string virtual_faidx_filename = ffi->u2b->name + ".fa.fai";

            if(strcmp(path, "/") == 0) {    // If the user is trying to show the files/directories of the root directory show the following
                filler(buffer, virtual_fasta_filename.c_str(), NULL, 0);
                filler(buffer, virtual_faidx_filename.c_str(), NULL, 0);
            }
        }
    }

    filler(buffer, "seq", NULL, 0); // Directed indexed API access to subsequence "<mount>/seq/chr1:123-456
    filler(buffer, "seq/chr1:123", NULL, 0); // Directed indexed API access to subsequence "<mount>/seq/chr1:123-456

    return 0;
}


static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
    fuse_instance *ffi = static_cast<fuse_instance *>(fuse_get_context()->private_data);

    char cur_time[100];
    time_t now = time(0);
    strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime(&now));

    static int written = -2;// -1 = permission deinied, -2 = missing file or directory

    if(ffi->from_fastafs) {
        printf("\033[0;32m[%s]\033[0;33m fastafs::do_read(\033[0msize=%u, offset=%u\033[0;33m):\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, (uint32_t) size, (uint32_t) offset, path, ffi->f->name.c_str(), ffi->padding);

        std::string virtual_fasta_filename = "/" + ffi->f->name + ".fa";
        std::string virtual_faidx_filename = "/" + ffi->f->name + ".fa.fai";
        std::string virtual_ucsc2bit_filename = "/" + ffi->f->name + ".2bit";
        std::string virtual_dict_filename = "/" + ffi->f->name + ".dict";

        printf("?? [[%s]]\n", path);
        if(strcmp(path, virtual_fasta_filename.c_str()) == 0) {
            written = (signed int) ffi->f->view_fasta_chunk_cached(ffi->cache, buffer, size, offset);
        } else if(strcmp(path, virtual_faidx_filename.c_str()) == 0) {
            written = (signed int) ffi->f->view_faidx_chunk(ffi->padding, buffer, size, offset);
        } else if(strcmp(path, virtual_ucsc2bit_filename.c_str()) == 0) {
            written = (signed int) ffi->f->view_ucsc2bit_chunk(buffer, size, offset);
        } else if(strcmp(path, virtual_dict_filename.c_str()) == 0) {
            written = (signed int) ffi->f->view_dict_chunk(buffer, size, offset);
        } else if(strncmp(path, "/seq/", 5) == 0) { // api access
            /*
            buffer[0] = 't';
            buffer[1] = 'e';
            buffer[2] = 's';
            buffer[3] = 't';
            //buffer[4] = '\0';
            written = 4;
            */

            printf("!! [[%s]]\n", path);
            // 01 : convert chrom loc to string with chr
            int p = -1;
            for(int i = 5; i < std::min( (int) 256, (int) strlen(path)) && p == -1; i++) {
                printf(":: %c\n",path[i]);
                if(path[i] == ':') {
                    p = i;
                }
            }
            if(p == -1) {
                p = std::min((int) 256, (int) strlen(path));
            }
            std::string chr = std::string(path, 5, p - 5);
            std::cout << "{" << chr << "}" << "\n";

            // 02 : check if 'chr' is equals this->data[i].name
            fastafs_seq *fsq = nullptr;
            size_t i;
            for(i = 0; i < ffi->f->data.size() && fsq == nullptr; i++ ) {
                printf("[%s] == [%s]   \n", chr.c_str() , ffi->f->data[i]->name.c_str());
                if( chr.compare(ffi->f->data[i]->name) == 0) {
                    fsq = ffi->f->data[i];
                }
            }

            // 03 - if chr was found , ok, otherise, not ok
            if(fsq != nullptr) {
                // code below seems to work, but copying to buf doesn't seem to work?

                std::ifstream file(ffi->f->filename.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
                if(file.is_open()) {
                    printf("alive?\n");
                    printf("padding: %i\n", ffi->cache_p0->sequences[i]->padding);

                    written = (signed int) fsq->view_fasta_chunk_cached(
                        ffi->cache_p0->sequences[i], // ffs2f_init_seq* cache,
                        buffer, // char *buffer
                        (size_t) size, // size_t buffer_size,
                        (off_t) 2 + fsq->name.size(), // off_t start_pos_in_fasta,
                        &file //     std::ifstream *fh)
                        );
                    
                    printf("\nwritten: %i\n", (int) written);
                    
                    for(int kk = 0; kk < written ; kk++) {
                        printf("%c", buffer[kk]);
                    }

                    printf("\nwritten: %i\n", (int) written);

                }
                file.close();
            }
        }
    } else {
        if(ffi->u2b != nullptr) {
            printf("\033[0;32m[%s]\033[0;33m 2bit::do_read(\033[0msize=%u, offset=%u\033[0;33m):\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, (uint32_t) size, (uint32_t) offset, path, ffi->u2b->name.c_str(), ffi->padding);

            std::string virtual_fasta_filename = "/" + ffi->u2b->name + ".fa";
            std::string virtual_faidx_filename = "/" + ffi->u2b->name + ".fa.fai";

            if(strcmp(path, virtual_fasta_filename.c_str()) == 0) {
                written = (signed int) ffi->u2b->view_fasta_chunk(ffi->padding, buffer, size, offset);
            } else if(strcmp(path, virtual_faidx_filename.c_str()) == 0) {
                written = (signed int) ffi->u2b->view_faidx_chunk(ffi->padding, buffer, size, offset);
            }
        }
    }

    //printf("    return written=%u\n", written);

    return written;
}



static int do_getxattr(const char* path, const char* name, char* value, size_t size)
{
    fuse_instance *ffi = static_cast<fuse_instance *>(fuse_get_context()->private_data);

    if(ffi->from_fastafs) {
        if(strcmp(name, FASTAFS_FILE_XATTR_NAME.c_str()) == 0) { // requesting FASTAFS filename only works with FASTAFS files..
            size_t filename_size = ffi->f->filename.size();

            if(size > filename_size) {
                strncpy(value,  ffi->f->filename.c_str(), filename_size);
                value[filename_size] = '\0';

                return (int) filename_size + 1;
            } else {
                return ERANGE;
            }
        } else if(strcmp(name, FASTAFS_PID_XATTR_NAME.c_str()) == 0) { // requesting FASTAFS filename only works with FASTAFS files..
            char mypid[8];
            sprintf(mypid, "%d", getpid());
            size_t pid_size = (size_t) strlen(mypid);


            if(size > pid_size) {
                strcpy(value,  mypid);// if statement makes strncpy unneeded
                value[pid_size] = '\0';

                return (int) pid_size + 1;
            } else {
                return ERANGE;
            }
        }
    }

    return -1; // returns -1 on other files
}



void do_destroy(void *pd)
{
    fuse_instance *ffi = static_cast<fuse_instance *>(fuse_get_context()->private_data);

    if(ffi->f != nullptr) {
        delete ffi->f;
    }
    if(ffi->cache != nullptr) {
        delete ffi->cache;
    }
    if(ffi->u2b != nullptr) {
        delete ffi->u2b;
    }

    delete ffi;

    /*
    //fastasfs
    fastafs *f;
    ffs2f_init *cache;
    bool from_fastafs; // if false, from 2bit

    // ucsc2bit
    ucsc2bit *u2b;

    // generic
    uint32_t padding;
    int argc_fuse;
    */
}



fuse_operations operations  = {
    do_getattr, // int (*getattr) (const char *, struct stat *);
    nullptr,    // int (*readlink) (const char *, char *, size_t);
    nullptr,    // int (*getdir) (const char *, fuse_dirh_t, fuse_dirfil_t);
    nullptr,    // int (*mknod) (const char *, mode_t, dev_t);
    nullptr,    // int (*mkdir) (const char *, mode_t);
    nullptr,    // int (*unlink) (const char *);
    nullptr,    // int (*rmdir) (const char *);
    nullptr,    // int (*symlink) (const char *, const char *);
    nullptr,    // int (*rename) (const char *, const char *);
    nullptr,    // int (*link) (const char *, const char *);
    nullptr,    // int (*chmod) (const char *, mode_t);
    nullptr,    // int (*chown) (const char *, uid_t, gid_t);
    nullptr,    // int (*truncate) (const char *, off_t);
    nullptr,    // int (*utime) (const char *, struct utimbuf *);
    nullptr,    // int (*open) (const char *, struct fuse_file_info *);
    do_read,    // int (*read) (const char *, char *, size_t, off_t, struct fuse_file_info *);
    nullptr,    // int (*write) (const char *, const char *, size_t, off_t, struct fuse_file_info *);
    nullptr,    // int (*statfs) (const char *, struct statvfs *);
    nullptr,    // int (*flush) (const char *, struct fuse_file_info *);
    nullptr,    // int (*release) (const char *, struct fuse_file_info *);
    nullptr,    // int (*fsync) (const char *, int, struct fuse_file_info *);
    nullptr,    // int (*setxattr) (const char *, const char *, const char *, size_t, int);
    do_getxattr,// int (*getxattr) (const char *, const char *, char *, size_t);
    nullptr,    // int (*listxattr) (const char *, char *, size_t);
    nullptr,    // int (*removexattr) (const char *, const char *);
    nullptr,    // int (*opendir) (const char *, struct fuse_file_info *);
    do_readdir, // int (*readdir) (const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
    nullptr,    // int (*releasedir) (const char *, struct fuse_file_info *);
    nullptr,    // int (*fsyncdir) (const char *, int, struct fuse_file_info *);
    nullptr,    // void *(*init) (struct fuse_conn_info *conn);
    do_destroy, // void (*destroy) (void *); | void destroy(void* private_data);
    nullptr,    // int (*access) (const char *, int);
    nullptr,    // int (*create) (const char *, mode_t, struct fuse_file_info *);
    nullptr,    // int (*ftruncate) (const char *, off_t, struct fuse_file_info *);
    nullptr,    // int (*fgetattr) (const char *, struct stat *, struct fuse_file_info *);
    nullptr,    // int (*lock) (const char *, struct fuse_file_info *, int cmd, struct flock *);
    nullptr,    // int (*utimens) (const char *, const struct timespec tv[2]);
    nullptr,    // int (*bmap) (const char *, size_t blocksize, uint64_t *idx);
//	nullptr,    // int (*ioctl) (const char *, int cmd, void *arg, struct fuse_file_info *, uint32_t flags, void *data);
//	nullptr,    // int (*poll) (const char *, struct fuse_file_info *, struct fuse_pollhandle *ph, unsigned *reventsp);
//	nullptr,    // int (*write_buf) (const char *, struct fuse_bufvec *buf, off_t off, struct fuse_file_info *);
//	nullptr,    // int (*read_buf) (const char *, struct fuse_bufvec **bufp, size_t size, off_t off, struct fuse_file_info *);
//	nullptr,    // int (*flock) (const char *, struct fuse_file_info *, int op);
//	nullptr,    // int (*fallocate) (const char *, int, off_t, off_t, struct fuse_file_info *);
};




void print_fuse_help()
{
    std::cout << "usage: fasfafs mount <mount: fastafs UID> <mountpoint> [options]\n";
    std::cout << "usage: mount.fastafs <mount: fastafs UID> <mountpoint> [options]\n";
    std::cout << "\n";
    std::cout << "general options:\n";
    std::cout << "    -2   --2bit            virtualise a 2bit file rather than FASTAFS UID\n";
    std::cout << "    -p <n>,--padding <n>   padding / FASTA line length\n";
    std::cout << "    -o opt,[opt...]        mount options\n";
    std::cout << "    -h   --help            print help\n";
    std::cout << "    -V   --version         print version\n";
    std::cout << "\n";
    std::cout << "FUSE options:\n";
    std::cout << "    -d   -o debug          enable debug output (implies -f)\n";
    std::cout << "    -f                     foreground operation\n";
    std::cout << "    -s                     disable multi-threaded operation\n";
    std::cout << "\n";
    std::cout << "    -o allow_other         allow access to other users\n";
    std::cout << "    -o allow_root          allow access to root\n";
    std::cout << "    -o auto_unmount        auto unmount on process termination\n";
    std::cout << "    -o nonempty            allow mounts over non-empty file/dir\n";
    std::cout << "    -o default_permissions enable permission checking by kernel\n";
    std::cout << "    -o fsname=NAME         set filesystem name\n";
    std::cout << "    -o subtype=NAME        set filesystem type\n";
    std::cout << "    -o large_read          issue large read requests (2.4 only)\n";
    std::cout << "    -o max_read=N          set maximum size of read requests\n";
    std::cout << "\n";
    std::cout << "    -o hard_remove         immediate removal (don't hide files)\n";
    std::cout << "    -o use_ino             let filesystem set inode numbers\n";
    std::cout << "    -o readdir_ino         try to fill in d_ino in readdir\n";
    std::cout << "    -o direct_io           use direct I/O\n";
    std::cout << "    -o kernel_cache        cache files in kernel\n";
    std::cout << "    -o [no]auto_cache      enable caching based on modification times (off)\n";
    std::cout << "    -o umask=M             set file permissions (octal)\n";
    std::cout << "    -o uid=N               set file owner\n";
    std::cout << "    -o gid=N               set file group\n";
    std::cout << "    -o entry_timeout=T     cache timeout for names (1.0s)\n";
    std::cout << "    -o negative_timeout=T  cache timeout for deleted names (0.0s)\n";
    std::cout << "    -o attr_timeout=T      cache timeout for attributes (1.0s)\n";
    std::cout << "    -o ac_attr_timeout=T   auto cache timeout for attributes (attr_timeout)\n";
    std::cout << "    -o noforget            never forget cached inodes\n";
    std::cout << "    -o remember=T          remember cached inodes for T seconds (0s)\n";
    std::cout << "    -o nopath              don't supply path if not necessary\n";
    std::cout << "    -o intr                allow requests to be interrupted\n";
    std::cout << "    -o intr_signal=NUM     signal to send on interrupt (10)\n";
    std::cout << "    -o modules=M1[:M2...]  names of modules to push onto filesystem stack\n";
    std::cout << "\n";
    std::cout << "    -o max_write=N         set maximum size of write requests\n";
    std::cout << "    -o max_readahead=N     set maximum readahead\n";
    std::cout << "    -o max_background=N    set number of maximum background requests\n";
    std::cout << "    -o congestion_threshold=N  set kernel's congestion threshold\n";
    std::cout << "    -o async_read          perform reads asynchronously (default)\n";
    std::cout << "    -o sync_read           perform reads synchronously\n";
    std::cout << "    -o atomic_o_trunc      enable atomic open+truncate support\n";
    std::cout << "    -o big_writes          enable larger than 4kB writes\n";
    std::cout << "    -o no_remote_lock      disable remote file locking\n";
    std::cout << "    -o no_remote_flock     disable remote file locking (BSD)\n";
    std::cout << "    -o no_remote_posix_lock disable remove file locking (POSIX)\n";
    std::cout << "    -o [no_]splice_write   use splice to write to the fuse device\n";
    std::cout << "    -o [no_]splice_move    move data while splicing to the fuse device\n";
    std::cout << "    -o [no_]splice_read    use splice to read from the fuse device\n";
    std::cout << "\n";
    std::cout << "Module options:\n";
    std::cout << "\n";
    std::cout << "[iconv]\n";
    std::cout << "    -o from_code=CHARSET   original encoding of file names (default: UTF-8)\n";
    std::cout << "    -o to_code=CHARSET	    new encoding of the file names (default: UTF-8)\n";
    std::cout << "\n";
    std::cout << "[subdir]\n";
    std::cout << "    -o subdir=DIR	    prepend this directory to all paths (mandatory)\n";
    std::cout << "    -o [no]rellinks	    transform absolute symlinks to relative\n";
}


fuse_instance *parse_args(int argc, char **argv, char **argv_fuse)
{
    // Certain arguments do not need to be put into fuse init, e.g "-p" "nextvalue"
    //char **argv_test = (char **) malloc(sizeof(char*) * argc);
    //fastafs_fuse_instance *ffi = new fastafs_fuse_instance({nullptr, 60, 1, new char[argc]});
    //fastafs_fuse_instance *ffi = new fastafs_fuse_instance({nullptr, 60, 0, nullptr});

    fuse_instance *fi = new fuse_instance({nullptr, nullptr, nullptr, true, nullptr, 60, 0});

    //fuse option variable to send to fuse
    argv_fuse[fi->argc_fuse++] = (char *) "fastafs"; // becomes fuse.fastafs

    printf("checkpoint a\n");

    std::vector<char*> fuse_options = {}; // those that need to be appended later

    char current_argument = '\0';// could be o for '-o', etc.



    std::vector<int> full_args = {};
    for(signed int i = 0; i < argc; ++i) {
        printf("processing argv[%i] = '%s'     [current argument=%i]\n", i, argv[i], (int) current_argument);

        if(current_argument != '\0') { // parse the arguments' value
            switch(current_argument) {
            case 'p': // scanning padding value
                if(!sscanf(argv[i], "%u", &fi->padding)) {
                    throw std::invalid_argument("Invalid argument (padding value): " + std::string(argv[i]));
                }
                break;
            default:
                //throw std::invalid_argument("Invalid argument: " + std::string(argv[i]));
                fuse_options.push_back(argv[i]); // could be values that correspond to fuse arguments, such as: -o, -f, etc.
                break;
            }

            current_argument = '\0';// reset
        } else if(argv[i][0] == '-') {
            if(strlen(argv[i]) == 1) {
                throw std::invalid_argument("Invalid argument: " + std::string(argv[i]));
            }

            switch(argv[i][1]) {
            case '2': // a fastafs specific flag
                fi->from_fastafs = false;
                break;

            case 'f': // fuse specific flags
            case 's':
            case 'd':
                fuse_options.push_back(argv[i]);
                break;

            case 'o': // argument, fuse specific
                current_argument = argv[i][1];
                fuse_options.push_back(argv[i]); // fuse specific
                break;

            default: // argument, fastafs spcific (such as '-p' followed by '50')
                current_argument = argv[i][1];
                break;
            }
        } else {
            full_args.push_back(i); // args such as 'sudo', 'fastafs' 'fastafs-uid' and '/mount/point' of which only the last 2 are needed
        }
    }

    printf("checkpoint b\n");


    if(full_args.size() > 2) {
        printf("checkpoint c\n");
        printf("full_args.size() = %u\n", (uint32_t) full_args.size());
        int mount_target_arg = full_args[full_args.size() - 2 ]; // last two arguments are <fastafs file> and <mount point>, location to last 2 args not starting with --/- are in this vector
        printf("out of bound???\n");

        if(fi->from_fastafs) {
            database d = database();
            std::string fname = d.get(argv[mount_target_arg]);

            std::string name; // prefix name of mounted fasta dict 2bit and fai files
            if(fname.size() == 0) { // invalid mount argument, don't bind fastafs object
                fname = std::string(argv[mount_target_arg]);
                //name = std::filesystem::path(fname).filename();
                name = basename_cpp(fname);

                // remove .fastafs suffix
                size_t lastindex = name.find_last_of(".");
                name = name.substr(0, lastindex);

            } else {
                name = std::string(argv[mount_target_arg]);
            }

            fi->f = new fastafs(name);
            fi->f->load(fname);
            fi->cache = fi->f->init_ffs2f(fi->padding, true);// allow mixed case
            fi->cache_p0 = fi->f->init_ffs2f(0, true);// allow mixed case
        } else {
            std::string basename = basename_cpp(std::string(argv[mount_target_arg]));
            //std::string basename = std::filesystem::path(std::string(argv[mount_target_arg])).filename();

            fi->u2b = new ucsc2bit(basename);// useses basename as prefix for filenames to mount: hg19.2bit -> hg19.2bit.fa
            fi->u2b->load(std::string(argv[mount_target_arg]));
        }

        //argv_fuse[fi->argc_fuse++] = argv[mount_target_arg];
        argv_fuse[fi->argc_fuse++] = argv[mount_target_arg + 1];
        for(size_t j = 0; j < fuse_options.size(); j++) {
            argv_fuse[fi->argc_fuse++] = fuse_options[j];
        }
    }

    printf("checkpoint c\n");

    return fi;
}




void fuse(int argc, char *argv[])
{
    printf("wake up\n");

    // part 1 - rewrite args because "fastafs" "mount" is considered as two args, crashing fuse_init
    //  - @todo at some point define that second mount is not really important? if possible
    char *argv2[argc];
    fuse_instance *ffi = parse_args(argc, argv, argv2);

    printf("checkpoint\n");

    // part 2 - print what the planning is
    char cur_time[100];
    time_t now = time(0);
    strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime(&now));
    printf("\033[0;32m[%s]\033[0;33m init (recv arguments):\033[0m [argc=%i]", cur_time, argc);
    for(int i = 0; i < argc; i++) {
        printf(" argv[%u]=\"%s\"", i, argv[i]);
    }

    strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime(&now));
    printf("\n\033[0;32m[%s]\033[0;33m init (fuse arguments):\033[0m [argc=%i]", cur_time, ffi->argc_fuse);
    for(int i = 0; i < ffi->argc_fuse; i++) {
        printf(" argv[%u]=\"%s\"", i, argv2[i]);
    }

    printf("\n");

    if(ffi->f == nullptr && ffi->u2b == nullptr) { // no fastafs was loaded
        print_fuse_help();
        exit(0);
    } else {
        fuse_main(ffi->argc_fuse, argv2, &operations, ffi);
    }
    //http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
}

