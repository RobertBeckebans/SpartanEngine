/*
Copyright(c) 2016-2018 Panos Karabelas

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

#pragma once

//= INCLUDES ====
#include <string>
#include <vector>
#include <memory>

//===============

enum Thumbnail_Type
{
	Icon_Custom,
	Icon_Component_Options,
	Icon_Component_AudioListener,
	Icon_Component_AudioSource,
	Icon_Component_Camera,
	Icon_Component_Collider,
	Icon_Component_Light,
	Icon_Component_Material,
	Icon_Component_MeshCollider,
	Icon_Component_MeshFilter,
	Icon_Component_MeshRenderer,
	Icon_Component_RigidBody,
	Icon_Component_Script,
	Icon_Component_Transform,
	Icon_Console_Info,
	Icon_Console_Warning,
	Icon_Console_Error,
	Icon_File_Default,
	Icon_Folder,
	Icon_File_Audio,
	Icon_File_Scene,
	Icon_File_Model,
	Icon_Button_Play
};

namespace Directus
{
	class Context;
	class Texture;
}

// An icon shader resource pointer by enum 
#define THUMBNAIL_PROVIDER_BY_ENUM(iconEnum) ThumbnailProvider::GetShaderResourceByEnum(iconEnum)
// An icon shader resource pointer by filePath
#define THUMBNAIL_PROVIDER_BY_FILEPATH(filepath) ThumbnailProvider::GetShaderResourceByFilePath(filepath)

// An image
#define THUMBNAIL_PROVIDER_IMAGE(icon_enum, size)	\
	ImGui::Image(									\
	THUMBNAIL_PROVIDER_BY_ENUM(icon_enum),			\
	ImVec2(size, size),								\
	ImVec2(0, 0),									\
	ImVec2(1, 1),									\
	ImColor(255, 255, 255, 255),					\
	ImColor(255, 255, 255, 0))						\

// An thumbnail button by enum
#define THUMBNAIL_PROVIDER_IMAGE_BUTTON_ENUM(icon_enum, size) ImGui::ImageButton(THUMBNAIL_PROVIDER_BY_ENUM(icon_enum), ImVec2(size, size))
// An thumbnail button by enum, with a specific ID
#define THUMBNAIL_PROVIDER_IMAGE_BUTTON_ENUM_ID(id, icon_enum, size) ThumbnailProvider::ImageButton_enum_id(id, icon_enum, size)
// An thumbnail button by filepath
#define THUMBNAIL_PROVIDER_IMAGE_BUTTON_FILEPATH(filepath, size) ImGui::ImageButton(THUMBNAIL_PROVIDER_BY_FILEPATH(filepath), ImVec2(size, size))

struct Thumbnail
{
	Thumbnail(Thumbnail_Type type, std::shared_ptr<Directus::Texture> texture, const std::string& filePath)
	{
		this->type = type;
		this->texture = texture;
		this->filePath = filePath;
	}
	Thumbnail_Type type;
	std::shared_ptr<Directus::Texture> texture;
	std::string filePath;
};

class ThumbnailProvider
{
public:
	static void Initialize(Directus::Context* context);

	//= SHADER RESOURCE ==================================================
	static void* GetShaderResourceByEnum(Thumbnail_Type iconEnum);
	static void* GetShaderResourceByFilePath(const std::string& filePath);
	//====================================================================

	//= ImGui::ImageButton =================================================================
	static bool ImageButton_enum_id(const char* id, Thumbnail_Type iconEnum, float size);
	static bool ImageButton_filepath(const std::string& filepath, float size);
	//======================================================================================

private:
	static void Thumbnail_Load(Thumbnail_Type iconEnum, const std::string& filePath);
	static bool Thumbnail_Exists(const std::string& filePath);
	static std::shared_ptr<Directus::Texture> LoadThumbnail(const std::string& filePath, Directus::Context* context);

	static std::vector<Thumbnail> m_thumbnails;
};