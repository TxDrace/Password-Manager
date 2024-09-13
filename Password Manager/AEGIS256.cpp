#include "AEGIS256.h"

AEGIS256_Encrypt::AEGIS256_Encrypt(const std::string& key, const std::string& message)
{
	this->message_len = message.size();
	this->cipher_bin_len = 0;
	this->cipher_hex_len = 0;
	this->nonce_public_len = crypto_aead_aegis256_NPUBBYTES;
	
	SHA256 sha_key(key);
	this->key = string2char<unsigned char>(sha_key.hash());
	this->message = string2char<unsigned char>(message);
	this->cipher_bin.reset(new unsigned char[this->message_len + crypto_aead_aegis256_ABYTES]);
	this->nonce_public.reset(new unsigned char[this->nonce_public_len]);
	randombytes_buf(this->nonce_public.get(), this->nonce_public_len);
}

AEGIS256_Encrypt::~AEGIS256_Encrypt()
{
}

std::string AEGIS256_Encrypt::encrypt()
{
	crypto_aead_aegis256_encrypt(this->cipher_bin.get(), &this->cipher_bin_len, this->message.get(), this->message_len, NULL, 0, NULL, this->nonce_public.get(), this->key.get());
	this->cipher_hex = bin2hex(this->cipher_bin.get(), this->cipher_bin_len, this->cipher_hex_len);
	return std::string(this->cipher_hex.get());
}

std::string AEGIS256_Encrypt::getNoncePublic_Hex()
{
	size_t npub_hex_len;
	std::unique_ptr<char[]> npub_hex = bin2hex(this->nonce_public.get(), this->nonce_public_len, npub_hex_len);
	return std::string(npub_hex.get());
}

AEGIS256_Decrypt::AEGIS256_Decrypt(const std::string& key, const std::string& cipher_hex, const std::string& nonce_public_hex)
{
	this->cipher_hex_len = cipher_hex.size(); 
	this->cipher_bin_len = this->cipher_hex_len / 2;
	this->nonce_public_hex_len = nonce_public_hex.size();
	this->nonce_public_bin_len = this->nonce_public_hex_len / 2;
	this->message_len = 0;
	
	SHA256 sha_key(key);
	this->key = string2char<unsigned char>(sha_key.hash());
	this->cipher_hex = string2char<char>(cipher_hex);
	this->nonce_public_hex = string2char<char>(nonce_public_hex);

	this->message.reset(new unsigned char[this->cipher_bin_len - crypto_aead_aegis256_ABYTES]);

	this->nonce_public_bin = hex2bin(this->nonce_public_hex.get(), this->cipher_hex_len, &this->cipher_bin_len);
	this->cipher_bin = hex2bin(this->cipher_hex.get(), this->cipher_hex_len, &this->cipher_bin_len);

}

AEGIS256_Decrypt::~AEGIS256_Decrypt()
{
}

std::string AEGIS256_Decrypt::decrypt()
{
	crypto_aead_aegis256_decrypt(this->message.get(), &this->message_len, NULL, this->cipher_bin.get(), this->cipher_bin_len, NULL, 0, this->nonce_public_bin.get(), this->key.get());
	return std::string((char*) this->message.get(), this->message_len);
}
