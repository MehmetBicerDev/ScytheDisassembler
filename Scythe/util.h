#pragma once
#include "includes.h"

namespace util
{
	/*FILE*/
	std::vector<uint8_t> read_file(const std::string& file_path);

	/*STRING*/
	std::string to_hex_string(const uint64_t& input);
	std::string to_hex_string(const std::vector<uint8_t>& bytes);
}