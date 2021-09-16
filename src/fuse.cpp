
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
#include <pthread.h>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <semaphore.h>

#include "fuse.hpp"
#include "database.hpp"
#include "fastafs.hpp"
#include "ucsc2bit.hpp"
#include "sequence_region.hpp"
#include "chunked_reader.hpp"


// http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
// more examples: https://libfuse.github.io/doxygen/hello_8c.html



struct file_thread_info {
    chunked_reader *cr;
    sem_t sem;
};

int MAX_FILE_THREADS = 12;

struct file_threads {
    std::vector<file_thread_info> crs;
    int thread_i = 0; // thread iterator
};



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
    bool allow_masking;
    int argc_fuse;

    timespec ts[2]; // access and modify time
};



static int do_getattr(const char *path, struct stat *st)
{
    fuse_instance *ffi = static_cast<fuse_instance *>(fuse_get_context()->private_data);

#if DEBUG
    char cur_time[100];
    time_t now = time(0);
    strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));
#endif

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

    st->st_nlink = 1;

    if(strcmp(path, "/") == 0) {
        //st->st_mode = S_IFREG | 0444;
        //st->st_nlink = 1;
        //st->st_size = 1024;
        //directory
        st->st_mode = S_IFDIR | 0555;
        st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
    } else if(strlen(path) == 4 && strncmp(path, "/seq", 4) == 0) {
        //directory
        //printf("setting to DIR because /seq\n");
        st->st_mode = S_IFDIR | 0555;
        st->st_nlink = 1;
    } else if(strlen(path) > 4 &&  strncmp(path, "/seq/", 5) == 0) {
        // API: "/seq/chr1:123-456"
        //printf("setting to FILE [%s] because /seq/...\n", path);
        // @ todo - run a check on wether the chr exists and return err otherwise
        st->st_mode = S_IFREG | 0444;
        st->st_nlink = 1;

        //@todo this needs to be defined with some api stuff:!!
        st->st_size = (signed int) ffi->f->view_sequence_region_size(ffi->cache_p0, (strchr(path, '/') + 5));
    } else {
        st->st_mode = S_IFREG | 0444;
        st->st_nlink = 1;

        if(ffi->from_fastafs) {
            if(ffi->f != nullptr) {
#if DEBUG
                printf("\033[0;32m[%s]\033[0;33m do_getattr:\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, path, ffi->f->name.c_str(), ffi->padding);
#endif

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
                }
                //else if(strncmp(path, "/seq/", 5) == 0) { // api access
                //printf("filesize: set to 4096\n");
                //}
            }
        } else {
            if(ffi->u2b != nullptr) {
#if DEBUG
                printf("\033[0;32m[%s]\033[0;33m do_getattr:\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, path, ffi->u2b->name.c_str(), ffi->padding);
#endif

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

#if DEBUG
    char cur_time[100];
    time_t now = time(0);
    strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));
#endif

    filler(buffer, ".", NULL, 0); // Current Directory
    filler(buffer, "..", NULL, 0); // Parent Directory

    if(ffi->from_fastafs) {
#if DEBUG
        printf("\033[0;32m[%s]\033[0;33m do_readdir(\033[0moffset=%u\033[0;33m):\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, (uint32_t) offset, path, ffi->f->name.c_str(), ffi->padding);
#endif

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
#if DEBUG
            printf("\033[0;32m[%s]\033[0;33m 2bit::do_readdir(\033[0moffset=%u\033[0;33m):\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, (uint32_t) offset, path, ffi->u2b->name.c_str(), ffi->padding);
#endif

            std::string virtual_fasta_filename = ffi->u2b->name + ".fa";
            std::string virtual_faidx_filename = ffi->u2b->name + ".fa.fai";

            if(strcmp(path, "/") == 0) {    // If the user is trying to show the files/directories of the root directory show the following
                filler(buffer, virtual_fasta_filename.c_str(), NULL, 0);
                filler(buffer, virtual_faidx_filename.c_str(), NULL, 0);
            }
        }
    }

    if(strcmp(path, "/") == 0) {    // If the user is trying to show the files/directories of the root directory show the following
        filler(buffer, "seq", NULL, 0); // Directed indexed API access to subsequence "<mount>/seq/chr1:123-456
    }

    return 0;
}


static int do_open(const char *path, struct fuse_file_info *fi)
{
    fuse_instance *ffi = static_cast<fuse_instance *>(fuse_get_context()->private_data);

#if DEBUG
    char cur_time[100];
    time_t now = time(0);
    strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));

    printf("\033[0;32m[%s]\033[0;33m do_open\n", cur_time);
    //(\033[0ms=%u, off=%u\033[0;33m):\033[0m %s \033[0;35m(%s, pad: %u)\033[0m\n",
    //cur_time, (uint32_t) size, (uint32_t) offset, path, );
#endif

    //chunked_reader *cr = new chunked_reader(ffi->f->filename.c_str());


    // has list with 32 chunked_reader objects
    file_threads *ft = new file_threads();
    for(ft->thread_i = 0; ft->thread_i < MAX_FILE_THREADS;  ft->thread_i++) {
        ft->crs.push_back(
        file_thread_info{
            new chunked_reader(ffi->f->filename.c_str())
        }
        );

        //printf("sem init... \n");
        sem_init(&(ft->crs[ft->thread_i].sem), 0, 1);
        //printf("sem init done... \n");
    }
    ft->thread_i = 0;

    fi->fh = reinterpret_cast<uintptr_t>(ft);
    
#if DEBUG
    printf("\033[0;35m fi->fh: %u\n", (unsigned int) fi->fh);
    printf("\033[0;35m fi->writepage: %u\n", fi->writepage);
    printf("\033[0;35m fi->direct_io: %u\n", fi->direct_io);
    printf("\033[0;35m fi->keep_cache: %u\n", fi->keep_cache);
    printf("\033[0;35m fi->padding: %u\n", fi->padding);
#endif

    // here the fi->fh should be set?!
    // if possible to chunked reader?
    //chunked_reader *cr = new chunked_reader("/tmp/wget");
    //fi->fh = fh_i++;
    // should be set to a real fh i presume?

    return 0;
}

