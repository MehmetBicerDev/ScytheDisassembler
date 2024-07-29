#pragma once
#include "includes.h"
#include "c_function.h"

namespace engine
{
	namespace disassembler
	{
		void disassemble_and_convert_functions(const std::vector<uint8_t>& data, std::vector<skeleton::c_function>& functions);
	}
}