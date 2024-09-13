#include "AEGIS256.h"

AEGIS256_Encrypt::AEGIS256_Encrypt(const std::string& key, const std::string& message)
{
	this->message_len = message.size();
	this->cipher_bin_len = 0;
	this->cipher_hex_len = 0;
	
	this->key = string2char<unsigned char>(key);
	this->message = string2char<unsigned char>(message);
	this->cipher_bin.reset(new unsigned char[this->message_len + crypto_aead_aegis256_ABYTES]);
	this->nonce_public.reset(new unsigned char[crypto_aead_aegis256_NPUBBYTES]);
	randombytes_buf(this->nonce_public.get(), crypto_aead_aegis256_NPUBBYTES);
}

AEGIS256_Encrypt::~AEGIS256_Encrypt()
{
}

std::string AEGIS256_Encrypt::encrypt()
{
	crypto_aead_aegis256_encrypt(this->cipher_bin.get(), &this->cipher_bin_len, this->message.get(), this->message_len, NULL, 0, NULL, this->nonce_public.get(), this->key.get());
	this->cipher_hex_len = this->cipher_bin_len * 2 + 1;
	this->cipher_hex.reset(new char[this->cipher_hex_len]);
	sodium_bin2hex(this->cipher_hex.get(), this->cipher_hex_len, this->cipher_bin.get(), this->cipher_bin_len);
	return std::string(this->cipher_hex.get());
}

std::unique_ptr<unsigned char[]> AEGIS256_Encrypt::getNoncePublic_Hex()
{
	std::unique_ptr<unsigned char[]> res (new unsigned char [crypto_aead_aegis256_NPUBBYTES]);
	memcpy(res.get(), this->nonce_public.get(), crypto_aead_aegis256_NPUBBYTES);
	return res;
}

AEGIS256_Decrypt::AEGIS256_Decrypt(const std::string& key, const std::string& cipher_hex, const unsigned char* nonce_public)
{
	this->cipher_hex_len = cipher_hex.size(); 
	this->cipher_bin_len = this->cipher_hex_len / 2;
	this->message_len = 0;
	
	this->key = string2char<unsigned char>(key);
	this->cipher_hex = string2char<char>(cipher_hex);

	this->nonce_public_bin.reset(new unsigned char [crypto_aead_aegis256_NPUBBYTES]);
	memcpy(this->nonce_public_bin.get(), nonce_public, crypto_aead_aegis256_NPUBBYTES);

	this->message.reset(new unsigned char[this->cipher_bin_len - crypto_aead_aegis256_ABYTES]);
	this->cipher_bin.reset(new unsigned char[this->cipher_bin_len]);

	sodium_hex2bin(this->cipher_bin.get(), this->cipher_bin_len, this->cipher_hex.get(), this->cipher_hex_len, NULL, &this->cipher_bin_len, NULL);
}

AEGIS256_Decrypt::~AEGIS256_Decrypt()
{
}

std::string AEGIS256_Decrypt::decrypt()
{
	crypto_aead_aegis256_decrypt(this->message.get(), &this->message_len, NULL, this->cipher_bin.get(), this->cipher_bin_len, NULL, 0, this->nonce_public_bin.get(), this->key.get());
	return std::string((char*) this->message.get(), this->message_len);
}
