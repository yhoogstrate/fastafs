#define BOOST_TEST_MODULE byte_encoder

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"
#include "byte_encoder.hpp"


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_byte_encoder_twobit_dna_single_chunk)
{
    // Encode "AAAA" (4 A's) → 0b10101010 = 170
    char input[4] = {'A', 'A', 'A', 'A'};
    unsigned char output[1];

    byte_encoder enc(std::make_unique<encoder_interface_twobit_dna>());
    enc.set_output_buffer(output, 1);

    size_t written = enc.encode(input, 4);

    BOOST_CHECK_EQUAL(written, 1);
    BOOST_CHECK_EQUAL(output[0], 170);
}


BOOST_AUTO_TEST_CASE(test_byte_encoder_twobit_dna_multi_chunk)
{
    // Encode "AAAATTTTGGGG" (3 chunks of 4)
    // AAA A → 170
    // TTT T → 0
    // GGG G → 255
    char input[12] = {'A', 'A', 'A', 'A', 'T', 'T', 'T', 'T', 'G', 'G', 'G', 'G'};
    unsigned char output[3];

    byte_encoder enc(std::make_unique<encoder_interface_twobit_dna>());
    enc.set_output_buffer(output, 3);

    size_t written = enc.encode(input, 12);

    BOOST_CHECK_EQUAL(written, 3);
    BOOST_CHECK_EQUAL(output[0], 170);  // AAAA
    BOOST_CHECK_EQUAL(output[1], 0);    // TTTT
    BOOST_CHECK_EQUAL(output[2], 255);  // GGGG
}


BOOST_AUTO_TEST_CASE(test_byte_encoder_twobit_rna_single_chunk)
{
    // Encode "UUUU" (4 U's) → 0b00000000 = 0 (same as DNA T: U=00)
    char input[4] = {'U', 'U', 'U', 'U'};
    unsigned char output[1];

    byte_encoder enc(std::make_unique<encoder_interface_twobit_rna>());
    enc.set_output_buffer(output, 1);

    size_t written = enc.encode(input, 4);

    BOOST_CHECK_EQUAL(written, 1);
    BOOST_CHECK_EQUAL(output[0], 0);
}


BOOST_AUTO_TEST_CASE(test_byte_encoder_fourbit_single_chunk)
{
    // Encode "AC" (A=0, C=1) → 0x01 (C in low 4 bits, A in high)
    char input[2] = {'A', 'C'};
    unsigned char output[1];

    byte_encoder enc(std::make_unique<encoder_interface_fourbit>());
    enc.set_output_buffer(output, 1);

    size_t written = enc.encode(input, 2);

    BOOST_CHECK_EQUAL(written, 1);
    BOOST_CHECK_EQUAL(output[0], 1);  // A (0000) in high, C (0001) in low
}


BOOST_AUTO_TEST_CASE(test_byte_encoder_fourbit_multi_chunk)
{
    // Encode "ACGTACGT" (4 chunks of 2)
    char input[8] = {'A', 'C', 'G', 'T', 'A', 'C', 'G', 'T'};
    unsigned char output[4];

    byte_encoder enc(std::make_unique<encoder_interface_fourbit>());
    enc.set_output_buffer(output, 4);

    size_t written = enc.encode(input, 8);

    BOOST_CHECK_EQUAL(written, 4);
}


BOOST_AUTO_TEST_CASE(test_byte_encoder_fivebit_single_chunk)
{
    // Encode "FASTA-FS" (F=5, A=0, S=18, T=19, A=0, -=27, F=5, S=18)
    // Should produce 5 compressed bytes
    char input[8] = {'F', 'A', 'S', 'T', 'A', '-', 'F', 'S'};
    unsigned char output[5];

    byte_encoder enc(std::make_unique<encoder_interface_fivebit>());
    enc.set_output_buffer(output, 5);

    size_t written = enc.encode(input, 8);

    BOOST_CHECK_EQUAL(written, 5);
    // Verify against known compressed representation
    BOOST_CHECK_EQUAL(output[0], 40);
    BOOST_CHECK_EQUAL(output[1], 37);
    BOOST_CHECK_EQUAL(output[2], 48);
    BOOST_CHECK_EQUAL(output[3], 108);
    BOOST_CHECK_EQUAL(output[4], 178);
}


BOOST_AUTO_TEST_CASE(test_byte_encoder_set_strategy)
{
    // Start with twobit_dna, then switch to twobit_rna
    byte_encoder enc(std::make_unique<encoder_interface_twobit_dna>());
    unsigned char output[2];

    // First: encode with DNA
    enc.set_output_buffer(output, 2);
    char dna[4] = {'A', 'A', 'A', 'A'};
    size_t written1 = enc.encode(dna, 4);
    BOOST_CHECK_EQUAL(written1, 1);
    BOOST_CHECK_EQUAL(output[0], 170);

    // Switch to RNA strategy
    enc.set_strategy(std::make_unique<encoder_interface_twobit_rna>());
    enc.set_output_buffer(output, 2);
    char rna[4] = {'U', 'U', 'U', 'U'};
    size_t written2 = enc.encode(rna, 4);
    BOOST_CHECK_EQUAL(written2, 1);
    BOOST_CHECK_EQUAL(output[0], 0);
}


#ifdef DEBUG
BOOST_AUTO_TEST_CASE(test_byte_encoder_invalid_input_size)
{
    // Input size must be multiple of nucleotides_per_chunk()
    byte_encoder enc(std::make_unique<encoder_interface_twobit_dna>());
    unsigned char output[2];
    enc.set_output_buffer(output, 2);

    char input[3] = {'A', 'A', 'A'};  // 3, not divisible by 4

    // Should throw in DEBUG mode
    BOOST_CHECK_THROW(enc.encode(input, 3), std::invalid_argument);
}


BOOST_AUTO_TEST_CASE(test_byte_encoder_insufficient_output_buffer)
{
    // Output buffer too small — encode should stop
    byte_encoder enc(std::make_unique<encoder_interface_twobit_dna>());
    unsigned char output[1];  // Only 1 byte
    enc.set_output_buffer(output, 1);

    char input[8] = {'A', 'A', 'A', 'A', 'T', 'T', 'T', 'T'};  // Needs 2 bytes

    size_t written = enc.encode(input, 8);

    BOOST_CHECK_EQUAL(written, 1);  // Only 1 byte written
    BOOST_CHECK_EQUAL(output[0], 170);
}
#endif


BOOST_AUTO_TEST_SUITE_END()

