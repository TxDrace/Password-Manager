#include <iostream>
#include <string>
#include <iomanip>
#include "PasswordManager.h"
#include "lib/color.hpp"
#include "lib/tabulate.hpp"
#include <sodium.h>

std::string take_password_from_user();
void query_credentials(PasswordManager& app, int argc, char* argv[]);

void print_credentials(const std::vector<Account> &accounts);
void print_find_result(const std::string& username, const std::string& password);
void print_all_services(const std::set<std::string>& service_list);

int get_account_index_from_user_input(const std::vector<Account> &accounts);
void get_new_account_info(std::string &service, std::string &username, std::string &password, std::string &description, const int &argc, char *argv[]);
void get_account_info_to_edit(PasswordManager &app, std::string& id, std::string &new_service, std::string & new_username, std::string &new_password, std::string &new_description, const int &argc, char *argv[]);
void get_account_info_to_remove(PasswordManager &app, std::string& id, const int &argc, char *argv[]);

void remove_spaces(std::string &str);
void ask_for_sure();
void transform_to_lower_case(std::string& str);



int main(int argc, char *argv[])
{
	try
	{
		if (sodium_init() == -1) {
			std::cerr << "Error initializing libsodium" << std::endl;
			return -1;
		}

		PasswordManager app;
		std::string tabs = "\t";
		std::string user, master_password;

		if (argc == 1)
		{
			std::cout << "Password Manager is a simple tool that allows you to store and retrieve passwords securely." << std::endl << std::endl;
			std::cout << "Usage:" << std::endl
				<< "  pm [command]" << std::endl<< std::endl;
			std::cout << "Available commands: " << std::endl;
			std::cout << "  find    " << tabs << "Find a password" << std::endl;
			std::cout << "  add     " << tabs << "Add a password" << std::endl;
			std::cout << "  edit    " << tabs << "Edit a password" << std::endl;
			std::cout << "  remove  " << tabs << "Remove a password" << std::endl;
			std::cout << "  reset   " << tabs << "Reset the master password" << std::endl;
			std::cout << "  gen     " << tabs << "Generate a password" << std::endl; 
			std::cout << "  list    " << tabs << "List all services" << std::endl;
			std::cout << "  import  " << tabs << "Import credentials from a json file" << std::endl << std::endl;
			std::cout << "Commands usage:" << std::endl;
			std::cout << "  pm find [service] [username]" << std::endl;
			std::cout << "  pm add [service] [username]" << std::endl;
			std::cout << "  pm edit [service] [username]" << std::endl;
			std::cout << "  pm remove [service] [username]" << std::endl;
			std::cout << "  pm reset" << std::endl;
			std::cout << "  pm gen" << std::endl;
			std::cout << "  pm list" << std::endl;
			std::cout << "  pm import" << std::endl;
		}

		else
		{
			std::string command = argv[1];
			if (command.find("find") != std::string::npos)
			{
				query_credentials(app, argc, argv);
			}

			else if (command == "add")
			{
				std::string service, username, password, description;
				get_new_account_info(service, username, password, description, argc, argv);
				transform_to_lower_case(service);
				app.add_credential(service, username, password, description);
				std::cout << dye::green("[SUCCESS] Credential added successfully.") << std::endl;
			}

			else if (command == "edit")
			{
				std::string id, new_service, new_username, new_password, new_description;
				get_account_info_to_edit(app, id, new_service, new_username, new_password, new_description, argc, argv);
				transform_to_lower_case(new_service);
				ask_for_sure();
				app.edit_credential(id, new_service, new_username, new_password, new_description);
				std::cout << dye::green("[SUCCESS] Credential edited successfully.") << std::endl;
			}

			else if (command == "remove")
			{
				std::string id;
				get_account_info_to_remove(app, id, argc, argv);
				ask_for_sure();
				app.remove_credential(id);
				std::cout << dye::green("[SUCCESS] Credential removed successfully.") << std::endl;
			}

			else if (command == "reset")
			{
				std::cout << "-> New master password: ";
				std::string new_master_password;
				std::getline(std::cin, new_master_password);

				if (new_master_password.empty())
				{
					throw std::runtime_error("Master password cannot be empty.");
				}

				app.change_master_password(new_master_password);
				std::cout << dye::green("[SUCCESS] All credentials have been update based on the new masterpassword.") << std::endl;
				std::cout << dye::red("[CAUTION] Update the environment variables now to avoid losing all your passwords") << std::endl;
			}

			else if (command == "gen")
			{
				std::cout << dye::green("[SUCCESS] Generated password: ") << dye::yellow(app.generate_random_password()) << std::endl;
			}

			else if (command == "list")
			{
				print_all_services(app.get_unique_services_list());
				std::cout << dye::green("[SUCCESS] Import credentials successfully ") << std::endl;
			}

			else if (command == "import")
			{
				std::cout << "Location of the .json file: ";
				std::string location;
				std::getline(std::cin, location);
				app.import_credential_data(location);
				std::cout << dye::green("[SUCCESS] Import credentials successfully ") << std::endl;
			}

			else
			{
				throw std::runtime_error("Invalid command.");
			}
		}

		return 0;
	}
	catch (std::exception& e)
	{
		std::cout << dye::red("[ERROR] " + std::string(e.what()) + ".") << std::endl;
	}
}

