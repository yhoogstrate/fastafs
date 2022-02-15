#define BOOST_TEST_MODULE database

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "database.hpp"


//#include <iostream>
//#include <fstream>


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_database__01)
{
    database d(".");
}



BOOST_AUTO_TEST_SUITE_END()
