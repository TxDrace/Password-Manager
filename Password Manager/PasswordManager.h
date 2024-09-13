#pragma once
#ifndef PASSWORD_MANAGER_H
#define PASSWORD_MANAGER_H
#include <string>
#include <vector>
#include <iostream>
#include <random>
#include <algorithm>
#include "DatabaseHandler.h"
#include "Account.h"
#include "AEGIS256.h"
#include "Helper.h"

class PasswordManager
{
private:
	const std::string MASTER_PASSWORD_ENV = "PM_MASTER_PASSWORD";
	const std::string MASTER_USER_ENV = "PM_MASTER_USER";
	const int PASSWORD_GEN_LENGTH = 50;
	std::string databaseName = "password-manager";
	std::string master_username;
	std::string master_password;
	DatabaseHandler db;

public: 
	PasswordManager();
	~PasswordManager();
	std::string get_master_password() const;
	std::string get_master_username() const;
	void set_master_password(const std::string& master_password);
	void set_master_user(const std::string& master_user);
	void encrypt_password(std::string password, std::string& encrypted_password, std::string& nonce_public);
	std::string decrypt_password(const std::string& cipher_hex_str, const std::string& nonce_public_hex);
	// bool verify_user(std::string user);
	std::vector<Account> find_all_credentials();
	std::vector<Account> find_credentials_of_service(std::string service);
	std::vector<Account> find_credentials_of_service_and_username(std::string service, std::string username);
	void add_credential(std::string service, std::string username, std::string password, std::string description);
	void edit_credential(std::string service, std::string username, std::string new_password);
	void remove_credential(std::string service, std::string username);
	void create_user(std::string username);
	// std::string get_environment_variable(std::string environment_key);
	// void change_master_password(std::string new_master_password);
	std::string generate_random_password();
};

#endif // PASSWORD_MANAGER_H


