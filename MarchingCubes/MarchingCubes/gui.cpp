#include "gui.h"
#include "Renderer.h"
#include "MetaBallApplication.h"
void doMainTab(unsigned int vbo) {
	if (ImGui::Button("Restart")) {
		MetaBallApplication::SpawnRandomMovingMetaballs();
		if (MetaBallApplication::paused) {
			MetaBallApplication::March();
			MetaBallApplication::UpdateVertices(vbo);
		}
	}
	ImGui::Checkbox("Paused", &MetaBallApplication::paused);
	if (ImGui::SliderFloat("Iso Level", &MetaBallApplication::cubes_iso_level, 0.1f, 1.0f)) {
		if (MetaBallApplication::paused) {
			MetaBallApplication::March();
			MetaBallApplication::UpdateVertices(vbo);
		}
	}

	ImGui::InputInt("number of blobs", &MetaBallApplication::number_to_spawn);
}

void doRenderingTab(unsigned int vbo) {
	if (ImGui::Checkbox("Wire frame mode", &MetaBallApplication::wireframe_mode)) {
		if (MetaBallApplication::wireframe_mode) {
			Renderer::WireframeOn();
		}
		else {
			Renderer::WireframeOff();
		}
	}
	ImGui::DragFloat3("Light position", &Renderer::light_pos[0], 0.01f, -20.0f, 20.0f);
	ImGui::ColorPicker3("Object colour", &Renderer::object_colour[0]);
	ImGui::ColorPicker3("Light colour", &Renderer::light_colour[0]);
	ImGui::DragFloat("ambient strength", &Renderer::ambient_strength, 0.01f, 0.0, 2.0);
	ImGui::DragFloat("specular strength", &Renderer::specular_strength, 0.01f, 0.0, 10.0);
	static const char* current_item = "32";
	if (ImGui::BeginCombo("shininess", current_item)) {
		if (ImGui::Selectable("8")) {
			Renderer::shininess = 8.0f;
			current_item = "8";
		}
		if (ImGui::Selectable("16")) {
			Renderer::shininess = 16.0f;
			current_item = "16";
		}
		if (ImGui::Selectable("32")) {
			Renderer::shininess = 32.0f;
			current_item = "32";
		}
		if (ImGui::Selectable("64")) {
			Renderer::shininess = 64.0f;
			current_item = "64";
		}
		if (ImGui::Selectable("128")) {
			Renderer::shininess = 128.0f;
			current_item = "128";
		}
		if (ImGui::Selectable("256")) {
			Renderer::shininess = 256.0f;
			current_item = "256";
		}
		ImGui::EndCombo();
	}
}
std::string GuidToString(GUID guid)
{
	char guid_cstr[39];
	snprintf(guid_cstr, sizeof(guid_cstr),
		"{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	return std::string(guid_cstr);
}

void doMetaBallsWindow(unsigned int vbo) {
	ImGui::Begin("meta balls");
	for (MovingMetaball& m : MetaBallApplication::_MovingMetaballs) {

		if (ImGui::CollapsingHeader(GuidToString(m.id).c_str(), ImGuiTreeNodeFlags_None))
		{
			//ImGui::Text("IsItemHovered: %d", ImGui::IsItemHovered());
			MetaBall* ptr = MetaBallApplication::GetMetaBallPtr(m.id);
			if (ImGui::DragFloat3("position", &ptr->position.x, 0.05f)) {
				if (MetaBallApplication::paused) {
					MetaBallApplication::March();
					MetaBallApplication::UpdateVertices(vbo);
				}
			}
			if (ImGui::DragFloat("radius", &ptr->radius, 0.05f)) {
				if (MetaBallApplication::paused) {
					MetaBallApplication::March();
					MetaBallApplication::UpdateVertices(vbo);
				}
			}
			if (ImGui::BeginCombo("phase", m.phase_string.c_str())) {
				if (ImGui::Selectable("positive")) {
					ptr->phase = PHASE::POSITIVE;
					m.phase_string = "positive";
				}
				if (ImGui::Selectable("negative")) {
					ptr->phase = PHASE::NEGATIVE;
					m.phase_string = "negative";
				}
				ImGui::EndCombo();
			}
		}
	}
	ImGui::End();
}

void doUI(unsigned int vbo)
{
	ImGui::Begin("Demo window");
	ImGui::Text(MetaBallApplication::march_timer_text.c_str());
	if(ImGui::BeginTabBar("tabs")){
		if (ImGui::BeginTabItem("Main")) {
			doMainTab(vbo);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Rendering")) {
			doRenderingTab(vbo);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Marching cubes settings")) {
			int threads = MetaBallApplication::GetThreads();
			if (ImGui::InputInt("threads to use", &threads, 1)) {
				if (threads > 0 && threads < MetaBallApplication::GetHardWareThreads()) {
					MetaBallApplication::SetThreads(threads);
				}
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	ImGui::End();

	doMetaBallsWindow(vbo);
}
