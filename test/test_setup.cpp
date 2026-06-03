#define BOOST_TEST_MODULE test_setup
#include <boost/test/included/unit_test.hpp>
#include <filesystem>

struct GlobalTestFixture {
    GlobalTestFixture() {
        std::filesystem::create_directories("tmp");
    }

    ~GlobalTestFixture() {
    }
};

BOOST_TEST_GLOBAL_FIXTURE(GlobalTestFixture);
