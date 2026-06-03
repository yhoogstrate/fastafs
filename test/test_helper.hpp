#pragma once

#include <filesystem>
#include <fstream>
#include <string>

#include <openssl/evp.h>

#include <boost/test/included/unit_test.hpp>

#include "utils.hpp"

inline void ensure_tmp_directory() {
    std::filesystem::create_directories("tmp");
}

// compute the MD5 hex digest of a file's full contents
inline std::string file_md5(const std::string &path) {
    std::ifstream fh(path, std::ios::in | std::ios::binary);
    BOOST_REQUIRE_MESSAGE(fh.is_open(), "could not open file for md5: " + path);

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_md5(), nullptr);

    char buf[4096];
    while(fh.read(buf, sizeof(buf)) or fh.gcount() > 0) {
        EVP_DigestUpdate(ctx, buf, (size_t) fh.gcount());
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;
    EVP_DigestFinal_ex(ctx, digest, &digest_len);
    EVP_MD_CTX_free(ctx);

    char hash[33];
    md5_digest_to_hash(digest, hash);
    return std::string(hash);
}

struct TestFixture {
    TestFixture() {
        ensure_tmp_directory();
    }
};
