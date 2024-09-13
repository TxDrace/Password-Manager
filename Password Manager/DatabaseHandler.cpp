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

//std::string DatabaseHandler::get_environment_variable(std::string environmentVarKey)
//{
//	char* pBuffer = nullptr;
//	size_t size = 0;
//	auto key = environmentVarKey.c_str();
//	// Use the secure version of getenv, ie. _dupenv_s to fetch environment variable.
//	if (_dupenv_s(&pBuffer, &size, key) == 0 && pBuffer != nullptr)
//	{
//		std::string environmentVarValue(pBuffer);
//		free(pBuffer);
//		return environmentVarValue;
//	}
//	else
//	{
//		return "";
//	}
//}

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


//std::string DatabaseHandler::query_master_password_of_user(std::string username)
//{
//	try {
//		CollectionIndex idx_users = CollectionIndex::USERS;
//		mongocxx::collection collection = db[this->collection_names[idx_users].c_str()];
//
//		auto find_result = collection.find_one(make_document(kvp("user", username.c_str())));
//		if (find_result) {
//			nlohmann::json json = nlohmann::json::parse(to_json(find_result->view()));
//			return json["masterkey"].get<std::string>();
//		}
//	}
//	catch (mongocxx::exception& e)
//	{
//		throw std::runtime_error(e.what());
//	}
//	catch (std::exception& e)
//	{
//		throw std::runtime_error(e.what());
//	}
//	return "";
//}

//void DatabaseHandler::create_user(std::string username, std::string master_password)
//{
//	try
//	{
//		CollectionIndex idx_users = CollectionIndex::USERS;
//		mongocxx::collection collection = db[this->collection_names[idx_users].c_str()];
//		SHAHashing sha(master_password);
//		string hashed_master_password = sha.Hash_hex();
//
//		bsoncxx::document::value doc_value = make_document(
//			kvp("user", username.c_str()),
//			kvp("masterkey", hashed_master_password.c_str())
//		);
//
//		collection.insert_one(doc_value.view());
//	}
//	catch (mongocxx::exception& e)
//	{
//		throw std::runtime_error(e.what());
//	}
//	catch (std::exception& e)
//	{
//		throw std::runtime_error(e.what());
//	}
//}

//void DatabaseHandler::edit_master_password(std::string master_username, std::string new_master_password)
//{
//	try
//	{
//		CollectionIndex idx_users = CollectionIndex::USERS;
//		mongocxx::collection collection = db[this->collection_names[idx_users].c_str()];
//		SHA256 sha(new_master_password);
//		std::string hashed_master_password = sha.hash();
//
//		collection.update_one(
//			make_document(kvp("user", master_username.c_str())),
//			make_document(kvp("$set", make_document(kvp("masterkey", hashed_master_password.c_str())))
//			)
//		);
//	}
//	catch (mongocxx::exception& e)
//	{
//		throw std::runtime_error(e.what());
//	}
//	catch (std::exception& e)
//	{
//		throw std::runtime_error(e.what());
//	}
//}


void DatabaseHandler::set_collection_name(std::string collectionName)
{
	
}

std::vector<Account> DatabaseHandler::query_credentials_by_service(std::string service)
{
	std::vector<Account> accounts;
	try
	{
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		auto cursor = collection.find(make_document(kvp("service", service.c_str())));
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

std::vector<Account> DatabaseHandler::query_all_credentials()
{
	std::vector<Account> accounts;
	try
	{
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		auto cursor = collection.find({});
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

std::vector<Account> DatabaseHandler::query_credentials_by_service_username(std::string service, std::string username)
{
	std::vector<Account> accounts;
	try
	{
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		auto cursor = collection.find(make_document(kvp("service", service.c_str()), kvp("username", username.c_str())));
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


void DatabaseHandler::add_credential(std::string id, std::string service, std::string username, std::string encrypted_password, std::string nonce_public, std::string description)
{
	try 
	{
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		bsoncxx::document::value doc_value = make_document(
			kvp("id", id.c_str()),
			kvp("service", service.c_str()),
			kvp("username", username.c_str()),
			kvp("password", encrypted_password.c_str()),
			kvp("nonce", nonce_public.c_str()),
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

void DatabaseHandler::edit_credential(std::string id, std::string new_service, std::string new_username, std::string new_password_encrypted, std::string new_nonce_public, std::string new_description)
{
	try 
	{
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		auto doc = collection.find_one(make_document(kvp("id", id.c_str())));
		if (doc)
		{
			nlohmann::json json = nlohmann::json::parse(to_json(doc->view()));
			Account acc(json);
			new_service = new_service.empty() ? acc.get_service() : new_service;
			new_username = new_username.empty() ? acc.get_username() : new_username;
			size_t new_password_encrypted_size = new_password_encrypted.size();
			new_password_encrypted = new_password_encrypted_size == 2 * crypto_aead_aegis256_ABYTES ? acc.get_password() : new_password_encrypted; // The new encrypted password is always != 0 because of the out put encrypt fuction (some ad - additional data are added)
			new_nonce_public = new_password_encrypted_size == 2 * crypto_aead_aegis256_ABYTES ? acc.get_nonce_public() : new_nonce_public; // The nonce will alway change everytime a new encrypted password come. So, if the encrypted password is not change, check the same condition as above, then change the nonce or keep remain
			new_description = new_description.empty() ? acc.get_description() : new_description;
		}
		
		collection.update_one(
			make_document(kvp("id", id.c_str())),
			make_document(kvp("$set", make_document(
				kvp("service", new_service.c_str()),
				kvp("username", new_username.c_str()),
				kvp("password", new_password_encrypted.c_str()),
				kvp("nonce", new_nonce_public.c_str()),
				kvp("description", new_description.c_str())
			)))
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

void DatabaseHandler::remove_credential(const std::string& id)
{
	try {
		CollectionIndex idx_credentials = CollectionIndex::CREDENTIALS;
		mongocxx::collection collection = db[this->collection_names[idx_credentials].c_str()];

		collection.delete_one(make_document(kvp("id", id.c_str())));
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