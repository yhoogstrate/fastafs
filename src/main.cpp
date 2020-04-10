
#include <iostream>
#include <array>
#include <vector>
#include <string.h>
#include <unordered_map>

#include "config.hpp"
#include "fasta_to_fastafs.hpp"
#include "ucsc2bit_to_fastafs.hpp"
#include "database.hpp"
#include "fuse.hpp"
#include "lsfastafs.hpp"

#include "seekable_compression.hpp"

// https://github.com/facebook/zstd/issues/521
// https://github.com/samtools/samtools/blob/develop/faidx.c


void usage()
{
    std::cout << "usage: " << PACKAGE << " [--version] [--help]" << std::endl << std::endl;
    std::cout <<      "  [generic operations]" << std::endl;
    std::cout <<      "    list       overview of FASTAFS database" << std::endl;
    std::cout <<      "    ps         overview of mounted FASTAFS instances and mountpoints" << std::endl;
    std::cout <<      std::endl;
    std::cout <<      "  [single fastafs operations]" << std::endl;
    std::cout <<      "    cache      adds FASTA file to cache" << std::endl;
    std::cout <<      "    view       view FASTAFS as FASTA file" << std::endl;
    std::cout <<      "    info       view FASTAFS information" << std::endl;
    std::cout <<      "    mount      mount FASTAFS as FASTA/2BIT file" << std::endl;
    std::cout <<      "    check      checks file integrity of FASTAFS file" << std::endl;
    std::cout << std::endl;
}

void usage_view(void)
{
    std::cout << "usage: " << PACKAGE << " view [OPTION]... [FASTAFS-ID/FILE]" << std::endl;
    std::cout << "View FASTAFS file in FASTA format" << std::endl << std::endl;
    std::cout << "  -f, --file           Provide fastafs by file path, not from database (cache)" << std::endl;
    std::cout << "  -p, --padding        Number of nucleotides before delimited with a newline [default=60]" << std::endl;
    std::cout << "  -m, --no-masking     Disable masking; bases in lower-case (not for 2bit output)" << std::endl;
    std::cout << "  -2, --2bit           View in UCSC twoBit/2bit format" << std::endl;
    std::cout << "                         http://genome.ucsc.edu/FAQ/FAQformat.html#format7" << std::endl;
    std::cout << std::endl;
    std::cout << "  -h, --help           Display this help and exit";
    std::cout << std::endl;
}

void usage_info(void)
{
    std::cout << "usage: " << PACKAGE << " info [<options>] <fastafs-id>\n\n";
    std::cout << "  -f, --file           Provide fastafs by file path, not from database (cache)" << std::endl;
    std::cout << "  -e. --ena-verify     Verify if sha1 checksums of sequences exist in ENA\n";
    std::cout << std::endl;
    std::cout << "  -h, --help           Display this help and exit";
    std::cout << std::endl;
}

void usage_check(void)
{
    std::cout << "usage: " << PACKAGE << " check [<options>] <fastafs-id>\n\n";
    std::cout << "  -f, --file           Provide fastafs by file path, not from database (cache)" << std::endl;
    std::cout << "  -5, --md5            Check file integrity using whole-file-CRC32 plus per-sequence MD5" << std::endl;
    std::cout << std::endl;
    std::cout << "  -h, --help           Display this help and exit";
    std::cout << std::endl;
}

void usage_cache(void)
{
    std::cout << "usage: " << PACKAGE << " cache <fastafs-id> <fasta file | ucsc TwoBit file>\n";
    std::cout << "                         cache -o <fastafs-file-path> <fasta file | ucsc TwoBit file>\n\n";
    std::cout << "  -o, --output-file    Explicitly define fastafs output file and do not write to database (cache)\n";
    std::cout << "  -2, --2bit           Force 2bit when files become larger than 4bit due to huge N-blocks\n";
    std::cout << "  -f, --fastafs-only   Convert to FASTAFS only; skip ZSTD-seekable\n";
}

