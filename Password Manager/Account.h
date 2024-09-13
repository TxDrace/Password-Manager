#pragma once
#ifndef ACCOUNT_H
#define ACCOUNT_H
#include <iostream>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class Account
{
private: 
	std::string id;
	std::string username;
	std::string password;
	std::string service;
	std::string description;
	std::string nonce_public_hex;

public:
	Account(std::string username, std::string password, std::string service, std::string description);
	Account(const nlohmann::json& json);
	~Account();
	std::string get_username() const;
	std::string get_password() const;
	std::string get_service() const;
	std::string get_description() const;
	std::string get_nonce_public() const;
	std::string get_id() const;
	void set_username(const std::string& username);
	void set_password(const std::string& password);
	void set_service(const std::string& service);
	void set_description(const std::string& description);
	void set_id(const std::string& id);
};


#endif // ACCOUNT_H
