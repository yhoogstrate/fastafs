#define BOOST_TEST_MODULE database

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "database.hpp"


//#include <iostream>
//#include <fstream>


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_database__01)
{
    const std::string default_dir_1 = database::get_default_dir();
    unsetenv("HOME");
    const std::string default_dir_2 = database::get_default_dir();
    
    BOOST_REQUIRE(default_dir_1.size() > 0);
    BOOST_REQUIRE(default_dir_2.size() > 0);
    
    BOOST_CHECK_EQUAL(default_dir_1, default_dir_2);
    // printf("[%s]==[%s]\n", default_dir_1.c_str(), default_dir_2.c_str());
}


BOOST_AUTO_TEST_CASE(test_database__02)
{
    database d("tmp/database");
}




BOOST_AUTO_TEST_SUITE_END()
