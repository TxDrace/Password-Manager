#include "Account.h"


//Account::Account(std::string username, std::string password, std::string service, std::string description)
//{
//	this->username = username;
//	this->password = password;
//	this->service = service;
//	this->description = description;
//}

Account::Account(const nlohmann::json& json)
{
	this->id = json["id"];
	this->username = json["username"];
	this->password = json["password"];
	this->service = json["service"];
	this->description = json["description"];
	this->nonce_public_hex = json["nonce"];
}

Account::~Account()
{
}

std::string Account::get_username() const
{
	return username;
}

std::string Account::get_password() const
{
	return password;
}

std::string Account::get_service() const
{
	return service;
}

std::string Account::get_description() const
{
	return description;
}

std::string Account::get_nonce_public() const
{
	return nonce_public_hex;
}

std::string Account::get_id() const
{
	return id;
}

void Account::set_username(const std::string& username)
{
	this->username = username;
}

void Account::set_password(const std::string& password)
{
	this->password = password;
}

void Account::set_service(const std::string& service)
{
	this->service = service;
}

void Account::set_description(const std::string& description)
{
	this->description = description;
}

void Account::set_id(const std::string& id)
{
	this->id = id;
}

// Path: AESEncryption.cpp