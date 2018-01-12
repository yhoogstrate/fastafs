
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



#include "fuse.hpp"
#include "database.hpp"


// http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/
// more examples: https://libfuse.github.io/doxygen/hello_8c.html



struct fastafs_fuse_instance {
	fastafs *f;
	unsigned int padding;
};



static int do_getattr( const char *path, struct stat *st )
{
	fastafs_fuse_instance *ffi = static_cast<fastafs_fuse_instance *>(fuse_get_context()->private_data);

	char cur_time[100];
	time_t now = time(0);
	strftime (cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));
	printf("\033[0;32m[%s]\033[0;33m do_getattr:\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n",cur_time, path, ffi->f->name.c_str(), ffi->padding);
	
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
	
	if ( strcmp( path, "/" ) == 0 ) {
		//st->st_mode = S_IFREG | 0644;
		//st->st_nlink = 1;
		//st->st_size = 1024;
		
		//directory
		st->st_mode = S_IFDIR | 0755;
		st->st_nlink = 2; // Why "two" hardlinks instead of "one"? The answer is here: http://unix.stackexchange.com/a/101536
	} else {
		std::string virtual_fasta_filename = "/" + ffi->f->name + ".fa";
		std::string virtual_faidx_filename = "/" + ffi->f->name + ".fa.fai";

		st->st_mode = S_IFREG | 0644;
		st->st_nlink = 1;
		
		if(strcmp(path, virtual_fasta_filename.c_str()) == 0) {
			st->st_size = ffi->f->fasta_filesize(ffi->padding);
		}
		else if(strcmp(path, virtual_faidx_filename.c_str()) == 0) {
			st->st_size = ffi->f->get_faidx(ffi->padding).size();
		}
	}
	
	printf("    st_size: %u\n", st->st_size);
	
	return 0;
}


static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	fastafs_fuse_instance *ffi = static_cast<fastafs_fuse_instance *>(fuse_get_context()->private_data);

	char cur_time[100];
	time_t now = time (0);
	strftime (cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));
	printf("\033[0;32m[%s]\033[0;33m do_readdir(\033[0moffset=%u\033[0;33m):\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n",cur_time, (unsigned int) offset, path, ffi->f->name.c_str(), ffi->padding);

	std::string virtual_fasta_filename = ffi->f->name + ".fa";
	std::string virtual_faidx_filename = ffi->f->name + ".fa.fai";

	filler(buffer, ".", NULL, 0); // Current Directory
	filler(buffer, "..", NULL, 0); // Parent Directory
	
	if (strcmp(path, "/" ) == 0 ) { // If the user is trying to show the files/directories of the root directory show the following
		filler(buffer, virtual_fasta_filename.c_str(), NULL, 0);
		filler(buffer, virtual_faidx_filename.c_str(), NULL, 0);

		std::cout << "    " << virtual_fasta_filename << "\n";
		std::cout << "    " << virtual_faidx_filename << "\n";
	}

	
	return 0;
}


static int do_read(const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
	fastafs_fuse_instance *ffi = static_cast<fastafs_fuse_instance *>(fuse_get_context()->private_data);
	
	char cur_time[100];
	time_t now = time (0);
	strftime (cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));
	printf("\033[0;32m[%s]\033[0;33m do_read(\033[0msize=%u, offset=%u\033[0;33m):\033[0m %s   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n",cur_time, (unsigned int) size, (unsigned int) offset, path, ffi->f->name.c_str(), ffi->padding);
	
	std::string virtual_fasta_filename = "/" + ffi->f->name + ".fa";
	std::string virtual_faidx_filename = "/" + ffi->f->name + ".fa.fai";
	
	static int written;
	if(strcmp(path, virtual_fasta_filename.c_str() ) == 0) {
		written = ffi->f->view_fasta_chunk(ffi->padding, buffer, size, offset);
		printf("    return written=%u\n", written);
	}
	else if(strcmp(path, virtual_faidx_filename.c_str() ) == 0 ) {
		written = ffi->f->view_faidx_chunk(ffi->padding, buffer, size, offset);
		printf("    return written=%u\n", written);
	} else {
		written = -1;
	}
	
	return written;
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
	nullptr,    // int (*getxattr) (const char *, const char *, char *, size_t);
	nullptr,    // int (*listxattr) (const char *, char *, size_t);
	nullptr,    // int (*removexattr) (const char *, const char *);
	nullptr,    // int (*opendir) (const char *, struct fuse_file_info *);
	do_readdir, // int (*readdir) (const char *, void *, fuse_fill_dir_t, off_t, struct fuse_file_info *);
	nullptr,    // int (*releasedir) (const char *, struct fuse_file_info *);
	nullptr,    // int (*fsyncdir) (const char *, int, struct fuse_file_info *);
	nullptr,    // void *(*init) (struct fuse_conn_info *conn);
	nullptr,    // void (*destroy) (void *);
	nullptr,    // int (*access) (const char *, int);
	nullptr,    // int (*create) (const char *, mode_t, struct fuse_file_info *);
	nullptr,    // int (*ftruncate) (const char *, off_t, struct fuse_file_info *);
	nullptr,    // int (*fgetattr) (const char *, struct stat *, struct fuse_file_info *);
	nullptr,    // int (*lock) (const char *, struct fuse_file_info *, int cmd, struct flock *);
	nullptr,    // int (*utimens) (const char *, const struct timespec tv[2]);
	nullptr,    // int (*bmap) (const char *, size_t blocksize, uint64_t *idx);