static int do_flush(const char *path, struct fuse_file_info *fi)
{
    return 0;
}

static int do_release(const char *path, struct fuse_file_info *fi)
{
    file_threads *ft = (file_threads*) fi->fh;
    //printf("do_release() - filehandle had [ %i ] locks \n", ft->thread_locks);

    if(ft != nullptr) {
        for(size_t i = 0; i < ft->crs.size(); i++) {
            sem_destroy(&ft->crs[i].sem);
            delete ft->crs[i].cr;

        }
        delete ft;
    }

    return 0;
}


// threaded implementation of libfuse?
// https://libfuse.github.io/doxygen/poll_8c.html

// test file error reads at: do_read(s=4096, off=20480):
static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi)
{
    file_threads *ft = (file_threads*) fi->fh;
    int cur_file_thread = ft->thread_i++ % MAX_FILE_THREADS;
    sem_wait(&ft->crs[cur_file_thread].sem);

    fuse_instance *ffi = static_cast<fuse_instance *>(fuse_get_context()->private_data);

    static int written = -2;// -1 = permission deinied, -2 = missing file or directory

    if(ffi->from_fastafs) {
#if DEBUG
        char cur_time[100];
        time_t now = time(0);
        strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));

        printf("\033[0;32m[%s]\033[0;33m do_read(\033[0ms=%u, off=%u\033[0;33m):\033[0m %s \033[0;35m(%s, pad: %u)\033[0m\n", cur_time, (uint32_t) size, (uint32_t) offset, path, ffi->f->name.c_str(), ffi->padding);
        //printf("\033[0;35m fi:     0x%p\n", (uintptr_t) fi);
        printf("\033[0;35m fi:     0x%p\n", (void*) fi);
        printf("\033[0;35m fi->fh: %u\n", (unsigned int) fi->fh);
        printf("\033[0;35m fi->writepage: %u\n", fi->writepage);
        printf("\033[0;35m fi->direct_io: %u\n", fi->direct_io);
        printf("\033[0;35m fi->keep_cache: %u\n", fi->keep_cache);
        printf("\033[0;35m fi->padding: %u\n", fi->padding);
