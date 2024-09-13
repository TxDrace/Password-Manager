#pragma once

#ifndef SHA256_H
#define SHA256_H

#include <iostream>
#include <sodium.h>
#include "convert.h"

class SHA256
{
private:
    std::unique_ptr<unsigned char[]> data;
    size_t data_len;

    std::unique_ptr<unsigned char[]> hash_bin;
    size_t hash_bin_len;

    std::unique_ptr<char[]> hex;
    size_t hex_len;

public:
    SHA256(std::string);

    ~SHA256();

    std::string hash() const;
};

#endif // SHA256_H

