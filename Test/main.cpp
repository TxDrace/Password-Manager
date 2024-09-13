//#pragma once
//#include <sodium.h>
//#include "SHA256.h"
//#include "AEGIS256.h"
//
//int main()
//{
//	if (sodium_init() == -1)
//	{
//		return -1;
//	}
//
//    //unsigned char nonce[crypto_aead_aegis256_NPUBBYTES];
//    //unsigned char key[crypto_aead_aegis256_KEYBYTES];
//    //unsigned char ciphertext[MESSAGE_LEN + crypto_aead_aegis256_ABYTES];
//    //unsigned long long ciphertext_len;
//    //crypto_aead_aegis256_keygen(key);
//    //randombytes_buf(nonce, sizeof nonce);
//    //crypto_aead_aegis256_encrypt(ciphertext, &ciphertext_len,
//    //    MESSAGE, MESSAGE_LEN,
//    //    NULL, 0,
//    //    NULL, nonce, key);
//    //size_t hex_len = ciphertext_len * 2 + 1;
//    //char* hex = new char[hex_len];
//    //sodium_bin2hex(hex, hex_len, ciphertext, ciphertext_len);
//    //std::cout << "Encrypted hex: " << hex << std::endl;
//    //size_t cipher_bin_len = hex_len / 2;
//    //unsigned char* cipher_bin = new unsigned char[cipher_bin_len];
//    //sodium_hex2bin(cipher_bin, cipher_bin_len, hex, hex_len, NULL, &cipher_bin_len, NULL);
//    //
//    //unsigned char decrypted[MESSAGE_LEN];
//    //unsigned long long decrypted_len;
//    //if (crypto_aead_aegis256_decrypt(decrypted, &decrypted_len,
//    //    NULL,
//    //    cipher_bin, cipher_bin_len,
//    //    NULL,
//    //    0,
//    //    nonce, key) != 0) {
//    //    /* message forged! */
//    //}
//    //std::cout << std::string((char*)decrypted) << std::endl;
//
//    /*SHA256 password("haituyen");
//
//    std::string message = "hjgkfhdsjfghjklghjdfsg#%^&@%^&#($*&HJJJJ";
//
//    AEGIS256_Encrypt password_encrypt(password.hash(), message);
//
//    std::string cipher = password_encrypt.encrypt();
//
//    std::unique_ptr<unsigned char[]> nonce_public = password_encrypt.getNoncePublic();
//
//    AEGIS256_Decrypt password_decrypt(password.hash(), cipher, nonce_public.get());
//
//    std::string plain = password_decrypt.decrypt();*/
//
//
//
//    
//    return 0;
//}

#include <iostream>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::v_noabi::to_json;
using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

int main() {
    // The mongocxx::instance constructor and destructor initialize and shut down the driver,
    // respectively. Therefore, a mongocxx::instance must be created before using the driver and
    // must remain alive for as long as the driver is in use.
    mongocxx::v_noabi::instance inst{};

    mongocxx::v_noabi::client conn{ mongocxx::v_noabi::uri {"mongodb://localhost/?replicaSet=replset"} };

    // By default, a session is causally consistent. Pass options::client_session to override
    // causal consistency.
    auto session = conn.start_session();
    auto coll = conn["db"]["collection"];
    auto result = coll.update_one(session,
        make_document(kvp("_id", 1)),
        make_document(kvp("$inc", make_document(kvp("x", 1)))));

    std::cout << "Updated " << result->modified_count() << " documents" << std::endl;

    // Read from secondary. In a causally consistent session the data is guaranteed to reflect the
    // update we did on the primary. The query may block waiting for the secondary to catch up,
    // or time out and fail after 2 seconds.
    options::find opts;
    read_preference secondary;
    secondary.mode(read_preference::read_mode::k_secondary);
    opts.read_preference(secondary).max_time(std::chrono::milliseconds(2000));
    auto cursor = coll.find(session, make_document(kvp("_id", 1)), opts);
    for (auto&& doc : cursor) {
        std::cout << bsoncxx::to_json(doc) << std::endl;
    }
}