#endif

        std::string virtual_fasta_filename = "/" + ffi->f->name + ".fa";
        std::string virtual_faidx_filename = "/" + ffi->f->name + ".fa.fai";
        std::string virtual_ucsc2bit_filename = "/" + ffi->f->name + ".2bit";
        std::string virtual_dict_filename = "/" + ffi->f->name + ".dict";

        if(strcmp(path, virtual_fasta_filename.c_str()) == 0) {
            written = (signed int) ffi->f->view_fasta_chunk(ffi->cache, buffer, size, offset, *ft->crs[cur_file_thread].cr);
        } else if(strcmp(path, virtual_faidx_filename.c_str()) == 0) {
            written = (signed int) ffi->f->view_faidx_chunk(ffi->padding, buffer, size, offset);
        } else if(strcmp(path, virtual_ucsc2bit_filename.c_str()) == 0) {
            written = (signed int) ffi->f->view_ucsc2bit_chunk(buffer, size, offset);
        } else if(strcmp(path, virtual_dict_filename.c_str()) == 0) {
            written = (signed int) ffi->f->view_dict_chunk(buffer, size, offset);
        } else if(strncmp(path, "/seq/", 5) == 0) { // api access
            written = (signed int) ffi->f->view_sequence_region(ffi->cache_p0, (strchr(path, '/') + 5), buffer, size, offset);
        }
    } else {
        if(ffi->u2b != nullptr) {
#if DEBUG
            char cur_time[100];
            time_t now = time(0);
            strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));

            printf("\033[0;32m[%s]\033[0;33m 2bit::do_read(\033[0msize=%u, offset=%u\033[0;33m):\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n", cur_time, (uint32_t) size, (uint32_t) offset, path, ffi->u2b->name.c_str(), ffi->padding);
#endif

            std::string virtual_fasta_filename = "/" + ffi->u2b->name + ".fa";
            std::string virtual_faidx_filename = "/" + ffi->u2b->name + ".fa.fai";

            if(strcmp(path, virtual_fasta_filename.c_str()) == 0) {
                written = (signed int) ffi->u2b->view_fasta_chunk(ffi->padding, buffer, size, offset);
            } else if(strcmp(path, virtual_faidx_filename.c_str()) == 0) {
                written = (signed int) ffi->u2b->view_faidx_chunk(ffi->padding, buffer, size, offset);
            }
        }
    }

    sem_post(&ft->crs[cur_file_thread].sem);

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

    return ENODATA; // returns -1 on other files
}


