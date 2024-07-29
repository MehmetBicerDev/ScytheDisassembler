#include "engine.h"

namespace engine
{
	std::vector<skeleton::c_function> dumped_functions;
	skeleton::c_function get_function_by_address(uint64_t address)
	{
		for (auto& fn : dumped_functions)
			if (fn.address == address) return fn;
		return skeleton::c_function(0, 0);
	}
}