#define BOOST_TEST_MODULE sequence_region

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "SequenceRegion.hpp"


//#include <iostream>
//#include <fstream>


BOOST_AUTO_TEST_SUITE(Testing)

/*
 * Goal is to parse the following strings:
 * "chr1"
 * "chr1:"
 * "chr1:123" # single base
 * "chr1:123-"
 * "chr1:123-456"
 * "chr1:-456" == "chr1:0-456"

 * "chr1:123-456:asdasd" error
 */


BOOST_AUTO_TEST_CASE(test_sequence_region)
{
    {
        char arg[] = "/seq/chr1";
        SequenceRegion sr = SequenceRegion(&(arg[5]));
        
        BOOST_CHECK_EQUAL(sr.seq_name , "chr1");
        BOOST_CHECK_EQUAL(sr.has_defined_end , false); // not defined; sequence's end
    }

    {
        char arg[] = "/seq/chr1:";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chr1");
        BOOST_CHECK_EQUAL(sr.has_defined_end , false); // not defined; sequence's end
    }

    {
        char arg[] = "/seq/chr1:123";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chr1");
        BOOST_CHECK_EQUAL(sr.start , 123);

        BOOST_CHECK_EQUAL(sr.has_defined_end , true);
        BOOST_CHECK_EQUAL(sr.end , 123);
    }

    {
        char arg[] = "/seq/chr1:-123";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chr1");
        BOOST_CHECK_EQUAL(sr.start , 0);

        BOOST_CHECK_EQUAL(sr.has_defined_end , true);
        BOOST_CHECK_EQUAL(sr.end , 123);
    }
    
    {
        char arg[] = "/seq/chr1:123-456";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chr1");
        BOOST_CHECK_EQUAL(sr.start , 123);

        BOOST_CHECK_EQUAL(sr.has_defined_end , true);
        BOOST_CHECK_EQUAL(sr.end , 456);
    }


    {
        char arg[] = "/seq/chr1:123-";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chr1");
        BOOST_CHECK_EQUAL(sr.start , 123);

        BOOST_CHECK_EQUAL(sr.has_defined_end , false);
        //BOOST_CHECK_EQUAL(sr.end , 456); - underfined
    }

    {
        char arg[] = "/seq/chr1:456-123";

        SequenceRegion *sr = nullptr;
        if(sr == nullptr) {// compiler doesn't understand this otherwise
            BOOST_CHECK_THROW(sr = new SequenceRegion(&(arg[5])) , std::invalid_argument);
        }
    }

}



BOOST_AUTO_TEST_CASE(test_sequence_region3)
{
    {
        char arg[] = "/seq/chrRr1";
        SequenceRegion sr = SequenceRegion(&(arg[5]));
        
        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.has_defined_end , false); // not defined; sequence's end
    }

    {
        char arg[] = "/seq/chrRr1:";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.has_defined_end , false); // not defined; sequence's end
    }

    {
        char arg[] = "/seq/chrRr1:1234";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.start , 1234);

        BOOST_CHECK_EQUAL(sr.has_defined_end , true);
        BOOST_CHECK_EQUAL(sr.end , 1234);
    }

    {
        char arg[] = "/seq/chrRr1:-1234";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.start , 0);

        BOOST_CHECK_EQUAL(sr.has_defined_end , true);
        BOOST_CHECK_EQUAL(sr.end , 1234);
    }
    
    {
        char arg[] = "/seq/chrRr1:1234-1235";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.start , 1234);

        BOOST_CHECK_EQUAL(sr.has_defined_end , true);
        BOOST_CHECK_EQUAL(sr.end , 1235);
    }


    {
        char arg[] = "/seq/chrRr1:1234-";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.start , 1234);

        BOOST_CHECK_EQUAL(sr.has_defined_end , false);
        //BOOST_CHECK_EQUAL(sr.end , 1235); - underfined
    }

    {
        char arg[] = "/seq/chrRr1:1235-1234";

        SequenceRegion *sr = nullptr;
        if(sr == nullptr) {// compiler doesn't understand this otherwise
            BOOST_CHECK_THROW(sr = new SequenceRegion(&(arg[5])) , std::invalid_argument);
        }
    }

}




BOOST_AUTO_TEST_CASE(test_sequence_region2)
{
    {
        char arg[] = "/seq/chrRr1";
        SequenceRegion sr = SequenceRegion(&(arg[5]));
        
        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.has_defined_end , false); // not defined; sequence's end
    }

    {
        char arg[] = "/seq/chrRr1:";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.has_defined_end , false); // not defined; sequence's end
    }

    {
        char arg[] = "/seq/chrRr1:123";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.start , 123);

        BOOST_CHECK_EQUAL(sr.has_defined_end , true);
        BOOST_CHECK_EQUAL(sr.end , 123);
    }

    {
        char arg[] = "/seq/chrRr1:-123";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.start , 0);

        BOOST_CHECK_EQUAL(sr.has_defined_end , true);
        BOOST_CHECK_EQUAL(sr.end , 123);
    }
    
    {
        char arg[] = "/seq/chrRr1:123-456";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.start , 123);

        BOOST_CHECK_EQUAL(sr.has_defined_end , true);
        BOOST_CHECK_EQUAL(sr.end , 456);
    }


    {
        char arg[] = "/seq/chrRr1:123-";
        SequenceRegion sr = SequenceRegion(&(arg[5]));

        BOOST_CHECK_EQUAL(sr.seq_name , "chrRr1");
        BOOST_CHECK_EQUAL(sr.start , 123);

        BOOST_CHECK_EQUAL(sr.has_defined_end , false);
        //BOOST_CHECK_EQUAL(sr.end , 456); - underfined
    }

    {
        char arg[] = "/seq/chrRr1:456-123";

        SequenceRegion *sr = nullptr;
        if(sr == nullptr) {// compiler doesn't understand this otherwise
            BOOST_CHECK_THROW(sr = new SequenceRegion(&(arg[5])) , std::invalid_argument);
        }
    }

}



BOOST_AUTO_TEST_SUITE_END()
