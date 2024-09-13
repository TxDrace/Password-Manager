// Password Manager.cpp : Defines the entry point for the application.
//

#include "PasswordManager.h"

PasswordManager::PasswordManager() : db(this->databaseName)
{
	try {
		this->master_username = get_environment_variable(this->MASTER_USER_ENV);
		this->master_password = get_environment_variable(this->MASTER_PASSWORD_ENV);
		/*cout << "Master username: " << this->master_username << endl;
		cout << "Master password: " << this->master_password << endl;*/
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

std::string PasswordManager::get_master_username() const
{
	return this->master_username;
}

void PasswordManager::set_master_password(const std::string& master_password)
{
	this->master_password = master_password;
}

void PasswordManager::set_master_user(const std::string& master_username)
{
	this->master_username = master_username;
}

std::string PasswordManager::encrypt_password(std::string password)
{
	AEGIS256_Encrypt aes(this->master_password, password);
	return aes.encrypt();
}

std::string PasswordManager::decrypt_password(std::string cipher_hex_str)
{
	unsigned char nonce_public[crypto_aead_aegis256_NPUBBYTES]; // @TODO: get the nonce_public from database
	AEGIS256_Decrypt aes(this->master_password, cipher_hex_str, nonce_public);
	return aes.decrypt();
}


bool PasswordManager::verify_user(std::string user)
{
	std::string master_password_from_db = db.query_master_password_of_user(user);
	SHAHashing sha(this->master_password);
	std::string hashed_master_password = sha.Hash_hex();
	/*cout << "Original master password: " << this->master_password << endl;
	cout << "Master password from db: " << master_password_from_db << endl;
	cout << "Master password in system: " << hashed_master_password << endl;*/
	if (master_password_from_db.compare(hashed_master_password) != 0)
	{
		//cout << "Master password is incorrect." << endl;
		return false;
	}
	//cout << "Master password is correct." << endl;
	return true;
}

void PasswordManager::create_user(std::string username)
{
	db.create_user(username, this->master_password);
}

std::vector<Account> PasswordManager::find_all_credentials()
{
	std::vector<Account> accounts = db.query_all_credentials(this->master_username);
	return accounts;
}

std::vector<Account> PasswordManager::find_credentials_of_service(std::string service)
{
	std::vector<Account> accounts = db.query_credentials(this->master_username, service);
	return accounts;
}

std::vector<Account> PasswordManager::find_one_credential_of_service_and_username(std::string service, std::string username)
{
	return db.query_credential(this->master_username, service, username);
}

void PasswordManager::add_credential(std::string service, std::string username, std::string password, std::string description)
{
	std::string encrypted_password = convert_byte_str_to_hex_str(encrypt_password(password));
	db.add_credential(this->master_username, service, username, encrypted_password, description);
}

void PasswordManager::edit_credential(std::string service, std::string username, std::string new_password)
{
	db.edit_credential(this->master_username, service, username, convert_byte_str_to_hex_str(encrypt_password(new_password)));
}

void PasswordManager::remove_credential(std::string service, std::string username)
{
	db.remove_credential(this->master_username, service, username);
}


std::string PasswordManager::get_environment_variable(std::string environmentVarKey)
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

void PasswordManager::change_master_password(std::string new_master_password)
{
	try
	{
		std::vector<Account> accounts = db.query_all_credentials(this->master_username);
		for (Account account : accounts)
		{
			AESDecryption aes_decrypt(this->master_password, convert_hex_str_to_byte_str(account.get_password()));
			std::string decrypted_password = aes_decrypt.decrypt();
			//cout << "Decrypted password: " << decrypted_password << endl;
			AESEncryption aes_encrypt(new_master_password, decrypted_password);
			std::string new_encrypted_password = convert_byte_str_to_hex_str(aes_encrypt.encrypt());
			db.edit_credential(this->master_username, account.get_service(), account.get_username(), new_encrypted_password);
		}
		db.edit_master_password(this->master_username, new_master_password);
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
