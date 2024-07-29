#pragma once
#include "c_instruction.h"

namespace engine
{
	namespace skeleton
	{
		class c_function
		{
		public:
			c_function(size_t sz, uint64_t addr)
				: size(sz), address(addr) {}

			size_t size;
			uint64_t address;
			std::vector<c_instruction> instructions;
		};
	}
}