// Password Manager.cpp : Defines the entry point for the application.
//

#include "PasswordManager.h"

PasswordManager::PasswordManager() : db(this->databaseName)
{
	try {
		this->master_password = get_environment_variable(this->MASTER_PASSWORD_ENV);
	}
	catch (const std::exception& e) {
		std::cerr << "[ERROR] " << e.what() << std::endl;
		exit(1);
	}
}

PasswordManager::~PasswordManager()
{
}

std::string PasswordManager::get_master_password() const
{
	return master_password;
}


void PasswordManager::set_master_password(const std::string& master_password)
{
	this->master_password = master_password;
}


void PasswordManager::encrypt_password(std::string password, std::string& encrypted_password, std::string& nonce_public)
{
	AEGIS256_Encrypt aes(this->master_password, password);
	encrypted_password = aes.encrypt();
	nonce_public = aes.getNoncePublic_Hex();
}

std::string PasswordManager::decrypt_password(const std::string& cipher_hex_str, const std::string& nonce_public_hex)
{ 
	AEGIS256_Decrypt aes(this->master_password, cipher_hex_str, nonce_public_hex);
	return aes.decrypt();
}


void PasswordManager::create_user(std::string username)
{
	// db.create_user(username, this->master_password);
}

std::vector<Account> PasswordManager::find_all_credentials()
{
	return db.query_all_credentials();
}

std::vector<Account> PasswordManager::find_credentials_of_service(std::string service)
{
	return db.query_credentials_by_service(service);
}

std::vector<Account> PasswordManager::find_credentials_of_service_and_username(std::string service, std::string username)
{
	return db.query_credentials_by_service_username(service, username);
}

void PasswordManager::add_credential(std::string service, std::string username, std::string password, std::string description)
{
	std::string encrypted_password, nonce_public;
	encrypt_password(password, encrypted_password, nonce_public);
	db.add_credential(generateId(), service, username, encrypted_password, nonce_public, description);
}

void PasswordManager::edit_credential(std::string id, std::string new_service, std::string new_username, std::string new_password, std::string new_description)
{
	std::string encrypted_password, nonce_public;
	encrypt_password(new_password, encrypted_password, nonce_public);
	db.edit_credential(id, new_service, new_username, encrypted_password, nonce_public, new_description);
}

void PasswordManager::remove_credential(std::string id)
{
	db.remove_credential(id);
}


void PasswordManager::change_master_password(std::string new_master_password)
{
	try
	{
		std::vector<Account> accounts = db.query_all_credentials();
		for (Account account : accounts)
		{
			// Decrypt using the old master password
			std::string decrypted_password = this->decrypt_password(account.get_password(), account.get_nonce_public());
			
			// If decrypted password is empty. Stop it from removing values in database. 
			// When I run reset for the second time without updating the enviroment variable. 
			// The decryption function above will failed cause it use old password to decrypt new cipher. 
			// This will lead to the decrypted_password = "". If I don't stop at this step, the database will be updated with new password encrypted by an empty string
			if (decrypted_password.empty())
			{
				throw std::runtime_error("Password decryption failed. Did you update the environment variable after your last master password reset?");
			}

			// Encrypt using new master password
			AEGIS256_Encrypt aegis256_encrypt(new_master_password, decrypted_password);
			std::string new_encrypted_password = aegis256_encrypt.encrypt();
			std::string new_nonce_public = aegis256_encrypt.getNoncePublic_Hex();

			// Update database
			db.edit_credential(account.get_id(), "", "", new_encrypted_password, new_nonce_public, "");
		}
	}
	catch (const std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
}


std::string PasswordManager::generate_random_password()
{
	const int length = this->PASSWORD_GEN_LENGTH;
	// The list of characters to be used in the password
	const std::string lower_chars = "abcdefghijklmnopqrstuvwxyz";
	const std::string upper_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	const std::string digits = "1234567890";
	const std::string special_chars = "!@#$%^&*()~";

	std::random_device rd;
	std::mt19937 generator(rd());

	std::uniform_int_distribution<> lower_distribution(0, lower_chars.size() - 1);
	std::uniform_int_distribution<> upper_distribution(0, upper_chars.size() - 1);
	std::uniform_int_distribution<> digit_distribution(0, digits.size() - 1);
	std::uniform_int_distribution<> special_distribution(0, special_chars.size() - 1);

	std::string password;

	// Ensure the password has at least one of each type of character
	password += lower_chars[lower_distribution(generator)];
	password += upper_chars[upper_distribution(generator)];
	password += digits[digit_distribution(generator)];
	password += special_chars[special_distribution(generator)];

	// Fill the rest of the password with random characters from all types
	const std::string all_chars = lower_chars + upper_chars + digits + special_chars;
	std::uniform_int_distribution<> all_distribution(0, all_chars.size() - 1);

	for (int i = 4; i < length; ++i) {
		password += all_chars[all_distribution(generator)];
	}

	// Shuffle the characters in the password to ensure randomness
	std::shuffle(password.begin(), password.end(), generator);

	return password;
}

void PasswordManager::import_credential_data(std::string file_path)
{
	nlohmann::json data_json = read_json_file(file_path);
	for (const auto& item : data_json)
	{
		std::string service = item["service"];
		std::string username = item["username"];
		std::string password = item["password"];
		std::string description = item["description"];

		add_credential(service, username, password, description);
	}
}

std::set<std::string> PasswordManager::get_unique_services_list()
{
	std::set<std::string> unique_service_values;
	std::vector<Account> accounts = find_all_credentials();
	for (auto& acc : accounts)
	{
		unique_service_values.insert(acc.get_service());
	}
	return unique_service_values;
}
