#pragma once
#include <sodium.h>
#include "SHA256.h"
#include "AEGIS256.h"
#include <chrono>
#include <iomanip>
#include <sstream>


std::unique_ptr<unsigned char[]> hex2bin(const char* hex, size_t hex_len, size_t* bin_len) 
{
    size_t bin_max_len = hex_len / 2;
    std::unique_ptr<unsigned char[]> bin(new unsigned char[bin_max_len]);
    sodium_hex2bin(bin.get(), bin_max_len, hex, hex_len, NULL, bin_len, NULL);
    return bin;
}

std::unique_ptr<char[]> bin2hex(const unsigned char* bin, size_t bin_len)
{
    size_t hex_max_len = bin_len * 2 + 1;
    std::unique_ptr<char[]> hex(new char[hex_max_len]);
    sodium_bin2hex(hex.get(), hex_max_len, bin, bin_len);
    return hex;
}

std::string generateId()
{
    const auto now = std::chrono::system_clock::now();
    const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t_c);

    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y%m%d%H%M%S");
    return oss.str();
}

int main()
{
	if (sodium_init() == -1)
	{
		return -1;
	}


    //std::string npub = "2d0717720fe1d78727a943e256d0e5ffdf4a14c656e78c661a3ae665388024a9";
    //std::unique_ptr<char[]> npub_hex = string2char<char>(npub);
    //size_t npub_bin_len;
    //std::unique_ptr<unsigned char[]> npub_bin = hex2bin(npub_hex.get(), npub.size(), &npub_bin_len);

    //std::unique_ptr<char[]> res = bin2hex(npub_bin.get(), npub_bin_len);

    
    return 0;
}

