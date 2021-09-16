#define BOOST_TEST_MODULE fastafs_fastafs

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





BOOST_AUTO_TEST_SUITE_END()



