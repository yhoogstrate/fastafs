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




// Helper function to test a FASTA file against expected MD5 sums
void test_fasta_file(const std::string& input_fasta, const std::vector<std::string>& expected_md5s)
{
    std::string basename = input_fasta.substr(input_fasta.find_last_of("/") + 1);
    basename = basename.substr(0, basename.find_last_of("."));
    std::string fastafs_file = "tmp/" + basename + ".fastafs";

    fasta_to_fastafs(input_fasta, fastafs_file, false);
    fastafs fs = fastafs(basename);
    fs.load(fastafs_file);

    ffs2f_init* cache = fs.init_ffs2f(0, false);
    BOOST_REQUIRE(fs.data.size() > 0);
    BOOST_REQUIRE_EQUAL(fs.data.size(), expected_md5s.size());

    chunked_reader file = chunked_reader(fs.filename.c_str());
    file.fopen(0);

    for (size_t i = 0; i < fs.data.size(); i++) {
        std::string actual = fs.data[i]->md5(cache->sequences[i], file);
        BOOST_CHECK_MESSAGE(actual == expected_md5s[i],
            "Sequence " << i << " MD5 mismatch: got '" << actual << "' expected '" << expected_md5s[i] << "'");
    }

    delete cache;
}




// Encoding type tests
BOOST_AUTO_TEST_CASE(test_md5_dna_uppercase)
{
    test_fasta_file("test/data/md5_test_dna_uppercase.fa", {"e78473afbfa729df9baf1e3fd9f24244"});
}

BOOST_AUTO_TEST_CASE(test_md5_dna_lowercase)
{
    test_fasta_file("test/data/md5_test_dna_lowercase.fa", {"e78473afbfa729df9baf1e3fd9f24244"});
}

BOOST_AUTO_TEST_CASE(test_md5_dna_mixed_case)
{
    test_fasta_file("test/data/md5_test_dna_mixed_case.fa", {"e78473afbfa729df9baf1e3fd9f24244"});
}

BOOST_AUTO_TEST_CASE(test_md5_dna_n_stretches)
{
    test_fasta_file("test/data/md5_test_dna_n_stretches.fa", {"9861391b0958ad3056cc13b00ba6d3ac"});
}

BOOST_AUTO_TEST_CASE(test_md5_rna_sequence)
{
    test_fasta_file("test/data/md5_test_rna_sequence.fa", {"435c5b16aeff3546152caba461185615"});
}

BOOST_AUTO_TEST_CASE(test_md5_dna_t_and_u)
{
    test_fasta_file("test/data/md5_test_dna_t_and_u.fa", {"87ab7acca360c8a44fde903213475d6d"});
}

BOOST_AUTO_TEST_CASE(test_md5_iupac_ambiguity)
{
    test_fasta_file("test/data/md5_test_iupac_ambiguity.fa", {"08b667953fb24dea54e86862170cf4ae"});
}

BOOST_AUTO_TEST_CASE(test_md5_fourbit_with_gap)
{
    test_fasta_file("test/data/md5_test_fourbit_with_gap.fa", {"cae822e1fe221decd462986babd41f91"});
}

BOOST_AUTO_TEST_CASE(test_md5_protein_sequence)
{
    test_fasta_file("test/data/md5_test_protein_sequence.fa", {"db205733182fe7f95090e1211b5b21fc"});
}

BOOST_AUTO_TEST_CASE(test_md5_protein_with_stop)
{
    test_fasta_file("test/data/md5_test_protein_with_stop.fa", {"a2744d38f7acd136d63c571ef3ec3bc3"});
}

BOOST_AUTO_TEST_CASE(test_md5_protein_with_gap)
{
    test_fasta_file("test/data/md5_test_protein_with_gap.fa", {"2786dfe828a2307e9636051dd55957a1"});
}

// Multi-sequence tests
BOOST_AUTO_TEST_CASE(test_md5_multiple_dna)
{
    test_fasta_file("test/data/md5_test_multiple_dna.fa", {"cae822e1fe221decd462986babd41f91", 
        "5725e2de858089c362acc2f9bc30e431", 
        "eed143a270737191167432866d5e6b94"});
}