int main(int argc, char *argv[])
{
    if(argc > 1) {
        if(strcmp(argv[1], "--help") == 0 or strcmp(argv[1], "-h") == 0) {
            usage();
        } else if(strcmp(argv[1], "--version") == 0) {
#if DEBUG
            std::cout << PACKAGE << " v" << PACKAGE_VERSION << GIT_SHA1_STRING << "-debug\n\n";
#else
            std::cout << PACKAGE << " v" << PACKAGE_VERSION << GIT_SHA1_STRING << "-release\n\n";
#endif //DEBUG

            std::cout << "Copyright (C) 2017 Dr. Youri Hoogstrate." << "\n";
            std::cout << "License GPLv2+: GNU GPL version 2 or later <http://gnu.org/licenses/gpl.html>.\n";
            std::cout << "This is free software: you are free to change and redistribute it.\n";
            std::cout << "There is NO WARRANTY, to the extent permitted by law.\n\n";
            std::cout << "The " << PACKAGE << " package is written by Youri Hoogstrate.\n";

            exit(0);
        } else if(strcmp(argv[1], "cache") == 0) {
            if(argc > 3) {
                bool to_cache = true;
                bool auto_recompress_to_fourbit = true;
                bool compress_to_zstd_seekable = true;

                for(int i = 0 ; i < argc ; i++) {
                    if((strcmp(argv[i], "-2") == 0) or (strcmp(argv[i], "--2bit") == 0)) {
                        auto_recompress_to_fourbit = false;
                    }
                    
                    if((strcmp(argv[i], "-f") == 0) or (strcmp(argv[i], "--fastafs-only") == 0)) {
                        compress_to_zstd_seekable = false;
                    }
                    
                    if( i < argc - 1 and
                        ((strcmp(argv[argc - 3], "-o") == 0) or (strcmp(argv[argc - 3], "--output-file") == 0))
                    ) {
                        to_cache = false;
                    }
                }


                // reserve place in database
                std::string fname_out;
                if(to_cache) {
                    database d = database();
                    fname_out = d.add(argv[argc - 2]);
                } else {
                    fname_out = std::string(argv[argc - 2]);
                }


                // convert to plain fastafs
                if(is_fasta_file(argv[argc - 1])) {
                    // converter is now generic for 2 and 4 bit
                    fasta_to_fastafs(argv[argc - 1], fname_out, auto_recompress_to_fourbit);
                } else if(is_ucsc2bit_file(argv[argc - 1])) {
                    ucsc2bit_to_fastafs(argv[argc - 1], fname_out);
                } else {
                    throw std::runtime_error("[main::cache] Invalid file format");
                    return 1;
                }


                // convert to zstd seekable
                if(compress_to_zstd_seekable) {
                    std::string fname_out_zstd = fname_out + ".zst";
                    size_t zst_written = ZSTD_seekable_compressFile_orDie( (const char*) fname_out.c_str(),
                                                      (const char*) fname_out_zstd.c_str(),
                                                      (int) ZSTD_COMPRESSION_QUALIITY,
                                                      (unsigned) ZSTD_SEEKABLE_FRAME_SIZE);

                    if(zst_written > 0) {
                        remove(fname_out.c_str());
                    }
                }
            } else {
                usage_cache();
                exit(0);
            }
        } else if(strcmp(argv[1], "view") == 0) {
            uint32_t padding = 60;

            if(argc > 2) {
                bool view_2bit = false;

                if(strcmp(argv[2], "--help") == 0 or strcmp(argv[2], "-h") == 0) {
                    usage_view();
                    exit(0);
                }

                bool from_file = false;
                bool skip_argument = false;
                bool allow_masking = true;// allow upper and lower case

                for(int i = 2; i < argc - 1; i++) {
                    if(skip_argument) {
                        skip_argument = false;
                    } else {
                        if(strcmp(argv[i], "-f") == 0 or strcmp(argv[i], "--file") == 0) {
                            from_file = true;
                        } else if(strcmp(argv[i], "-m") == 0 or strcmp(argv[i], "--no-masking") == 0) {
                            allow_masking = false;
                        } else if((strcmp(argv[i], "-p") == 0 or strcmp(argv[i], "--padding") == 0) and i + 1 < argc - 1) {
                            try {
                                sscanf(argv[++i], "%u", &padding);
                            } catch(std::exception const & e) {
                                std::cerr << "ERROR: invalid padding value, must be integer value ranging from 0 to max-int size\n";
                                return EINVAL;
                            }
                            skip_argument = true;// skip next argument, as it is the padding value
                        } else if(strcmp(argv[i], "-2") == 0 or strcmp(argv[i], "--2bit") == 0) {
                            view_2bit = true;
                        }
                    }
                }

                std::string fname;
                if(from_file) {
                    fname = std::string(argv[argc - 1]);
                } else {
                    database d = database();
                    fname = d.get(argv[argc - 1]);

                    if(fname.size() == 0) {
                        std::cout << "Invalid FASTAFS requested\n";
                        return EINVAL;
                    }
                }

                fastafs f = fastafs(std::string(argv[argc - 1]));
                f.load(fname);

                if(view_2bit) {
                    char buffer[READ_BUFFER_SIZE + 1];// = new char [READ_BUFFER_SIZE];
                    uint32_t offset = 0;

                    uint32_t written = f.view_ucsc2bit_chunk(buffer, READ_BUFFER_SIZE, offset);
                    while(written > 0) {
                        std::cout << std::string(buffer, written);
                        offset += written;

                        written = f.view_ucsc2bit_chunk(buffer, READ_BUFFER_SIZE, offset);
                    }
                } else {
                    ffs2f_init* cache = f.init_ffs2f(padding, allow_masking);
                    f.view_fasta(cache);//@todo make argument parsing

                    delete cache;
                }
            } else {
                usage_view();
                return EINVAL;
            }
        } else if(strcmp(argv[1], "info") == 0) {
            if(argc > 2) {
                if(strcmp(argv[2], "--help") == 0 or strcmp(argv[2], "-h") == 0) {
                    usage_info();
                    exit(0);
                }

                bool from_file = false;
                bool ena_verify_checksum = false;

                for(int i = 2; i < argc - 1; i++) {
                    if(strcmp(argv[i], "-f") == 0 or strcmp(argv[i], "--file") == 0) {
                        from_file = true;
                    } else if(strcmp(argv[i], "-e") == 0 or strcmp(argv[i], "--ena-verify") == 0) {
                        ena_verify_checksum = true;
                    } else {
                        usage_info();
                        exit(1);
                    }
                }

                std::string fname;

                if(from_file) {
                    fname = std::string(argv[argc - 1]);
                } else {
                    database d = database();
                    fname = d.get(argv[argc - 1]);

                    if(fname.size() == 0) {
                        std::cout << "Invalid FASTAFS requested\n";
                        return EINVAL;
                    }
                }

                fastafs f = fastafs(std::string(argv[argc - 1]));
                f.load(fname);

                return f.info(ena_verify_checksum);
            } else {
                usage_info();
            }
        } else if(strcmp(argv[1], "mount") == 0) {
            fuse(argc, argv);
        } else if(strcmp(argv[1], "list") == 0) {
            database d = database();
            d.list();
        } else if(strcmp(argv[1], "ps") == 0) {
            std::unordered_multimap<std::string, std::pair<std::string, std::string> > fastafs_fuse_mounts = get_fastafs_processes();
            for(auto n : fastafs_fuse_mounts) {
                std::cout <<  n.second.first <<  "\t" << n.first << "\t" << n.second.second << "\n";
            }
        } else if(strcmp(argv[1], "check") == 0) {
            if(argc > 2) {
                if(strcmp(argv[2], "--help") == 0 or strcmp(argv[2], "-h") == 0) {
                    usage_check();
                    exit(0);
                }
                
                bool from_file = false;
                bool check_md5 = false;

                for(int i = 2; i < argc - 1; i++) {
                    if(strcmp(argv[i], "-f") == 0 or strcmp(argv[i], "--file") == 0) {
                        from_file = true;
                    }
                    else if (strcmp(argv[i], "-5") == 0 or strcmp(argv[i], "--md5") == 0) {
                        check_md5 = true;
                    }
                }

                std::string fname;
                if(from_file) {
                    fname = std::string(argv[argc - 1]);
                } else {
                    database d = database();
                    fname = d.get(argv[argc - 1]);

                    if(fname.size() == 0) {
                        std::cout << "Invalid FASTAFS requested\n";
                        return EINVAL;
                    }
                }

                fastafs f = fastafs(std::string(argv[argc - 1]));
                f.load(fname);

                bool check1 = f.check_file_integrity(true);
                bool check2 = true;
                
                if(check_md5) {
                    check2 = f.check_sequence_integrity(true);
                }
                
                if(check1 and check2) {
                    return 0;
                } else {
                    return EIO;
                }
            } else {
                usage_check();
            }
        } else {
            std::cerr << PACKAGE << ": '" << argv[1] << "' is not a " << PACKAGE << " command. See '" << PACKAGE << " --help':" << std::endl << std::endl;
            usage();

            return EINVAL;
        }
    } else {
        usage();

        return EINVAL;
    }
    return 0;
}

