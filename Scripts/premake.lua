-- Copyright(c) 2016-2019 Panos Karabelas

-- Permission is hereby granted, free of charge, to any person obtaining a copy
-- of this software and associated documentation files (the "Software"), to deal
-- in the Software without restriction, including without limitation the rights
-- to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
-- copies of the Software, and to permit persons to whom the Software is furnished
-- to do so, subject to the following conditions :

-- The above copyright notice and this permission notice shall be included in
-- all copies or substantial portions of the Software.

-- THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
-- IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
-- FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
-- COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
-- IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
-- CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


SOLUTION_NAME			= "Spartan"
EDITOR_NAME				= "Editor"
RUNTIME_NAME			= "Runtime"
TARGET_NAME				= "Spartan" -- Name of executable
DEBUG_FORMAT			= "c7"
EDITOR_DIR				    = "../" .. EDITOR_NAME
RUNTIME_DIR				= "../" .. RUNTIME_NAME
LIBRARY_DIR				= "../ThirdParty/libraries"
INTERMEDIATE_DIR		= "../Binaries/Intermediate"
TARGET_DIR_RELEASE  = "../Binaries/Release"
TARGET_DIR_DEBUG    = "../Binaries/Debug"
API_GRAPHICS			    = _ARGS[1]

-- Convert graphics api var to the corresponding project define
if API_GRAPHICS == "d3d11" then
	API_GRAPHICS	    = "API_GRAPHICS_D3D11"
	TARGET_NAME		= "Spartan_d3d11"
elseif API_GRAPHICS == "d3d12" then
	API_GRAPHICS	    = "API_GRAPHICS_D3D12"
	TARGET_NAME		= "Spartan_d3d12"
elseif API_GRAPHICS == "vulkan" then
	API_GRAPHICS	    = "API_GRAPHICS_VULKAN"
	TARGET_NAME		= "Spartan_vk"
end

-- Solution
solution (SOLUTION_NAME)
	location ".."
	systemversion "latest"
	cppdialect "C++17"
	language "C++"
	platforms "x64"
	configurations { "Release", "Debug" }
	
	-- Defines
	defines
	{
		"SPARTAN_RUNTIME_STATIC=1",
		"SPARTAN_RUNTIME_SHARED=0"
	}
	
	filter { "platforms:x64" }
		system "Windows"
		architecture "x64"
		
	--	"Debug"
	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "MultiProcessorCompile", "LinkTimeOptimization" }
		symbols "On"			
		
	--	"Release"	
	filter "configurations:Release"
		defines { "NDEBUG" }
		flags { "MultiProcessorCompile" }
		symbols "Off"	
		optimize "Full"

-- Runtime -------------------------------------------------------------------------------------------------
project (RUNTIME_NAME)
	location (RUNTIME_DIR)
	objdir (INTERMEDIATE_DIR)
	kind "StaticLib"
	staticruntime "On"
	defines{ "SPARTAN_RUNTIME", API_GRAPHICS }
	
	-- Files
	files 
	{ 
		RUNTIME_DIR .. "/**.h",
		RUNTIME_DIR .. "/**.cpp",
		RUNTIME_DIR .. "/**.hpp",
		RUNTIME_DIR .. "/**.inl"
	}

	-- Includes
	includedirs { "../ThirdParty/DirectXShaderCompiler" }
	includedirs { "../ThirdParty/SPIRV-Cross_2020-01-16" }
	includedirs { "../ThirdParty/Vulkan_1.2.131.1" }
	includedirs { "../ThirdParty/AngelScript_2.33.0" }
	includedirs { "../ThirdParty/Assimp_5.0.0" }
	includedirs { "../ThirdParty/Bullet_2.89" }
	includedirs { "../ThirdParty/FMOD_1.10.10" }
	includedirs { "../ThirdParty/FreeImage_3.18.0" }
	includedirs { "../ThirdParty/FreeType_2.10.0" }
	includedirs { "../ThirdParty/pugixml_1.10" }
	
	-- Libraries
	libdirs (LIBRARY_DIR)

	--	"Debug"
	filter "configurations:Debug"
		targetdir (TARGET_DIR_DEBUG)
		debugdir (TARGET_DIR_DEBUG)
		debugformat (DEBUG_FORMAT)
		links { "dxcompiler", "spirv-cross-core_debug", "spirv-cross-hlsl_debug", "spirv-cross-glsl_debug" }
		links { "angelscript_debug" }
		links { "assimp_debug" }
		links { "fmodL64_vc" }
		links { "FreeImageLib_debug" }
		links { "freetype_debug" }
		links { "BulletCollision_debug", "BulletDynamics_debug", "BulletSoftBody_debug", "LinearMath_debug" }
		links { "pugixml_debug" }
		links { "IrrXML_debug" }
			
	--	"Release"
	filter "configurations:Release"
		targetdir (TARGET_DIR_RELEASE)
		debugdir (TARGET_DIR_RELEASE)
		links { "dxcompiler", "spirv-cross-core", "spirv-cross-hlsl", "spirv-cross-glsl" }
		links { "angelscript" }
		links { "assimp" }
		links { "fmod64_vc" }
		links { "FreeImageLib" }
		links { "freetype" }
		links { "BulletCollision", "BulletDynamics", "BulletSoftBody", "LinearMath" }
		links { "pugixml" }
		links { "IrrXML" }

-- Editor --------------------------------------------------------------------------------------------------
project (EDITOR_NAME)
	location (EDITOR_DIR)
	links { RUNTIME_NAME }
	dependson { RUNTIME_NAME }
	targetname ( TARGET_NAME )
	objdir (INTERMEDIATE_DIR)
	kind "WindowedApp"
	staticruntime "On"
	defines{ "SPARTAN_EDITOR", API_GRAPHICS }
	
	-- Files
	files 
	{ 
		EDITOR_DIR .. "/**.rc",
		EDITOR_DIR .. "/**.h",
		EDITOR_DIR .. "/**.cpp",
		EDITOR_DIR .. "/**.hpp",
		EDITOR_DIR .. "/**.inl"
	}
	
	-- Includes
	includedirs { "../" .. RUNTIME_NAME }
	
	-- Libraries
	libdirs (LIBRARY_DIR)

	-- "Debug"
	filter "configurations:Debug"
		targetdir (TARGET_DIR_DEBUG)	
		debugdir (TARGET_DIR_DEBUG)
		debugformat (DEBUG_FORMAT)		
				
	-- "Release"
	filter "configurations:Release"
		targetdir (TARGET_DIR_RELEASE)
		debugdir (TARGET_DIR_RELEASE)