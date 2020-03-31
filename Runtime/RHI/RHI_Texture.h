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
#include "RHI_Object.h"
#include "RHI_Viewport.h"
#include "RHI_Definition.h"
#include "../Resource/IResource.h"
//================================

namespace Spartan
{
	enum RHI_Texture_Flags : uint16_t
	{
		RHI_Texture_ShaderView			        = 1 << 0,
        RHI_Texture_UnorderedAccessView         = 1 << 1,
		RHI_Texture_RenderTargetView	        = 1 << 2,
		RHI_Texture_DepthStencilView	        = 1 << 3,
        RHI_Texture_DepthStencilViewReadOnly    = 1 << 4,
        RHI_Texture_Grayscale                   = 1 << 5,
        RHI_Texture_Transparent                 = 1 << 6,
        RHI_Texture_GenerateMipsWhenLoading     = 1 << 7
	};

    enum RHI_Shader_View_Type : uint8_t
    {
        RHI_Shader_View_ColorDepth,
        RHI_Shader_View_Stencil,
        RHI_Shader_View_Unordered_Access
    };

	class SPARTAN_CLASS RHI_Texture : public RHI_Object, public IResource
	{
	public:
		RHI_Texture(Context* context);
		~RHI_Texture();

		//= IResource ===========================================
		bool SaveToFile(const std::string& file_path) override;
		bool LoadFromFile(const std::string& file_path) override;
		//=======================================================

		auto GetWidth() const											{ return m_width; }
		void SetWidth(const uint32_t width)								{ m_width = width; }

		auto GetHeight() const											{ return m_height; }
		void SetHeight(const uint32_t height)							{ m_height = height; }

		auto GetGrayscale() const										{ return m_flags & RHI_Texture_Grayscale; }
		void SetGrayscale(const bool is_grayscale)						{ is_grayscale ? m_flags |= RHI_Texture_Grayscale : m_flags &= ~RHI_Texture_Grayscale; }

		auto GetTransparency() const									{ return m_flags & RHI_Texture_Transparent; }
		void SetTransparency(const bool is_transparent)					{ is_transparent ? m_flags |= RHI_Texture_Transparent : m_flags &= ~RHI_Texture_Transparent; }

		auto GetBpp() const												{ return m_bpp; }
		void SetBpp(const uint32_t bpp)									{ m_bpp = bpp; }

		auto GetBpc() const												{ return m_bpc; }
		void SetBpc(const uint32_t bpc)									{ m_bpc = bpc; }

		auto GetChannels() const										{ return m_channels; }
		void SetChannels(const uint32_t channels)						{ m_channels = channels; }

		auto GetFormat() const											{ return m_format; }
		void SetFormat(const RHI_Format format)							{ m_format = format; }

		// Data
		const auto& GetData() const										{ return m_data; }		
        void SetData(const std::vector<std::vector<std::byte>>& data)   { m_data = data; }
        auto AddMipmap()                                                { return &m_data.emplace_back(std::vector<std::byte>()); }
        bool HasMipmaps() const                                         { return !m_data.empty();  }
        uint32_t GetMiplevels() const                                   { return m_mip_levels; }
        std::vector<std::byte>* GetData(uint32_t mipmap_index);
        std::vector<std::byte> GetMipmap(uint32_t index);

        // Binding type
        bool IsSampled()                    const { return m_flags & RHI_Texture_ShaderView; }
        bool IsRenderTargetCompute()        const { return m_flags & RHI_Texture_UnorderedAccessView; }
        bool IsRenderTargetDepthStencil()   const { return m_flags & RHI_Texture_DepthStencilView; }
        bool IsRenderTargetColor()          const { return m_flags & RHI_Texture_RenderTargetView; }

        // Format type
        bool IsDepthFormat()    const { return m_format == RHI_Format_D32_Float || m_format == RHI_Format_D32_Float_S8X24_Uint; }
        bool IsStencilFormat()  const { return m_format == RHI_Format_D32_Float_S8X24_Uint; }
        bool IsDepthStencil()   const { return IsDepthFormat() || IsStencilFormat(); }
        bool IsColorFormat()    const { return !IsDepthStencil(); }
        
        // Layout
        void SetLayout(const RHI_Image_Layout layout, RHI_CommandList* command_list = nullptr);
        RHI_Image_Layout GetLayout() const { return m_layout; }

        // Misc
        auto GetArraySize()         const { return m_array_size; }
        const auto& GetViewport()   const { return m_viewport; }

        // GPU resources
        auto Get_View_Texture(const uint32_t i = 0)                             const { return m_view_texture[i]; }
        auto Get_View_UnorderedAccess()	                                        const { return m_view_unordered_access; }
        auto Get_View_Attachment_DepthStencil(const uint32_t i = 0)             const { return i < m_view_attachment_depth_stencil.size() ? m_view_attachment_depth_stencil[i] : nullptr; }
        auto Get_View_Attachment_DepthStencil_ReadOnly(const uint32_t i = 0)    const { return i < m_view_attachment_depth_stencil_read_only.size() ? m_view_attachment_depth_stencil_read_only[i] : nullptr; }
        auto Get_View_Attachment_Color(const uint32_t i = 0)	                const { return i < m_view_attachment_color.size() ? m_view_attachment_color[i] : nullptr; }
        auto Get_Texture()                                                      const { return m_texture; }

	protected:
		bool LoadFromFile_NativeFormat(const std::string& file_path);
		bool LoadFromFile_ForeignFormat(const std::string& file_path, bool generate_mipmaps);
		static uint32_t GetChannelCountFromFormat(RHI_Format format);
        virtual bool CreateResourceGpu() { LOG_ERROR("Function not implemented by API"); return false; }

		uint32_t m_bpp			                = 0; // bits per pixel
		uint32_t m_bpc			                = 8; // bytes per channel
		uint32_t m_width		                = 0;
		uint32_t m_height		                = 0;
		uint32_t m_channels		                = 4;
        uint32_t m_array_size                   = 1;
        uint32_t m_mip_levels                   = 1;
		RHI_Format m_format		                = RHI_Format_Undefined;
        RHI_Image_Layout m_layout               = RHI_Image_Undefined;
        uint16_t m_flags	                    = 0;
		RHI_Viewport m_viewport;
		std::vector<std::vector<std::byte>> m_data;
		std::shared_ptr<RHI_Device> m_rhi_device;

        // API
        void* m_view_texture[2]         = { nullptr, nullptr }; // color/depth, stencil
        void* m_view_unordered_access   = nullptr;
        void* m_texture                 = nullptr;
        void* m_resource_memory         = nullptr;
        std::vector<void*> m_view_attachment_color;
        std::vector<void*> m_view_attachment_depth_stencil;
        std::vector<void*> m_view_attachment_depth_stencil_read_only;
	private:
		uint32_t GetByteCount();
	};
}
