#include "Helper.h"

std::string get_environment_variable(std::string environmentVarKey)
{
	char* pBuffer = nullptr;
	size_t size = 0;
	auto key = environmentVarKey.c_str();
	// Use the secure version of getenv, ie. _dupenv_s to fetch environment variable.
	if (_dupenv_s(&pBuffer, &size, key) == 0 && pBuffer != nullptr)
	{
		std::string environmentVarValue(pBuffer);
		free(pBuffer);
		return environmentVarValue;
	}
	else
	{
		throw std::runtime_error("Environment variable not found: " + environmentVarKey);
	}
}

std::unique_ptr<unsigned char[]> hex2bin(const char* hex, size_t hex_len, size_t* bin_len) 
{
	size_t bin_max_len = hex_len / 2;
	std::unique_ptr<unsigned char[]> bin(new unsigned char[bin_max_len]);
	sodium_hex2bin(bin.get(), bin_max_len, hex, hex_len, NULL, bin_len, NULL);
	return bin;
}

std::unique_ptr<char[]> bin2hex(const unsigned char* bin, size_t bin_len, size_t& hex_len)
{
	hex_len = bin_len * 2 + 1; // According to https://doc.libsodium.org/helpers#hexadecimal-encoding-decoding
	std::unique_ptr<char[]> hex(new char[hex_len]);
	sodium_bin2hex(hex.get(), hex_len, bin, bin_len);
	return hex;
}