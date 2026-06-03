#pragma once

#include <filesystem>
#include <boost/test/included/unit_test.hpp>

inline void ensure_tmp_directory() {
    std::filesystem::create_directories("tmp");
}

struct TestFixture {
    TestFixture() {
        ensure_tmp_directory();
    }
};
