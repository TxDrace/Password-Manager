#include <iostream>
#include <string>
#include <iomanip>
#include "PasswordManager.h"
#include "lib/color.hpp"
#include "lib/tabulate.hpp"
#include <sodium.h>

std::string take_password_from_user();
void print_accounts(const std::vector<Account> &accounts);
bool login(PasswordManager &app);
bool login_with_system_variables(PasswordManager &app);
void transform_to_lower_case(std::string &str);
int get_account_index_from_user_input(const std::vector<Account> &accounts);
void print_find_result(const std::string &username, const std::string &password);
void get_new_account_info(std::string &service, std::string &username, std::string &password, std::string &description, const int &argc, char *argv[]);
void query_credentials(PasswordManager &app, int argc, char *argv[]);
void get_account_info_to_edit(PasswordManager &app, std::string &service, std::string &username, std::string &new_password, const int &argc, char *argv[]);
void get_account_info_to_remove(PasswordManager &app, std::string &service, std::string &username, const int &argc, char *argv[]);
void remove_spaces(std::string &str);
void ask_for_sure();

int main(int argc, char *argv[])
{
	if (sodium_init() == -1) {
		std::cerr << "Error initializing libsodium" << std::endl;
		return 1;
	}

	std::cout << "Okay!!";
	return 0;


	/*PasswordManager app;
	std::string tabs = "\t";
	std::string user, master_password;

	if (argc == 1)
	{
		std::cout << "Password Manager is a simple tool that allows you to store and retrieve passwords securely." << std::endl
			 << std::endl;
		std::cout << "Usage:" << std::endl
			 << "  pm [command]" << std::endl
			 << std::endl;
		std::cout << "Available commands: " << std::endl;
		std::cout << "  find  " << tabs << "Find a password" << std::endl;
		std::cout << "  add   " << tabs << "Add a password" << std::endl;
		std::cout << "  edit  " << tabs << "Edit a password" << std::endl;
		std::cout << "  remove" << tabs << "Remove a password" << std::endl;
		std::cout << "  reset " << tabs << "Reset the master password" << std::endl;
		std::cout << "  gen   " << tabs << "Generate a password" << std::endl
			 << std::endl;
		std::cout << "Commands usage:" << std::endl;
		std::cout << "  pm find [service] [username]" << std::endl;
		std::cout << "  pm add [service] [username]" << std::endl;
		std::cout << "  pm edit [service] [username]" << std::endl;
		std::cout << "  pm remove [service] [username]" << std::endl;
		std::cout << "  pm reset" << std::endl;
		std::cout << "  pm gen" << std::endl;
	}

	else
	{
		try
		{
			if (login_with_system_variables(app))
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
					std::string service, username, new_password;
					get_account_info_to_edit(app, service, username, new_password, argc, argv);
					transform_to_lower_case(service);
					ask_for_sure();
					app.edit_credential(service, username, new_password);
					std::cout << dye::green("[SUCCESS] Credential edited successfully.") << std::endl;
				}

				else if (command == "remove")
				{
					std::string service, username;
					get_account_info_to_remove(app, service, username, argc, argv);
					transform_to_lower_case(service);
					ask_for_sure();
					app.remove_credential(service, username);
					std::cout << dye::green("[SUCCESS] Credential removed successfully.") << std::endl;
				}

				else if (command == "reset")
				{
					std::cout << "-> New master password: ";
					std::string new_master_password;
					std::cin >> new_master_password;

					if (new_master_password.empty())
					{
						throw std::runtime_error("Master password cannot be empty.");
					}

					app.change_master_password(new_master_password);
				}

				else if (command == "gen")
				{
					std::cout << dye::green("[SUCCESS] Generated password: ") << dye::yellow(app.generate_random_password()) << std::endl;
				}

				else
				{
					throw std::runtime_error("Invalid command.");
				}
			}
		}
		catch (std::exception &e)
		{
			std::cout << dye::red("[ERROR] " + std::string(e.what()) + ".") << std::endl;
		}
	}
	return 0;*/
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
	getline(std::cin, ipt);

	// Otherwise next std::cout will print into the same line
	std::cout << std::endl;

	// Restore the mode
	SetConsoleMode(hStdInput, mode);
	return ipt;
}

void print_accounts(const std::vector<Account> &accounts)
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
		std::string index = std::to_std::string(i);
		std::string service = account.get_service();
		std::string username = account.get_username();
		std::string description = account.get_description();
		// std::cout << std::left << std::setfill('.') << std::setw(10) << index << std::left << std::setw(20) << service << std::setw(50) << username << description << std::endl;
		credentials.add_row({index, service, username, description});
		i++;
	}
	std::cout << credentials << std::endl;
}

bool login(PasswordManager &app)
{
	std::string master_username, master_password;
	do
	{
		std::cout << "-> Master username: ";
		std::cin >> master_username;
		std::cout << "-> Master password: ";
		std::cin.ignore();
		master_password.assign(take_password_from_user());
		if (!master_password.empty() || !master_username.empty())
		{
			app.set_master_user(master_username);
			app.set_master_password(master_password);
			return app.verify_user(master_username);
		}
		else
		{
			std::cout << "[FAIL] Master password and username cannot be empty." << std::endl;
			return false;
		}
	} while (master_password.empty());
}