//	nullptr,    // int (*ioctl) (const char *, int cmd, void *arg, struct fuse_file_info *, unsigned int flags, void *data);
//	nullptr,    // int (*poll) (const char *, struct fuse_file_info *, struct fuse_pollhandle *ph, unsigned *reventsp);
//	nullptr,    // int (*write_buf) (const char *, struct fuse_bufvec *buf, off_t off, struct fuse_file_info *);
//	nullptr,    // int (*read_buf) (const char *, struct fuse_bufvec **bufp, size_t size, off_t off, struct fuse_file_info *);
//	nullptr,    // int (*flock) (const char *, struct fuse_file_info *, int op);
//	nullptr,    // int (*fallocate) (const char *, int, off_t, off_t, struct fuse_file_info *);
};



static struct options {
	//const char *filename;
	int *padding;
	int show_help;
} options;

#define OPTION(t, p)        { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
	//OPTION("--name=%s", filename),
	OPTION("--padding=%i", padding),
	OPTION("-q", show_help),
	OPTION("--qelp", show_help),
	FUSE_OPT_END
};



void fuse(int argc, char *argv[])
{
	// part 1 - rewrite args because "fastafs" "mount" is considered as two args, crashing fuse_init
	//  - @todo at some point define that second mount is not really important? if possible
	char *argv2[argc];
	int argc2 = argc - 1;
	argv2[0] = (char *) "fasfafs mount";
	for(unsigned i = 2; i < argc; i++)
	{
		if(i == argc - 2 and argv[i][0] != '-') {
			argv2[i-1] = argv[i+1];
			argc2 -= 1; 
			break;
		}
		else {
			argv2[i-1] = argv[i];
		}
	}
	
	
	// part 2 - print what the planning is
	char cur_time[100];
	time_t now = time (0);
	strftime (cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));
	printf("\033[0;32m[%s]\033[0;33m init:\033[0m [argc=%i]",cur_time, argc);
	for(unsigned int i=0;i<argc;i++){
		printf(" argv[%u]=\"%s\"", i, argv[i]);
	}
	strftime (cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));
	printf("\n\033[0;32m[%s]\033[0;33m init:\033[0m [argc2=%i]",cur_time,argc2);
	for(unsigned int i=0;i<argc2;i++){
		printf(" argv2[%u]=\"%s\"", i, argv2[i]);
	}
	printf("\n");


	// plan 3 - add "-p/--padding" to args
	struct fuse_args args = FUSE_ARGS_INIT(argc2, argv2);
	//fuse_opt_parse(&args, NULL, NULL, NULL);
	fuse_opt_parse(&args, &options, option_spec, NULL);
	printf("  padding: %i\n", options.padding);
	//fuse_opt_add_arg(&args, "-p/--padding   padding size of FASTA file");

	
	// plan 4 - depending on the args either run help or bind the actual fastafs object
	if(argc2 < 2) { // not enought parameters, don't bind fastafs object
		fuse_main(args.argc, args.argv, &operations, NULL);
	}
	else {
		// @todo -F/--file for straight from fastafs file?
		database d = database();
		std::string fname = d.get(argv[argc - 2]);
		if(fname.size() == 0) { // invalid mount argument, don't bind fastafs object
			fuse_main(args.argc, args.argv, &operations, NULL);
		}
		else { // valid fastafs and bind fastafs object
			fastafs *f = new fastafs(std::string(argv[argc - 2]));
			f->load(fname);
			
			fastafs_fuse_instance *ffi = new fastafs_fuse_instance({f, 50});
			
			strftime (cur_time, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));
			printf("\033[0;32m[%s]\033[0;33m mounting\033[0m",cur_time);
			printf("   \033[0;35m(fastafs: %s, padding: %u)\033[0m\n",ffi->f->name.c_str(), ffi->padding);
			
			fuse_main(args.argc, args.argv, &operations, ffi);
		}
	}
	
	//http://www.maastaar.net/fuse/linux/filesystem/c/2016/05/21/writing-a-simple-filesystem-using-fuse/

	
	//struct fuse_args args = FUSE_ARGS_INIT(argc2, argv2);

	//fuse_opt_parse(&args, NULL, NULL, NULL);
	//fuse_opt_add_arg(&args, "-omodules=subdir,subdir=/foo");

	//return fuse_main(args.argc, args.argv, &my_operations, NULL);

}