std::string take_password_from_user()
{
	HANDLE hStdInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;

	// Create a restore point Mode, is know 503
	GetConsoleMode(hStdInput, &mode);

	// Enable echo input, set to 499
	SetConsoleMode(hStdInput, mode & (~ENABLE_ECHO_INPUT));

	// Take input
	std::string ipt;
	std::getline(std::cin, ipt);

	// Restore the mode
	SetConsoleMode(hStdInput, mode);
	return ipt;
}

void print_credentials(const std::vector<Account> &accounts)
{
	tabulate::Table credentials;
	credentials.add_row({"Index", "Service", "Username", "Description"});
	for (size_t i = 0; i < 4; ++i)
	{
		credentials[0][i].format().font_color(tabulate::Color::yellow).font_style({tabulate::FontStyle::bold});
	}

	size_t i = 0;
	for (Account account : accounts)
	{
		std::string index = std::to_string(i);
		std::string service = account.get_service();
		std::string username = account.get_username();
		std::string description = account.get_description();
		credentials.add_row({index, service, username, description});
		i++;
	}
	std::cout << credentials << std::endl;
}

void transform_to_lower_case(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

int get_account_index_from_user_input(const std::vector<Account> &accounts)
{
	std::cout << "Choose an account: ";
	std::string choice_str;
	std::getline(std::cin, choice_str);

	// Check if the input is a number only
	if (!std::all_of(choice_str.begin(), choice_str.end(), ::isdigit))
	{
		throw std::runtime_error("Invalid input. Please enter a number.");
	}

	// Convert the std::string to an integer
	int choice = std::stoi(choice_str);

	if (choice < 0 || choice >= accounts.size())
	{
		throw std::runtime_error("Invalid choice.");
	}
	else
	{
		return choice;
	}
}

void print_find_result(const std::string &username, const std::string &password)
{
	tabulate::Table result;
	result.add_row({"Username", username});
	result.add_row({"Password", password});
	result.column(0).format().font_color(tabulate::Color::yellow).font_style({tabulate::FontStyle::bold});
	result.column(1).format().font_color(tabulate::Color::green).font_style({tabulate::FontStyle::bold});
	std::cout << result << std::endl;
}

void print_all_services(const std::set<std::string>& service_list)
{
	tabulate::Table services_table;
	services_table.add_row({ "Index", "Service" });
	for (size_t i = 0; i < 2; ++i)
	{
		services_table[0][i].format().font_color(tabulate::Color::yellow);
	}

	size_t i = 0;
	for (auto& item : service_list)
	{
		services_table.add_row({ std::to_string(i), item});
		i++;
	}
	std::cout << services_table << std::endl;
}

void get_new_account_info(std::string &service, std::string &username, std::string &password, std::string &description, const int &argc, char *argv[])
{
	if (argc == 2) // user enter: pm add
	{
		std::cout << "Service: ";
		std::getline(std::cin, service);
		remove_spaces(service);
		std::cout << "Username: ";
		std::getline(std::cin, username);
		remove_spaces(username);
		std::cout << "Password: ";
		//password.assign(take_password_from_user());
		std::getline(std::cin, password);
		std::cout << "Description: ";
		std::getline(std::cin, description);
	}
	else if (argc == 3) // user enter: pm add [service]
	{
		service = argv[2];
		std::cout << "Username: ";
		std::getline(std::cin, username);
		remove_spaces(username);
		std::cout << "Password: ";
		//password.assign(take_password_from_user());
		std::getline(std::cin, password);
		std::cout << "Description: ";
		std::getline(std::cin, description);
	}
	else if (argc == 4) // user enter: pm add [service] [username]
	{
		service = argv[2];
		username = argv[3];
		std::cout << "Password: ";
		//password.assign(take_password_from_user());
		std::getline(std::cin, password);
		std::cout << "Description: ";
		std::getline(std::cin, description);
	}
}

void query_credentials(PasswordManager &app, int argc, char *argv[])
{
	if (argc == 2) // user enter: pm find
	{
		std::vector<Account> accounts = app.find_all_credentials();
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found.");
		}
		else
		{
			print_credentials(accounts);
			int choice = get_account_index_from_user_input(accounts);
			Account account = accounts[choice];
			print_find_result(account.get_username(), app.decrypt_password(account.get_password(), account.get_nonce_public()));
		}
	}
	else if (argc == 3) // user enter: pm find [service]
	{
		std::string service = argv[2];
		transform_to_lower_case(service);
		std::vector<Account> accounts = app.find_credentials_of_service(service);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service '" + service + "'");
		}
		else
		{
			print_credentials(accounts);
			int choice = get_account_index_from_user_input(accounts);
			Account account = accounts[choice];
			print_find_result(account.get_username(), app.decrypt_password(account.get_password(), account.get_nonce_public()));
		}
	}
	else if (argc == 4) // user enter: pm find [service] [username]
	{
		std::string service = argv[2];
		std::string username = argv[3];
		transform_to_lower_case(service);
		std::vector<Account> accounts = app.find_credentials_of_service_and_username(service, username);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service '" + service + "' with username '" + username + "'");
		}
		else 
		{
			print_credentials(accounts);
			int choice = get_account_index_from_user_input(accounts);
			Account account = accounts[choice];
			print_find_result(account.get_username(), app.decrypt_password(account.get_password(), account.get_nonce_public()));
		}
	}
}

