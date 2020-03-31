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

#pragma once

//= INCLUDES =====================
#include <memory>
#include <vector>
#include "../RHI/RHI_Definition.h"
#include "../RHI/RHI_Shader.h"
//================================

namespace Spartan
{
	enum Variation_Flag : unsigned long
	{
		Variation_Albedo	= 1UL << 0,
		Variation_Roughness	= 1UL << 1,
		Variation_Metallic	= 1UL << 2,
		Variation_Normal	= 1UL << 3,
		Variation_Height	= 1UL << 4,
		Variation_Occlusion	= 1UL << 5,
		Variation_Emission	= 1UL << 6,
		Variation_Mask		= 1UL << 7
	};

	class ShaderVariation : public RHI_Shader, public std::enable_shared_from_this<ShaderVariation>
	{
	public:
		ShaderVariation(const std::shared_ptr<RHI_Device>& rhi_device, Context* context);
		~ShaderVariation() = default;

		void Compile(const std::string& file_path, unsigned long shader_flags);

		unsigned long GetShaderFlags() const	{ return m_flags; }
		bool HasAlbedoTexture() const			{ return m_flags & Variation_Albedo; }
		bool HasRoughnessTexture() const		{ return m_flags & Variation_Roughness; }
		bool HasMetallicTexture() const			{ return m_flags & Variation_Metallic; }
		bool HasNormalTexture() const			{ return m_flags & Variation_Normal; }
		bool HasHeightTexture() const			{ return m_flags & Variation_Height; }
		bool HasOcclusionTexture() const		{ return m_flags & Variation_Occlusion; }
		bool HasEmissionTexture() const			{ return m_flags & Variation_Emission; }
		bool HasMaskTexture() const				{ return m_flags & Variation_Mask; }

		// Variation cache
		static const std::shared_ptr<ShaderVariation>& GetMatchingShader(unsigned long flags);
        static const auto& GetVariations() { return m_variations; }

	private:
		void AddDefinesBasedOnMaterial();
		
		Context* m_context;
		unsigned long m_flags;	
		static std::vector<std::shared_ptr<ShaderVariation>> m_variations;
	};
}
