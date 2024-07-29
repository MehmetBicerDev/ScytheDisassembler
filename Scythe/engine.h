#pragma once
#include "includes.h"
#include "c_function.h"

namespace engine
{
	extern std::vector<skeleton::c_function> dumped_functions;

	skeleton::c_function get_function_by_address(uint64_t address);
}