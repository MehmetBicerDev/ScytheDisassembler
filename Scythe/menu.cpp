#include "menu.h"
#include "ImExtensions.h"
#include "ui.h"
#include "disassembler.h"
#include "util.h"
#include "engine.h"
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>

namespace menu
{
	bool show_menu = false;
	bool file_selector_show = false;
	uint64_t selected_function = 0;
	void draw_menu_cosmetics();
	void draw_head_context();
	void draw_sidebar();
	void draw_mainbar();

	std::string search_filter = "";
	std::string path = "";
	void update()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::SetNextWindowPos({ 0, 20 });
			ImGui::SetNextWindowSize(ImVec2{ (float)ui::size.x, (float)ui::size.y });
			ImGui::Begin(("Scythe Disassembler | V" + scythe::version).c_str(), &show_menu, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus);
			{
				ImGui::Text(("Scythe Disassembler | V" + scythe::version).c_str());
				ImGui::Separator();

				if (engine::dumped_functions.size()) {
					draw_sidebar();
					ImGui::SameLine(0, 0);
					draw_mainbar();
				}
				else {
					ImGui::Text("Please load a file.");
				}
				ImGui::End();
			}
			draw_head_context();
		}

		if (ImExtensions::FileSelectorManual("Dumped File", path, file_selector_show))
		{
			engine::disassembler::disassemble_and_convert_functions(util::read_file(path), engine::dumped_functions);
		}
		ImGui::Render();
	}

	void draw_head_context()
	{
		ImGui::SetNextWindowPos({ 0, 0 });
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Load File"))
				{
					file_selector_show = true;
				}

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	void draw_menu_cosmetics()
	{
		auto window_draw_list = ImGui::GetWindowDrawList();
		window_draw_list->AddRectFilled(ImVec2(0, 0), ImVec2(900, 19), 0xFFFFFFFF);
	}

	void draw_sidebar()
	{
		ImGui::SetCursorPosX(0);
		ImGui::BeginChild("##Sidebar", ImVec2(200, 480), ImGuiChildFlags_Border);

		ImExtensions::InputTextWithHint("Address Filter", "<address filter>", &search_filter, 0, 0, 0);
		for (auto fn : engine::dumped_functions)
		{
			if (!search_filter.empty() && util::to_hex_string(fn.address).find(search_filter) == std::string::npos) continue;
			ImGui::SetNextItemWidth(200.0f);
			ImGui::SetCursorPosX(0);
			if (ImGui::Button(("Base+0x" + util::to_hex_string(fn.address)).c_str()))
			{
				selected_function = fn.address;
			}
		}
		ImGui::EndChild();
	}

	void draw_mainbar()
	{
		ImGui::BeginChild("##Mainbar", ImVec2(694, 480), ImGuiChildFlags_Border);

		if (selected_function)
		{
			const auto fn = engine::get_function_by_address(selected_function);
			ImGui::SetCursorPos(ImVec2(0, 0));
			if (ImGui::BeginTable("##instruction_table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
				ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed, 100.0f);
				ImGui::TableSetupColumn("Bytes");
				ImGui::TableSetupColumn("Opcode");
				ImGui::TableHeadersRow();

				size_t byte_offset = 0;
				for (const auto& instruction : fn.instructions) {
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text("Base+0x%llx", byte_offset + fn.address);

					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%s", util::to_hex_string(instruction.bytes).c_str());

					ImGui::TableSetColumnIndex(2);
					ImExtensions::DrawAssemblyInstruction(instruction.disassembled_code);

					byte_offset += instruction.size;
				}

				ImGui::EndTable();
			}
		}
		ImGui::EndChild();
	}
}