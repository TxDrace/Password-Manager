#pragma once
#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <iostream>
#include <sodium.h>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string get_environment_variable(std::string environmentVarKey);
std::unique_ptr<unsigned char[]> hex2bin(const char* hex, size_t hex_len, size_t* bin_len);
std::unique_ptr<char[]> bin2hex(const unsigned char* bin, size_t bin_len, size_t& hex_len);
std::string generateId();

#endif