bool login_with_system_variables(PasswordManager &app)
{
	std::string master_username = app.get_master_username();
	std::string master_password = app.get_master_password();
	if (!app.get_master_password().empty() || !master_username.empty())
	{
		app.set_master_user(master_username);
		app.set_master_password(master_password);
		std::cout << "Stay calm! I'm checking your master username & master password..." << std::endl;
		if (!app.verify_user(master_username))
		{
			throw std::runtime_error("Oops! Your master username or master password is incorrect.");
		}
		else
		{
			std::cout << dye::green("[SUCCESS] Okay, welcome back!") << std::endl;
			return true;
		}
	}
	else
	{
		// std::cout << "[FAIL] Master password and username cannot be empty." << std::endl;
		throw std::runtime_error("Master password and username cannot be empty.");
	}
	return false;
}

void transform_to_lower_case(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

int get_account_index_from_user_input(const std::vector<Account> &accounts)
{
	std::cout << "Choose an account: ";
	std::string choice_str;
	std::cin >> choice_str;

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
		password.assign(take_password_from_user());
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
		password.assign(take_password_from_user());
		std::cout << "Description: ";
		std::getline(std::cin, description);
	}
	else if (argc == 4) // user enter: pm add [service] [username]
	{
		service = argv[2];
		username = argv[3];
		std::cout << "Password: ";
		password.assign(take_password_from_user());
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
			print_accounts(accounts);
			int choice = get_account_index_from_user_input(accounts);
			Account account = accounts[choice];
			print_find_result(account.get_username(), app.decrypt_password(account.get_password()));
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
			print_accounts(accounts);
			int choice = get_account_index_from_user_input(accounts);
			Account account = accounts[choice];
			print_find_result(account.get_username(), app.decrypt_password(account.get_password()));
		}
	}
	else if (argc == 4) // user enter: pm find [service] [username]
	{
		std::string service = argv[2];
		std::string username = argv[3];
		transform_to_lower_case(service);
		std::vector<Account> accounts = app.find_one_credential_of_service_and_username(service, username);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service '" + service + "' with username '" + username + "'");
		}
		else if (accounts.size() > 1)
		{
			throw std::runtime_error("Multiple accounts found for service'" + service + "' with username '" + username + "'");
		}
		else if (accounts.size() == 1)
		{
			Account account = accounts[0];
			print_find_result(account.get_username(), app.decrypt_password(account.get_password()));
		}
	}
}

void get_account_info_to_edit(PasswordManager &app, std::string &service, std::string &username, std::string &new_password, const int &argc, char *argv[])
{
	if (argc == 2) // user enter: pm edit -> list all accounts and choose one to edit
	{
		std::vector<Account> accounts = app.find_all_credentials();
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found.");
		}
		else
		{
			std::cout << std::endl;
			print_accounts(accounts);
			int choice = get_account_index_from_user_input(accounts);
			std::cout << "New password: ";
			std::cin.ignore();
			new_password.assign(take_password_from_user());

			if (choice < 0 || choice >= accounts.size())
			{
				throw std::runtime_error("Invalid choice.");
			}

			service = accounts[choice].get_service();
			username = accounts[choice].get_username();
		}
	}
	else if (argc == 3) // user enter: pm edit [service]
	{
		service = argv[2];
		std::vector<Account> accounts = app.find_credentials_of_service(service);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service '" + service + "'");
		}

		else
		{
			std::cout << std::endl;
			print_accounts(accounts);
			int choice = get_account_index_from_user_input(accounts);
			std::cout << "New password: ";
			std::cin.ignore();
			new_password.assign(take_password_from_user());
			username = accounts[choice].get_username();
		}
	}
	else if (argc == 4) // user enter: pm edit [service] [username]
	{
		std::string service = argv[2];
		std::string username = argv[3];
		std::vector<Account> accounts = app.find_one_credential_of_service_and_username(service, username);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service '" + service + "' and username '" + username + "'");
		}
		else if (accounts.size() > 1)
		{
			throw std::runtime_error("Multiple accounts found for service '" + service + "' and username '" + username + "'");
		}
		else if (accounts.size() == 1)
		{
			std::cout << "New password: ";
			new_password.assign(take_password_from_user());
		}
	}
}

void get_account_info_to_remove(PasswordManager &app, std::string &service, std::string &username, const int &argc, char *argv[])
{
	if (argc == 2) // user enter: pm remove
	{
		std::vector<Account> accounts = app.find_all_credentials();
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found.");
		}
		else
		{
			print_accounts(accounts);
			int choice = get_account_index_from_user_input(accounts);
			service = accounts[choice].get_service();
			username = accounts[choice].get_username();
		}
	}
	else if (argc == 3) // user enter: pm remove [service]
	{
		service = argv[2];
		std::vector<Account> accounts = app.find_credentials_of_service(service);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service'" + service + "'");
		}
		else
		{
			print_accounts(accounts);
			int choice = get_account_index_from_user_input(accounts);
			username = accounts[choice].get_username();
		}
	}
	else if (argc == 4) // user enter: pm remove [service] [username]
	{
		service = argv[2];
		username = argv[3];
		std::vector<Account> accounts = app.find_one_credential_of_service_and_username(service, username);
		if (accounts.empty())
		{
			throw std::runtime_error("No accounts found for service '" + service + "' and username '" + username + "'");
		}
		else if (accounts.size() > 1)
		{
			throw std::runtime_error("Multiple accounts found for service '" + service + "' and username '" + username + "'");
		}
	}
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
