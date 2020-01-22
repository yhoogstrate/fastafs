#define BOOST_TEST_MODULE fastfs_check

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "fasta_to_twobit_fastafs.hpp"
#include "fasta_to_fourbit_fastafs.hpp"



BOOST_AUTO_TEST_SUITE(Testing)

/**
 * @brief
 *
 * @test
 */
BOOST_AUTO_TEST_CASE(test_file_integrity)
{
    fasta_to_twobit_fastafs("test/data/test.fa", "tmp/test_cache_test.fastafs");

    // check computed file size
    fastafs f = fastafs("");
    f.load("tmp/test_cache_test.fastafs");
    BOOST_REQUIRE_EQUAL(f.fastafs_filesize(), 403);

    BOOST_CHECK_EQUAL(f.check_sequence_integrity(false), true);
    BOOST_CHECK_EQUAL(f.check_file_integrity(false), true);

    for(int i = 5; i < 403 - 5 - 1 - 1 ; i ++) {
        char buffer[400];

        std::string tmp_file = "tmp/test_cache_test_" + std::to_string(i) + ".fastafs";
        std::ifstream fh_fastafs_in("tmp/test_cache_test.fastafs", std::ios::out | std::ios::binary);
        std::ofstream fh_fastafs_out(tmp_file, std::ios::out | std::ios::binary);

        fh_fastafs_in.read(buffer, i);
        fh_fastafs_out.write(reinterpret_cast<char *>(&buffer), i);

        // modify the i-th base to something else
        fh_fastafs_in.read(buffer, 1);
        if(buffer[0] == '\x01') {
            buffer[0] = '\x02';
        } else {
            buffer[0] = '\x01';
        }
        fh_fastafs_out.write(reinterpret_cast<char *>(&buffer), 1);

        fh_fastafs_in.read(buffer, 403 - i - 1);
        fh_fastafs_out.write(reinterpret_cast<char *>(&buffer), 403 - i - 1);

        fh_fastafs_in.close();
        fh_fastafs_out.close();


        fastafs f = fastafs("");
        f.filename = tmp_file; // don't load
        BOOST_CHECK_EQUAL(f.check_file_integrity(false), false);

        // make a new loop and only test those that are affected, as the checksums can then be loaded appropriately
        if(f.check_sequence_integrity(true)) {
            printf("[%i][!true]\n", i);
        } else {
            printf("[%i][!false]\n", i);
        }

    }
}





BOOST_AUTO_TEST_SUITE_END()
