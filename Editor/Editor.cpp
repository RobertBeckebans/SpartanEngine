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

//= INCLUDES =====================================
#include "Editor.h"
#include "Core/Engine.h"
#include "Core/Settings.h"
#include "Rendering/Model.h"
#include "ImGui_Extension.h"
#include "ImGui/Implementation/ImGui_RHI.h"
#include "ImGui/Implementation/imgui_impl_win32.h"
#include "Widgets/Widget_Assets.h"
#include "Widgets/Widget_Console.h"
#include "Widgets/Widget_MenuBar.h"
#include "Widgets/Widget_ProgressDialog.h"
#include "Widgets/Widget_Properties.h"
#include "Widgets/Widget_Toolbar.h"
#include "Widgets/Widget_Viewport.h"
#include "Widgets/Widget_World.h"
//================================================

//= NAMESPACES ==========
using namespace std;
using namespace Spartan;
//=======================
 
namespace _Editor
{
	Widget* widget_menu_bar		= nullptr;
	Widget* widget_toolbar		= nullptr;
	Widget* widget_world		= nullptr;
	const char* editor_name	    = "SpartanEditor";
}

Editor::~Editor()
{
	m_widgets.clear();
	m_widgets.shrink_to_fit();

	// Shutdown ImGui (unless the renderer was never initialized and ImGui was no initialized to begin with)
    if (m_renderer->IsInitialized())
    {
        ImGui::RHI::Shutdown();
	    ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
}

void Editor::OnWindowMessage(WindowData& window_data)
{
    // During window creation, Windows fire off a couple of messages,
    // m_initializing is to prevent that spamming.
    if (!m_engine && !m_initializing)
    {
        m_initializing = true;

        // Create engine
        m_engine = make_unique<Engine>(window_data);

        // Acquire useful engine subsystems
        m_context       = m_engine->GetContext();
        m_renderer      = m_context->GetSubsystem<Renderer>();
        m_profiler      = m_context->GetSubsystem<Profiler>();
        m_rhi_device    = m_renderer->GetRhiDevice();
        
        if (m_renderer->IsInitialized())
        {
            // ImGui version validation
            IMGUI_CHECKVERSION();
            m_context->GetSubsystem<Settings>()->RegisterThirdPartyLib("Dear ImGui", IMGUI_VERSION, "https://github.com/ocornut/imgui");

            // ImGui context creation
            ImGui::CreateContext();

            // ImGui configuration
            auto& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.ConfigWindowsResizeFromEdges = true;
            io.ConfigViewportsNoTaskBarIcon = true;
            ApplyStyle();

            // ImGui backend setup
            ImGui_ImplWin32_Init(window_data.handle);
            ImGui::RHI::Initialize(m_context, static_cast<float>(window_data.width), static_cast<float>(window_data.height));

            // Initialization of misc custom systems
            IconProvider::Get().Initialize(m_context);
            EditorHelper::Get().Initialize(m_context);

            // Create all ImGui widgets
            Widgets_Create();
        }
        else
        {
            LOG_ERROR("The engine failed to initialize the renderer subsystem, aborting editor creation.");
        }

        m_initializing = false;
    }
    else if (!m_initializing)
    {
        ImGui_ImplWin32_WndProcHandler(
            static_cast<HWND>(window_data.handle),
            static_cast<uint32_t>(window_data.message),
            static_cast<int64_t>(window_data.wparam),
            static_cast<uint64_t>(window_data.lparam)
        );

        if (m_engine->GetWindowData().width != window_data.width || m_engine->GetWindowData().height != window_data.height)
        {
            ImGui::RHI::OnResize(window_data.width, window_data.height);
        }

        m_engine->SetWindowData(window_data);
    }
}

void Editor::OnTick()
{	
	if (!m_engine)
		return;

	// Engine
	m_engine->Tick();

    // Editor
    m_profiler->TimeBlockStart("Editor", TimeBlock_Cpu);
    {
        // Ensure that rendering can take place
        if (!m_renderer || !m_renderer->IsInitialized())
            return;

        // ImGui implementation - start frame
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Editor update
        Widgets_Tick();

        // ImGui implementation - end frame
        ImGui::Render();
        ImGui::RHI::RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }
    m_profiler->TimeBlockEnd();
}

void Editor::Widgets_Create()
{
    m_widgets.emplace_back(make_unique<Widget_Console>(m_context));
    m_widgets.emplace_back(make_unique<Widget_MenuBar>(m_context)); _Editor::widget_menu_bar = m_widgets.back().get();
    m_widgets.emplace_back(make_unique<Widget_Toolbar>(m_context)); _Editor::widget_toolbar = m_widgets.back().get();
    m_widgets.emplace_back(make_unique<Widget_Viewport>(m_context));	
	m_widgets.emplace_back(make_unique<Widget_Assets>(m_context));	
	m_widgets.emplace_back(make_unique<Widget_Properties>(m_context));
	m_widgets.emplace_back(make_unique<Widget_World>(m_context)); _Editor::widget_world = m_widgets.back().get();
    m_widgets.emplace_back(make_unique<Widget_ProgressDialog>(m_context));
}

void Editor::Widgets_Tick()
{
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        MainWindow_Begin();
    }

	for (auto& widget : m_widgets)
	{
		widget->Begin();
		widget->Tick();
		widget->End();
	}

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        MainWindow_End();
    }
}

