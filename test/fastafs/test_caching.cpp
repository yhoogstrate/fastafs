#define BOOST_TEST_MODULE fastfs_caching

#include <sys/types.h>
#include <sys/stat.h>


#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_fastafs.hpp"
#include "fastafs.hpp"



// @todo to utils?
void flush_buffer(char *buffer, size_t n, char fill)
{
    for(size_t i = 0; i < n; i++) {
        buffer[i] = fill;
    }
}



BOOST_AUTO_TEST_SUITE(Testing)

BOOST_AUTO_TEST_CASE(init)
{
    // make tmp dir
    mkdir("tmp", S_IRWXU);
}


BOOST_AUTO_TEST_CASE(test_fastafs_seq_caching)
{
    // 1: create FASTAFS file
    std::string fastafs_file = "tmp/test_003.fastafs";
    fasta_to_fastafs("test/data/test_003.fa", fastafs_file, false);
    fastafs fs = fastafs("test");
    fs.load(fastafs_file);
    BOOST_REQUIRE(fs.data.size() == 4);
    
    BOOST_CHECK_EQUAL(fs.data[0]->fasta_filesize(60), 527); // x-checked :)
    
    //BOOST_CHECK_EQUAL(fs.data[0]->n_starts.size(), 77);
    //BOOST_CHECK_EQUAL(fs.data[0]->n_ends.size(), 77);
    
    ffs2f_init* cache_p60 = fs.init_ffs2f(60, true);


    const int READ_BUFFER_SIZE_F = 4096 ; // make sure it is large enough, error occurrsed with buf len=4096
    char* buffer = new char[READ_BUFFER_SIZE_F + 2];
    uint32_t ret;

    // test the first read
    chunked_reader fh1 = chunked_reader(fs.filename.c_str());
    flush_buffer(buffer, READ_BUFFER_SIZE_F + 1, '\0');
    ret = fs.view_fasta_chunk(cache_p60, buffer, 20, 0, fh1);
    
    printf("[%s]\n", buffer);
    
    ret = fs.view_fasta_chunk(cache_p60, buffer, 20, 20, fh1);
    
    printf("[%s]\n", buffer);
    
    

    //printf("[ %d ] \n", cache_p60->sequences[0]->n_starts.size() );
    //printf("[ %d ] \n", cache_p60->sequences[0]->n_ends.size() );
    
    //for(int i = 0; i < cache_p60->sequences[0]->n_starts.size(); i++) {
        //printf("[ %d ]  ->  [ %d , %d ] \n", i, cache_p60->sequences[0]->n_starts[i] , cache_p60->sequences[0]->n_ends[i] );
    //}
}





BOOST_AUTO_TEST_SUITE_END()




