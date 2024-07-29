#pragma once
#include "includes.h"

namespace engine
{
	namespace skeleton
	{
		class c_instruction
		{
		public:
			c_instruction(size_t sz, const uint8_t* byte_array, const std::string& dis_code)
				: size(sz), bytes(byte_array, byte_array + sz), disassembled_code(dis_code) {}

			size_t size;
			std::vector<uint8_t> bytes;
			std::string disassembled_code;
		};
	}
}