void Editor::MainWindow_Begin()
{
	// Set window flags
	const auto window_flags =
		ImGuiWindowFlags_MenuBar                |
		ImGuiWindowFlags_NoDocking              |
		ImGuiWindowFlags_NoTitleBar             |
		ImGuiWindowFlags_NoCollapse             |
		ImGuiWindowFlags_NoResize               |
		ImGuiWindowFlags_NoMove                 |
		ImGuiWindowFlags_NoBringToFrontOnFocus  |
		ImGuiWindowFlags_NoNavFocus;

	// Set window position and size
	float offset_y  = 0;
    offset_y        += _Editor::widget_menu_bar ? _Editor::widget_menu_bar->GetHeight() : 0;
    offset_y        += _Editor::widget_toolbar  ? _Editor::widget_toolbar->GetHeight()  : 0;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + offset_y));
	ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, viewport->Size.y - offset_y));
	ImGui::SetNextWindowViewport(viewport->ID);

	// Set window style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::SetNextWindowBgAlpha(0.0f);

    // Begin window
    bool open = true;
    m_editor_begun = ImGui::Begin(_Editor::editor_name, &open, window_flags);
    ImGui::PopStyleVar(3);

    // Begin dock space
	if (m_editor_begun)
    {
	    // Dock space
	    const auto window_id = ImGui::GetID(_Editor::editor_name);
	    if (!ImGui::DockBuilderGetNode(window_id))
	    {
	    	// Reset current docking state
	    	ImGui::DockBuilderRemoveNode(window_id);
	    	ImGui::DockBuilderAddNode(window_id, ImGuiDockNodeFlags_None);
            ImGui::DockBuilderSetNodeSize(window_id, ImGui::GetMainViewport()->Size);

	    	// DockBuilderSplitNode(ImGuiID node_id, ImGuiDir split_dir, float size_ratio_for_node_at_dir, ImGuiID* out_id_dir, ImGuiID* out_id_other);
            ImGuiID dock_main_id		        = window_id;
            ImGuiID dock_right_id		        = ImGui::DockBuilderSplitNode(dock_main_id,		ImGuiDir_Right, 0.2f,   nullptr, &dock_main_id);
            const ImGuiID dock_right_down_id	= ImGui::DockBuilderSplitNode(dock_right_id,	ImGuiDir_Down,	0.6f,   nullptr, &dock_right_id);
            ImGuiID dock_down_id		        = ImGui::DockBuilderSplitNode(dock_main_id,		ImGuiDir_Down,	0.25f,  nullptr, &dock_main_id);
            const ImGuiID dock_down_right_id	= ImGui::DockBuilderSplitNode(dock_down_id,		ImGuiDir_Right, 0.6f,   nullptr, &dock_down_id);

	    	// Dock windows	
	    	ImGui::DockBuilderDockWindow("World",		dock_right_id);
	    	ImGui::DockBuilderDockWindow("Properties",	dock_right_down_id);
	    	ImGui::DockBuilderDockWindow("Console",		dock_down_id);
	    	ImGui::DockBuilderDockWindow("Assets",		dock_down_right_id);
	    	ImGui::DockBuilderDockWindow("Viewport",	dock_main_id);
	    	ImGui::DockBuilderFinish(dock_main_id);
	    }

        ImGui::DockSpace(window_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    }
}