BOOST_AUTO_TEST_CASE(test_md5_mixed_types)
{
    test_fasta_file("test/data/md5_test_mixed_types.fa", {"cae822e1fe221decd462986babd41f91", 
        "638ef73a7502450731f6bfb2c2dd8747"});
}

// Edge cases
BOOST_AUTO_TEST_CASE(test_md5_empty_sequence)
{
    std::string basename = "test_empty_sequence";
    std::string input_fasta = "test/data/md5_test_empty_sequence.fa";
    std::string fastafs_file = "tmp/" + basename + ".fastafs";

    fasta_to_fastafs(input_fasta, fastafs_file, false);
    fastafs fs = fastafs(basename);
    fs.load(fastafs_file);

    // Empty sequence should have n = 0
    BOOST_CHECK_EQUAL(fs.data.size(), 1);
    BOOST_CHECK_EQUAL(fs.data[0]->n, 0);
}

BOOST_AUTO_TEST_CASE(test_md5_exact_line_boundary_60nt)
{
    test_fasta_file("test/data/md5_test_exact_line_boundary_60nt.fa", {"e78473afbfa729df9baf1e3fd9f24244"});
}

BOOST_AUTO_TEST_CASE(test_md5_short_sequence)
{
    test_fasta_file("test/data/md5_test_short_sequence.fa", {"7fc56270e7a70fa81a5935b72eacbe29",
         "4144e097d2fa7a491cec2a7a4322f2bc", "33f786e15eb427ffd3edec16cfdc0cd2"});
}

// Newline diversity tests
BOOST_AUTO_TEST_CASE(test_md5_line_length_30nt)
{
    test_fasta_file("test/data/md5_test_line_length_30nt.fa", {"1318db83731c295785dfe5c66c31d5df"});
}

BOOST_AUTO_TEST_CASE(test_md5_line_length_80nt)
{
    test_fasta_file("test/data/md5_test_line_length_80nt.fa", {"f2328202517bf3c236a5f33cede89408"});
}

BOOST_AUTO_TEST_CASE(test_md5_line_length_120nt)
{
    test_fasta_file("test/data/md5_test_line_length_120nt.fa", {"092d2d847566b2d47197a37551b9148e"});
}

BOOST_AUTO_TEST_CASE(test_md5_line_length_1nt_per_line)
{
    test_fasta_file("test/data/md5_test_line_length_1nt_per_line.fa", {"cc0af3a4fedb18378b4b57b98068e69f"});
}

BOOST_AUTO_TEST_CASE(test_md5_line_boundary_59nt)
{
    test_fasta_file("test/data/md5_test_line_boundary_59nt.fa", {"e56d569c0a26b4841c9761ae4684a964"});
}

BOOST_AUTO_TEST_CASE(test_md5_line_boundary_61nt)
{
    test_fasta_file("test/data/md5_test_line_boundary_61nt.fa", {"5622e692d0035b564db02fcdea1ef85b"});
}

BOOST_AUTO_TEST_CASE(test_md5_line_length_mixed)
{
    test_fasta_file("test/data/md5_test_line_length_mixed.fa", {"67f85e5e9beb343d75f2b537f6b9ef39"});
}

BOOST_AUTO_TEST_CASE(test_md5_no_final_newline)
{
    test_fasta_file("test/data/md5_test_no_final_newline.fa", {"e78473afbfa729df9baf1e3fd9f24244"});
}

BOOST_AUTO_TEST_CASE(test_md5_multiple_blank_lines)
{
    test_fasta_file("test/data/md5_test_multiple_blank_lines.fa", {"864d36199e27f0426baf6fa1e03bd9ad"});
}

BOOST_AUTO_TEST_CASE(test_md5_single_line_very_long)
{
    test_fasta_file("test/data/md5_test_single_line_very_long.fa", {"3da1eedcd268014027e11ac435e23efa"});
}



BOOST_AUTO_TEST_SUITE_END()
