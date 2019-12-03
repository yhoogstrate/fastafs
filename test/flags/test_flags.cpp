#define BOOST_TEST_MODULE flags

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "flags.hpp"


//#include <iostream>
//#include <fstream>


BOOST_AUTO_TEST_SUITE(Testing)


BOOST_AUTO_TEST_CASE(test_fastafs_flags)
{
	fastafs_flags f;
	
	char buffer[2 + 1];
	buffer[2] = '\0';
	
	// test: 00000000 00000000
	buffer[0] = '\x0';
	buffer[1] = '\x0';
	f.set(buffer);
	
    BOOST_CHECK_EQUAL(f.is_complete(), false);
    BOOST_CHECK_EQUAL(f.is_incomplete(), true);


	// test: 10000000 00000000
	buffer[0] = '\x80'; // worked with writing to file and checking with `xxd -b file` ~ this is binary equivalent to 10000000
	buffer[1] = '\x0';
	f.set(buffer);
	
    BOOST_CHECK_EQUAL(f.is_complete(), true);
    BOOST_CHECK_EQUAL(f.is_incomplete(), false);


	// test: 11111111 00000000
	buffer[0] = '\xFF';
	buffer[1] = '\x0';
	f.set(buffer);
	
    BOOST_CHECK_EQUAL(f.is_complete(), true);
    BOOST_CHECK_EQUAL(f.is_incomplete(), false);

	// test: 00000001 00000000
	buffer[0] = '\x01';
	buffer[1] = '\x0';
	f.set(buffer);
	
    BOOST_CHECK_EQUAL(f.is_complete(), false);
    BOOST_CHECK_EQUAL(f.is_incomplete(), true);



	// re-test: 00000000 00000000
	buffer[0] = '\x0';
	buffer[1] = '\x0';
	f.set(buffer);
	
    BOOST_CHECK_EQUAL(f.is_complete(), false);
    BOOST_CHECK_EQUAL(f.is_incomplete(), true);
    
	//f.set_complete();
    //BOOST_CHECK_EQUAL(f.is_complete(), true);
    //BOOST_CHECK_EQUAL(f.is_incomplete(), false);
}




BOOST_AUTO_TEST_SUITE_END()