void Editor::MainWindow_End()
{
    if (m_editor_begun)
    {
	    ImGui::End();
    }
}

void Editor::ApplyStyle() const
{
	// Color settings	
	const auto color_text			        = ImVec4(0.810f, 0.810f, 0.810f, 1.000f);
    const auto color_text_disabled			= ImVec4(color_text.x, color_text.y, color_text.z, 0.5f);
    const auto color_interactive			= ImVec4(0.229f, 0.337f, 0.501f, 1.000f);
    const auto color_interactive_hovered    = ImVec4(0.312f, 0.456f, 0.675f, 1.000f);
    const auto color_interactive_clicked    = ImVec4(0.412f, 0.556f, 0.775f, 1.000f);
	const auto color_background		        = ImVec4(50.0f  / 255.0f, 50.0f  / 255.0f, 50.0f  / 255.0f, 1.0f);
    const auto color_background_content     = ImVec4(35.0f  / 255.0f, 35.0f  / 255.0f, 35.0f  / 255.0f, 1.0f);
    const auto color_shadow                 = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

    // Use default dark style as a base
    ImGui::StyleColorsDark();

	// Colors
    ImVec4* colors                          = ImGui::GetStyle().Colors;
    colors[ImGuiCol_Text]			        = color_text;
    colors[ImGuiCol_TextDisabled]			= color_text_disabled;
    colors[ImGuiCol_WindowBg]			    = color_background;             // Background of normal windows
    colors[ImGuiCol_ChildBg]			    = color_background;             // Background of child windows
    colors[ImGuiCol_PopupBg]			    = color_background;             // Background of popups, menus, tooltips windows
    colors[ImGuiCol_Border]			        = color_interactive;
    colors[ImGuiCol_BorderShadow]			= color_shadow;
    colors[ImGuiCol_FrameBg]			    = color_background_content;     // Background of checkbox, radio button, plot, slider, text input
    colors[ImGuiCol_FrameBgHovered]			= color_interactive;
    colors[ImGuiCol_FrameBgActive]			= color_interactive_clicked;
    colors[ImGuiCol_TitleBg]			    = color_background_content;
    colors[ImGuiCol_TitleBgActive]			= color_interactive;
    colors[ImGuiCol_TitleBgCollapsed]		= color_background;
    colors[ImGuiCol_MenuBarBg]			    = color_background_content;
    colors[ImGuiCol_ScrollbarBg]			= color_background_content;
    colors[ImGuiCol_ScrollbarGrab]		    = color_interactive;
    colors[ImGuiCol_ScrollbarGrabHovered]	= color_interactive_hovered;
    colors[ImGuiCol_ScrollbarGrabActive]	= color_interactive_clicked;
    colors[ImGuiCol_CheckMark]			    = color_text;
    colors[ImGuiCol_SliderGrab]			    = color_interactive;
    colors[ImGuiCol_SliderGrabActive]		= color_interactive_clicked;
    colors[ImGuiCol_Button]			        = color_interactive;
    colors[ImGuiCol_ButtonHovered]		    = color_interactive_hovered;
    colors[ImGuiCol_ButtonActive]			= color_interactive_clicked;
    colors[ImGuiCol_Header]			        = color_interactive;            // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
    colors[ImGuiCol_HeaderHovered]			= color_interactive_hovered;
    colors[ImGuiCol_HeaderActive]			= color_interactive_clicked;
    colors[ImGuiCol_Separator]			    = color_interactive;
    colors[ImGuiCol_SeparatorHovered]		= color_interactive_hovered;
    colors[ImGuiCol_SeparatorActive]		= color_interactive_clicked;
    colors[ImGuiCol_ResizeGrip]			    = color_interactive;
    colors[ImGuiCol_ResizeGripHovered]		= color_interactive_hovered;
    colors[ImGuiCol_ResizeGripActive]		= color_interactive_clicked;
    colors[ImGuiCol_Tab]			        = color_interactive;
    colors[ImGuiCol_TabHovered]			    = color_interactive_hovered;
    colors[ImGuiCol_TabActive]			    = color_interactive_clicked;
    colors[ImGuiCol_TabUnfocused]			= color_interactive;
    colors[ImGuiCol_TabUnfocusedActive]		= color_interactive;            // Might be called active, but it's active only because it's it's the only tab available, the user didn't really activate it
    colors[ImGuiCol_DockingPreview]			= color_interactive_clicked;    // Preview overlay color when about to docking something
    colors[ImGuiCol_DockingEmptyBg]			= color_interactive;            // Background color for empty node (e.g. CentralNode with no window docked into it)
    colors[ImGuiCol_PlotLines]			    = color_interactive;
    colors[ImGuiCol_PlotLinesHovered]		= color_interactive_hovered;
    colors[ImGuiCol_PlotHistogram]			= color_interactive;
    colors[ImGuiCol_PlotHistogramHovered]	= color_interactive_hovered;
    colors[ImGuiCol_TextSelectedBg]			= color_background;
    colors[ImGuiCol_DragDropTarget]			= color_interactive_hovered;    // Color when hovering over target
    colors[ImGuiCol_NavHighlight]			= color_background;             // Gamepad/keyboard: current highlighted item
    colors[ImGuiCol_NavWindowingHighlight]  = color_background;             // Highlight window when using CTRL+TAB
    colors[ImGuiCol_NavWindowingDimBg]		= color_background;             // Darken/colorize entire screen behind the CTRL+TAB window list, when active
    colors[ImGuiCol_ModalWindowDimBg]		= color_background;             // Darken/colorize entire screen behind a modal window, when one is active

    // Spatial settings
    const auto font_size    = 24.0f;
    const auto font_scale   = 0.7f;
    const auto roundness    = 2.0f;

	// Spatial
    ImGuiStyle& style               = ImGui::GetStyle();
	style.WindowBorderSize	        = 1.0f;
	style.FrameBorderSize	        = 0.0f;
    style.ScrollbarSize             = 20.0f;
	style.FramePadding		        = ImVec2(5, 5);
	style.ItemSpacing		        = ImVec2(6, 5);
    style.WindowMenuButtonPosition  = ImGuiDir_Right;
	style.WindowRounding	        = roundness;
	style.FrameRounding		        = roundness;
	style.PopupRounding		        = roundness;
	style.GrabRounding		        = roundness;
    style.ScrollbarRounding         = roundness;
    style.Alpha                     = 1.0f;

    // Font
    auto& io = ImGui::GetIO();
    const string dir_fonts = m_context->GetSubsystem<ResourceCache>()->GetDataDirectory(Asset_Fonts) + "/";
	io.Fonts->AddFontFromFileTTF((dir_fonts + "CalibriBold.ttf").c_str(), font_size);
    io.FontGlobalScale = font_scale;
}
