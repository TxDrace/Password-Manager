#include "SHA256.h"

SHA256::SHA256(std::string in)
{
	this->data_len = in.size();
	this->data = string2char<unsigned char>(in);

	this->hash_bin_len = crypto_hash_sha256_BYTES;
	this->hash_bin.reset(new unsigned char[this->hash_bin_len]);

	this->hex_len = this->hash_bin_len * 2 + 1; // According to https://doc.libsodium.org/helpers#hexadecimal-encoding-decoding
	this->hex.reset(new char[this->hex_len]);
}

SHA256::~SHA256()
{
	// All dynamic arrays is declared with smart pointers -> there's no need to manually delete them
}

std::string SHA256::hash() const
{
	crypto_hash_sha256(this->hash_bin.get(), this->data.get(), this->data_len);
	sodium_bin2hex(this->hex.get(), this->hex_len, this->hash_bin.get(), this->hash_bin_len);
	return std::string(this->hex.get());
};