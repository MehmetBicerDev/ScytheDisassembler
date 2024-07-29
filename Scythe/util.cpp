#include "util.h"
#include <fstream>
#include <sstream>
#include <iomanip>
std::vector<uint8_t> util::read_file(const std::string& file_path)
{
	std::ifstream file(file_path, std::ios::binary);
	return std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

std::string util::to_hex_string(const uint64_t& input)
{
	std::stringstream stream;
	stream << std::hex << std::setw(16) << std::setfill('0') << input;
	return stream.str();
}

std::string util::to_hex_string(const std::vector<uint8_t>& bytes)
{
	std::ostringstream oss;
	for (auto byte : bytes) {
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte) << " ";
	}
	return oss.str();
}