#define BOOST_TEST_MODULE byte_decoder

#include <boost/test/included/unit_test.hpp>

#include "byte_decoder.hpp"


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_byte_decoder_test_001)
{
    byte_decoder b;
    
    std::string aa("AAA");
    b.set_input_data((const unsigned char*) aa.c_str(), aa.size());
}




BOOST_AUTO_TEST_SUITE_END()
