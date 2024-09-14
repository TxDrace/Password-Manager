#pragma once
#ifndef DATABASE_HANDLER_H
#define DATABASE_HANDLER_H
#include <string>
#include <iostream>
#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/exception/exception.hpp>
#include <bson/bson.h>
#include <mongocxx/uri.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/pool.hpp>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <iostream>
#include <vector>
#include <assert.h>
#include "SHA256.h"
#include "Account.h"
#include "Helper.h"

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_document;

#define MONGODB_URI "MONGODB_URI"

enum CollectionIndex { CREDENTIALS, USERS };

class DatabaseHandler
{
private:
	std::string databaseName;
	std::vector<std::string> collection_names = { "credentials", "masterkeys" };
	mongocxx::instance inst;
	mongocxx::uri mongoURI;
	mongocxx::client conn;
	mongocxx::database db;

public:
	DatabaseHandler(std::string databaseName);
	~DatabaseHandler();
	std::string to_json(bsoncxx::document::view view);
	std::vector<Account> query_credentials_by_service(std::string service);
	std::vector<Account> query_all_credentials();
	std::vector<Account> query_credentials_by_service_username(std::string service, std::string username);
	void add_credential(std::string id, std::string service, std::string username, std::string encrypted_password, std::string nonce_public, std::string description);
	void edit_credential(std::string id, std::string new_service, std::string new_username, std::string new_password_encryptedy, std::string new_nonce_public, std::string new_description);
	void remove_credential(const std::string& id);
};

#endif // DATABASE_HANDLER_H