// decoy function to not throw an error if snakemake access this
// as it doesn't have access to fi it is practically not possible to do a generic update
static int do_utimens(const char *path, const struct timespec ts[2]) // seems it doesn't understand 'fuse_file_info ?' , struct fuse_file_info *fi)
{
    //(void) fi;
    //int res;

    /* don't use utime/utimes since they follow symlinks */
    //res = utimensat(0, path, ts, AT_SYMLINK_NOFOLLOW);
    // set fi data  to ts
    //if (res == -1)
    //return -errno;

    return 0;
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

    // for
    //if(ffi->cr != nullptr) {
    //delete ffi->cr;
    //}


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
    do_open,    // int (*open) (const char *, struct fuse_file_info *);
    do_read,    // int (*read) (const char *, char *, size_t, off_t, struct fuse_file_info *);
    nullptr,    // int (*write) (const char *, const char *, size_t, off_t, struct fuse_file_info *);
    nullptr,    // int (*statfs) (const char *, struct statvfs *);
    do_flush,   // int (*flush) (const char *, struct fuse_file_info *);
    do_release, // int (*release) (const char *, struct fuse_file_info *);
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
    do_utimens, // int (*utimens) (const char *, const struct timespec tv[2]); // sets / updates access and modify time values
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
    std::cout << "    -m   --no-masking      Disable masking; bases in lower-case (not for 2bit output)\n";
    std::cout << "    -p <n>,--padding <n>   padding / FASTA line length\n";
    std::cout << "    -g   --from-file       force reading from file rather than cache database\n";
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



    fuse_instance *fi = new fuse_instance({
        nullptr, // pointer to fastafs decoder - if from_fasta is set to true
        nullptr,// pointer to fastafs_init with defined padding
        nullptr, // pointer to fastafs_init with cache size of 0 (for mounting ./seq/chr1:123-456
        true, // from fastafs

        nullptr, // pointer to ucsc2bit decoder - if from_fasta is set to false
        60, // default_padding
        true, // allow_masking
        0 // argc_fuse
    });

    //fuse option variable to send to fuse
    argv_fuse[fi->argc_fuse++] = (char *) "fastafs"; // becomes fuse.fastafs

    std::vector<char*> fuse_options = {}; // those that need to be appended later

    char current_argument = '\0';// could be o for '-o', etc.

    bool from_file_rather_than_from_db = false;

    std::vector<int> full_args = {};
    for(signed int i = 0; i < argc; ++i) {
#if DEBUG
        printf("processing argv[%i] = '%s'     [current argument=%i]\n", i, argv[i], (int) current_argument);
#endif

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
            case 'm': // disable masking
                fi->allow_masking = false;
                break;

            case 'g': // directly use file
                from_file_rather_than_from_db = true;
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
                if(strcmp(argv[i], "--2bit") == 0) {
                    fi->from_fastafs = false;;
                } else if(strcmp(argv[i], "--no-masking") == 0) {
                    fi->allow_masking = false;
                } else {
                    current_argument = argv[i][1];
                }
                break;
            }
        } else {
            full_args.push_back(i); // args such as 'sudo', 'fastafs' 'fastafs-uid' and '/mount/point' of which only the last 2 are needed
        }
    }


    if(full_args.size() > 2) {
        int mount_target_arg = full_args[full_args.size() - 2 ]; // last two arguments are <fastafs file> and <mount point>, location to last 2 args not starting with --/- are in this vector

        if(fi->from_fastafs) {
            std::string fname;
            std::string name;
            
            if(from_file_rather_than_from_db) {
                fname = std::string(argv[mount_target_arg]);
                //name = std::filesystem::path(fname).filename();
                name = basename_cpp(fname);

                // remove .fastafs suffix
                size_t lastindex = name.find_last_of(".");
                name = name.substr(0, lastindex);
            } else {
                database d = database();
                fname = d.get(argv[mount_target_arg]);

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
            }

            fi->f = new fastafs(name);
            fi->f->load(fname);
            fi->cache = fi->f->init_ffs2f(fi->padding, fi->allow_masking);
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

    return fi;
}



void fuse(int argc, char *argv[])
{


    // part 1 - rewrite args because "fastafs" "mount" is considered as two args, crashing fuse_init
    //  - @todo at some point define that second mount is not really important? if possible
    char *argv2[argc];
    fuse_instance *ffi = parse_args(argc, argv, argv2);

#if DEBUG
    // part 2 - print what the planning is
    char cur_time[100];
    time_t now = time(0);
    strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("\033[0;32m[%s]\033[0;33m init (recv arguments):\033[0m [argc=%i]", cur_time, argc);
    for(int i = 0; i < argc; i++) {
        printf(" argv[%u]=\"%s\"", i, argv[i]);
    }

    strftime(cur_time, 100, "%Y-%m-%d %H:%M:%S", localtime(&now));
    printf("\n\033[0;32m[%s]\033[0;33m init (fuse arguments):\033[0m [argc=%i]", cur_time, ffi->argc_fuse);
    for(int i = 0; i < ffi->argc_fuse; i++) {
        printf(" argv[%u]=\"%s\"", i, argv2[i]);
    }

    printf("\n");
#endif


    if(ffi->f == nullptr && ffi->u2b == nullptr) { // no fastafs was loaded
        print_fuse_help();
        exit(0);
    } else {
        fuse_main(ffi->argc_fuse, argv2, &operations, ffi);
    }
    //http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/

    //return ret;
}

