#pragma once
#ifndef AEGIS256_H
#define AEGIS256_H

#include <iostream>
#include "assert.h"
#include "SHA256.h"
#include "convert.h"
#include <string>
#include "Helper.h"

class AEGIS256_Encrypt
{
private:
	std::unique_ptr<unsigned char[]> cipher_bin;
	std::unique_ptr<char[]> cipher_hex;
	std::unique_ptr<unsigned char[]> message;
	std::unique_ptr<unsigned char[]> key;
	std::unique_ptr<unsigned char[]> nonce_public;

	size_t cipher_bin_len;
	size_t cipher_hex_len;
	size_t message_len;
	size_t nonce_public_len;

public:
	AEGIS256_Encrypt(const std::string& key, const std::string& message);
	~AEGIS256_Encrypt();

	std::string encrypt();
	std::string getNoncePublic_Hex();
};


class AEGIS256_Decrypt
{
private:
	std::unique_ptr<unsigned char[]> cipher_bin;
	std::unique_ptr<unsigned char[]> nonce_public_bin;
	std::unique_ptr<unsigned char[]> message;
	std::unique_ptr<unsigned char[]> key;

	std::unique_ptr<char[]> cipher_hex;
	std::unique_ptr<char[]> nonce_public_hex;

	size_t cipher_bin_len;
	size_t cipher_hex_len;
	size_t message_len;
	size_t nonce_public_hex_len;
	size_t nonce_public_bin_len;

public:
	AEGIS256_Decrypt(const std::string& key, const std::string& cipher_hex, const std::string& nonce_public_hex);
	~AEGIS256_Decrypt();

	std::string decrypt();
};

#endif // AEGIS256_H
