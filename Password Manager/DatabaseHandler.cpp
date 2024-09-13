#include "DatabaseHandler.h"

DatabaseHandler::DatabaseHandler(std::string databaseName):
	databaseName(databaseName),
	inst(mongocxx::instance{}),
	mongoURI(mongocxx::uri{ get_environment_variable(MONGODB_URI).c_str() })
	/*pool(mongocxx::pool{ mongoURI })*/
{
	// Set the version of the Stable API on the client
	mongocxx::options::client client_options;
	auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
	client_options.server_api_opts(api);

	// Setup the connection and get the collections.
	this->conn = mongocxx::client { this->mongoURI, client_options };
	this->db = this->conn[this->databaseName.c_str()];
}

DatabaseHandler::~DatabaseHandler()
{
}

std::string DatabaseHandler::get_environment_variable(std::string environmentVarKey)
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
		return "";
	}
}

std::string DatabaseHandler::to_json(bsoncxx::document::view view)
{
	bson_t bson;
	bson_init_static(&bson, view.data(), view.length());

	size_t size;
	auto result = bson_as_json(&bson, &size);

	if (!result)
		return "";

	const auto deleter = [](char* result) { bson_free(result); };
	const std::unique_ptr<char[], decltype(deleter)> cleanup(result, deleter);

	return { result, size };
}


std::string DatabaseHandler::query_master_password_of_user(std::string username)
{
	try {
		CollectionIndex idx_users = CollectionIndex::USERS;
		mongocxx::collection collection = db[this->collection_names[idx_users].c_str()];

		auto find_result = collection.find_one(make_document(kvp("user", username.c_str())));
		if (find_result) {
			nlohmann::json json = nlohmann::json::parse(to_json(find_result->view()));
			return json["masterkey"].get<std::string>();
		}
	}
	catch (mongocxx::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	return "";
}

void DatabaseHandler::create_user(std::string username, std::string master_password)
{
	try
	{
		CollectionIndex idx_users = CollectionIndex::USERS;
		mongocxx::collection collection = db[this->collection_names[idx_users].c_str()];
		SHAHashing sha(master_password);
		string hashed_master_password = sha.Hash_hex();

		bsoncxx::document::value doc_value = make_document(
			kvp("user", username.c_str()),
			kvp("masterkey", hashed_master_password.c_str())
		);

		collection.insert_one(doc_value.view());
	}
	catch (mongocxx::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
}

void DatabaseHandler::edit_master_password(std::string master_username, std::string new_master_password)
{
	try
	{
		CollectionIndex idx_users = CollectionIndex::USERS;
		mongocxx::collection collection = db[this->collection_names[idx_users].c_str()];
		SHAHashing sha(new_master_password);
		string hashed_master_password = sha.Hash_hex();

		collection.update_one(
			make_document(kvp("user", master_username.c_str())),
			make_document(kvp("$set", make_document(kvp("masterkey", hashed_master_password.c_str())))
			)
		);
	}
	catch (mongocxx::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
}


void DatabaseHandler::set_collection_name(std::string collectionName)
{
	
}

std::vector<Account> DatabaseHandler::query_credentials(std::string user, std::string service)
{
	std::vector<Account> accounts;
	try
	{
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		auto cursor = collection.find(make_document(kvp("user", user.c_str()), kvp("service", service.c_str())));
		if (cursor.begin() != cursor.end()) {
			for (auto doc : cursor) {
				nlohmann::json json = nlohmann::json::parse(to_json(doc));
				Account acc(json);
				accounts.push_back(acc);
			}
		}
	}
	catch (mongocxx::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	return accounts;
}

std::vector<Account> DatabaseHandler::query_all_credentials(std::string user)
{
	std::vector<Account> accounts;
	try
	{
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		auto cursor = collection.find(make_document(kvp("user", user.c_str())));
		if (cursor.begin() != cursor.end()) {
			for (auto doc : cursor) {
				nlohmann::json json = nlohmann::json::parse(to_json(doc));
				Account acc(json);
				accounts.push_back(acc);
			}
		}
	}
	catch (mongocxx::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	return accounts;
}

std::vector<Account> DatabaseHandler::query_credential(std::string user, std::string service, std::string username)
{
	std::vector<Account> accounts;
	try
	{
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		auto result = collection.find_one(make_document(kvp("user", user.c_str()), kvp("service", service.c_str()), kvp("username", username.c_str())));
		if (result) {
			nlohmann::json json = nlohmann::json::parse(to_json(result->view()));
			Account acc(json);
			accounts.push_back(acc);
		}
	}
	catch (mongocxx::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	return accounts;
}


void DatabaseHandler::add_credential(std::string user, std::string service, std::string username, std::string password, std::string description)
{
	try 
	{
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		bsoncxx::document::value doc_value = make_document(
			kvp("user", user.c_str()),
			kvp("service", service.c_str()),
			kvp("username", username.c_str()),
			kvp("password", password.c_str()),
			kvp("description", description.c_str())
		);

		collection.insert_one(doc_value.view());
	} 
	catch (const mongocxx::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	catch (const std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
}

void DatabaseHandler::edit_credential(std::string user, std::string service, std::string username, std::string new_password)
{
	try 
	{
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		collection.update_one(
			make_document(kvp("user", user.c_str()), kvp("service", service.c_str()), kvp("username", username.c_str())),
			make_document(kvp("$set", make_document(kvp("password", new_password.c_str())))
			)
		);
	}
	catch (mongocxx::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
}

void DatabaseHandler::remove_credential(std::string user, std::string service, std::string username)
{
	try {
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		collection.delete_one(make_document(kvp("user", user.c_str()), kvp("service", service.c_str()), kvp("username", username.c_str())));
	}
	catch (mongocxx::exception& e)
	{
		throw std::runtime_error(e.what());
	}
	catch (std::exception& e)
	{
		throw std::runtime_error(e.what());
	}
}