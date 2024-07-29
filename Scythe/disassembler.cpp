#include "disassembler.h"
#include <Zydis/Zydis.h>

namespace engine
{
	namespace disassembler
	{
		bool is_function_prologue(const ZydisDecodedInstruction& instruction, ZydisDecodedOperand* operands) {
			if (instruction.mnemonic == ZYDIS_MNEMONIC_PUSH &&
				operands[0].type == ZYDIS_OPERAND_TYPE_REGISTER &&
				operands[0].reg.value == ZYDIS_REGISTER_RBP) {
				return true;
			}
			return false;
		}bool is_trivial_instruction(const ZydisDecodedInstruction& instruction, ZydisDecodedOperand* operands) {
			return (instruction.mnemonic == ZYDIS_MNEMONIC_ADD &&
				operands[0].type == ZYDIS_OPERAND_TYPE_MEMORY &&
				operands[1].type == ZYDIS_OPERAND_TYPE_REGISTER &&
				operands[1].reg.value == ZYDIS_REGISTER_AL);
		}
		void disassemble_and_convert_functions(const std::vector<uint8_t>& data, std::vector<skeleton::c_function>& functions)
		{
			ZydisDecoder decoder;
			ZydisDecoderInit(&decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_STACK_WIDTH_64);

			ZydisFormatter formatter;
			ZydisFormatterInit(&formatter, ZYDIS_FORMATTER_STYLE_INTEL);

			ZydisDecodedInstruction instruction;
			ZydisDecodedOperand operands[ZYDIS_MAX_OPERAND_COUNT];

			size_t offset = 0;
			bool in_function = false;
			size_t function_start = 0;
			skeleton::c_function* current_function = nullptr;

			while (offset < data.size()) {
				ZyanStatus status = ZydisDecoderDecodeFull(&decoder, data.data() + offset, data.size() - offset, &instruction, operands);
				if (ZYAN_SUCCESS(status)) {
					if (is_function_prologue(instruction, operands)) {
						if (in_function) {
							bool is_trivial = true;
							for (const auto& instr : current_function->instructions) {
								ZydisDecodedInstruction instr_decoded;
								ZyanStatus instr_status = ZydisDecoderDecodeFull(&decoder, instr.bytes.data(), instr.size, &instr_decoded, operands);
								if (ZYAN_SUCCESS(instr_status) && !is_trivial_instruction(instr_decoded, operands)) {
									is_trivial = false;
									break;
								}
							}

							if (!is_trivial) {
								functions.push_back(*current_function);
							}

							delete current_function;
							current_function = nullptr;
							in_function = false;
						}
						function_start = offset;
						in_function = true;
						current_function = new skeleton::c_function(0, function_start);
					}

					if (in_function) {
						char buffer[256];
						ZydisFormatterFormatInstruction(&formatter, &instruction, operands, instruction.operand_count_visible, buffer, sizeof(buffer), offset, 0);
						current_function->instructions.emplace_back(instruction.length, data.data() + offset, buffer);
					}

					offset += instruction.length;
				}
				else {
					std::cerr << "Failed to decode instruction at offset " << std::hex << offset << std::endl;
					offset++;
				}
			}

			if (in_function) {
				bool is_trivial = true;
				for (const auto& instr : current_function->instructions) {
					ZydisDecodedInstruction instr_decoded;
					ZyanStatus instr_status = ZydisDecoderDecodeFull(&decoder, instr.bytes.data(), instr.size, &instr_decoded, operands);
					if (ZYAN_SUCCESS(instr_status) && !is_trivial_instruction(instr_decoded, operands)) {
						is_trivial = false;
						break;
					}
				}

				if (!is_trivial) {
					functions.push_back(*current_function);
				}

				delete current_function;
			}
		}
	}
}