void get_account_info_to_edit(PasswordManager& app, std::string& id, std::string& new_service, std::string& new_username, std::string& new_password, std::string& new_description, const int& argc, char* argv[])
{
	std::vector<Account> accounts;
	if (argc == 2) // user enter: pm edit -> list all accounts and choose one to edit
	{
		accounts = app.find_all_credentials();
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found.");
		}
	}
	else if (argc == 3) // user enter: pm edit [service]
	{
		std::string service = argv[2];
		accounts = app.find_credentials_of_service(service);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service '" + service + "'");
		}
	}
	else if (argc == 4) // user enter: pm edit [service] [username]
	{
		std::string service = argv[2];
		std::string username = argv[3];
		accounts = app.find_credentials_of_service_and_username(service, username);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service '" + service + "' and username '" + username + "'");
		}
	}
	else
	{
		throw std::runtime_error("Invalid number of arguments");
	}

	std::cout << std::endl;
	print_credentials(accounts);
	int choice = get_account_index_from_user_input(accounts);
	if (choice < 0 || choice >= accounts.size())
	{
		throw std::runtime_error("Invalid choice.");
	}

	std::cout << "New service (ENTER to ignore): ";
	std::getline(std::cin, new_service);
	std::cout << "New username (ENTER to ignore): ";
	std::getline(std::cin, new_username);
	std::cout << "New password (ENTER to ignore): ";
	//new_password.assign(take_password_from_user());
	std::getline(std::cin, new_password);
	std::cout << "New description (ENTER to ignore): ";
	std::getline(std::cin, new_description);

	id = accounts[choice].get_id();
}

void get_account_info_to_remove(PasswordManager& app, std::string& id, const int& argc, char* argv[])
{
	std::vector<Account> accounts;
	if (argc == 2) // user enter: pm remove
	{
		accounts = app.find_all_credentials();
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found.");
		}
	}
	else if (argc == 3) // user enter: pm remove [service]
	{
		std::string service = argv[2];
		accounts = app.find_credentials_of_service(service);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service'" + service + "'");
		}
	}
	else if (argc == 4) // user enter: pm remove [service] [username]
	{
		std::string service = argv[2];
		std::string username = argv[3];
		accounts = app.find_credentials_of_service_and_username(service, username);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service '" + service + "' and username '" + username + "'");
		}
	}
	else
	{
		throw std::runtime_error("Invalid number of arguments");
	}

	print_credentials(accounts);
	int choice = get_account_index_from_user_input(accounts);
	id = accounts[choice].get_id();
}

void remove_spaces(std::string &str)
{
	str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), str.end());

	std::replace(str.begin(), str.end(), ' ', '-');
}

void ask_for_sure()
{
	std::cout << "Are you sure you want to proceed ? [y/n]: ";
	std::string choice;
	std::cin >> choice;
	if (!(choice == "y" || choice == "Y"))
	{
		throw std::runtime_error("Process has been cancelled by user.");
	}
}
