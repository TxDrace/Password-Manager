#include "Helper.h"

std::string get_environment_variable(std::string environmentVarKey)
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

std::unique_ptr<unsigned char[]> hex2bin(const char* hex, size_t hex_len, size_t* bin_len) 
{
	size_t bin_max_len = hex_len / 2;
	std::unique_ptr<unsigned char[]> bin(new unsigned char[bin_max_len]);
	sodium_hex2bin(bin.get(), bin_max_len, hex, hex_len, NULL, bin_len, NULL);
	return bin;
}

std::unique_ptr<char[]> bin2hex(const unsigned char* bin, size_t bin_len, size_t& hex_len)
{
	hex_len = bin_len * 2 + 1; // According to https://doc.libsodium.org/helpers#hexadecimal-encoding-decoding
	std::unique_ptr<char[]> hex(new char[hex_len]);
	sodium_bin2hex(hex.get(), hex_len, bin, bin_len);
	return hex;
}

std::string generateId()
{
	const auto now = std::chrono::system_clock::now();
	const std::time_t t_c = std::chrono::system_clock::to_time_t(now);
	std::tm tm = *std::localtime(&t_c);

	std::ostringstream oss;
	oss << std::put_time(&tm, "%Y%m%d%H%M%S");
	return oss.str() + generate_random_10_digit_number();
}


std::string generate_random_10_digit_number() 
{
	// Define the range for a 10-digit number
	const int min = 1000000000;
	const int max = 9999999999;

	// Create a random device and a random number generator
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(min, max);

	// Generate the random number
	int random_number = dis(gen);

	// Convert the number to a string
	return std::to_string(random_number);
}

std::string read_file(const std::string& file_path) 
{
	std::ifstream file(file_path);
	if (!file.is_open()) {
		throw std::runtime_error("Unable to open file");
	}

	std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	return content;
}

nlohmann::json read_json_file(const std::string& file_path)
{
	std::string file_content = read_file(file_path);
	nlohmann::json json_obj = nlohmann::json::parse(file_content);
	return json_obj;
}