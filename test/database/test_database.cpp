#define BOOST_TEST_MODULE database

#include <boost/test/included/unit_test.hpp>

#include "config.hpp"

#include "database.hpp"

#include <filesystem>
#include <fstream>


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
    std::string tmpdir = "tmp/test_db_basic";
    std::filesystem::remove_all(tmpdir);

    database d(tmpdir);

    BOOST_CHECK(std::filesystem::exists(tmpdir));
    BOOST_CHECK(std::filesystem::exists(tmpdir + "/index"));

    std::filesystem::remove_all(tmpdir);
}


BOOST_AUTO_TEST_CASE(test_add_duplicate_throws)
{
    std::string tmpdir = "tmp/test_duplicate";
    std::filesystem::remove_all(tmpdir);
    std::filesystem::create_directories(tmpdir);

    database d(tmpdir);
    d.add((char*)"test_entry");

    std::ofstream dummy(tmpdir + "/test_entry.fastafs");
    dummy.close();

    BOOST_CHECK_THROW(d.add((char*)"test_entry"), std::runtime_error);

    std::filesystem::remove_all(tmpdir);
}


BOOST_AUTO_TEST_CASE(test_refresh_removes_orphans)
{
    std::string tmpdir = "tmp/test_orphans";
    std::filesystem::remove_all(tmpdir);
    std::filesystem::create_directories(tmpdir);

    database d(tmpdir);
    d.add((char*)"valid_entry");
    d.add((char*)"orphan_entry");

    // Verwijder het bestand van orphan_entry
    std::filesystem::remove(tmpdir + "/orphan_entry.fastafs");
    std::filesystem::remove(tmpdir + "/orphan_entry.fastafs.zst");

    d.refresh();

    // Na refresh: orphan_entry mag niet meer in index
    std::string result = d.get((char*)"orphan_entry");
    BOOST_CHECK_EQUAL(result, "");

    // valid_entry moet nog bestaan
    BOOST_CHECK(d.get((char*)"valid_entry").size() > 0);

    std::filesystem::remove_all(tmpdir);
}


BOOST_AUTO_TEST_CASE(test_refresh_deduplicates)
{
    std::string tmpdir = "tmp/test_duplicates";
    std::filesystem::remove_all(tmpdir);
    std::filesystem::create_directories(tmpdir);

    database d(tmpdir);
    d.add((char*)"entry1");

    // Voeg handmatig duplicaten toe aan index
    std::ofstream indexfile(tmpdir + "/index", std::ios::app);
    indexfile << "entry1\n";
    indexfile << "entry1\n";
    indexfile.close();

    d.refresh();

    // Index mag entry1 maar één keer bevatten
    std::ifstream infile(tmpdir + "/index");
    std::string line;
    int count = 0;
    while(std::getline(infile, line)) {
        if(line == "entry1") count++;
    }
    infile.close();

    BOOST_CHECK_EQUAL(count, 1);

    std::filesystem::remove_all(tmpdir);
}


BOOST_AUTO_TEST_SUITE_END()
