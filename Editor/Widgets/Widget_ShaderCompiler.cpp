/*
Copyright(c) 2016-2020 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES ============================
#include "Widget_ShaderEditor.h"
#include "Rendering/Renderer.h"
#include "Core/Context.h"
#include "Core/FileSystem.h"
#include "RHI/RHI_Shader.h"
#include <fstream>
#include <sstream>
#include "../ImGui/Source/imgui_stdlib.h"
//=======================================

//= NAMESPACES =========
using namespace std;
using namespace Spartan;
//======================

Widget_ShaderEditor::Widget_ShaderEditor(Context* context) : Widget(context)
{
    m_title         = "Shader Editor";
    m_flags         |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize;
    m_is_visible	= false;
    m_size          = ImVec2(1366, 1000);

    m_renderer = m_context->GetSubsystem<Renderer>();
}

void Widget_ShaderEditor::Tick()
{
    auto& shaders = m_renderer->GetShaders();

    // Left side - Shader list
    bool shader_dirty = false;

    ImGui::BeginGroup();
    {
        ImGui::Text("Shaders");

        if (ImGui::BeginChild("##shader_list", ImVec2(m_size.x * 0.32f, m_size.y), true))
        {
            for (const auto& shader_it : shaders)
            {
                // Build name
                string name = shader_it.second->GetName();
                for (const auto& define : shader_it.second->GetDefines())
                {
                    name += "[" + define.first + "]";
                }

                if (ImGui::Button(name.c_str()))
                {
                    m_shader = shader_it.second.get();
                    shader_dirty = true;
                    m_shader_name = name;
                }
            }
            ImGui::EndChild();
        }
    }
    ImGui::EndGroup();

    // Right side - Shader source
    ImGui::SameLine();  
    ImGui::BeginGroup();
    {
        ImGui::Text(m_shader ? m_shader_name.c_str() : "Select a shader");

        if (ImGui::BeginChild("##shader_source", ImVec2(m_size.x * 0.68f, m_size.y), true))
        {
            if (shader_dirty)
            {
                GetAllShadersFiles(m_shader->GetFilePath());
            }

            // Shader source
            if (ImGui::BeginTabBar("#shader_tab_bar", ImGuiTabBarFlags_Reorderable))
            {
                for (auto& shader : m_shader_files)
                {
                    if (ImGui::BeginTabItem(FileSystem::GetFileNameFromFilePath(shader.first).c_str()))
                    {
                        ImGui::InputTextMultiline("##shader_source", &shader.second, ImVec2(-1, ImGui::GetTextLineHeight() * 54.8f), ImGuiInputTextFlags_AllowTabInput);
                        ImGui::EndTabItem();
                    }
                }
                ImGui::EndTabBar();
            }
            
            if (ImGui::Button("Compile"))
            {
                // Save all files
                for (auto& shader : m_shader_files)
                {
                    ofstream out(shader.first);
                    out << shader.second;
                    out.flush();
                    out.close();
                }

                // Compile
                m_shader->Compile(m_shader->GetShaderStage(), m_shader->GetFilePath());
            }

            ImGui::EndChild();
        }
    }
    ImGui::EndGroup();
}

void Widget_ShaderEditor::GetAllShadersFiles(const string& file_path)
{
    vector<string> include_files = { file_path };
    auto new_includes = FileSystem::GetIncludedFiles(file_path);
    include_files.insert(include_files.end(), new_includes.begin(), new_includes.end());

    // Update shader files
    m_shader_files.clear();
    for (const auto& file : include_files)
    {
        ifstream in(file);
        stringstream buffer;
        buffer << in.rdbuf();
        m_shader_files[file] = buffer.str();
    }
}
