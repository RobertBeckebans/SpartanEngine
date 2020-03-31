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

//= IMPLEMENTATION ===============
#include "../RHI_Implementation.h"
#ifdef API_GRAPHICS_D3D11
//================================

//= INCLUDES =====================
#include "../RHI_ConstantBuffer.h"
#include "../RHI_Device.h"
#include "../../Logging/Log.h"
//================================

//= NAMESPACES =====
using namespace std;
//==================

namespace Spartan
{
	RHI_ConstantBuffer::~RHI_ConstantBuffer()
	{
		safe_release(*reinterpret_cast<ID3D11Buffer**>(&m_buffer));
	}

	void* RHI_ConstantBuffer::Map(const uint32_t offset_index /*= 0*/) const
    {
		if (!m_rhi_device || !m_rhi_device->GetContextRhi()->device_context || !m_buffer)
		{
			LOG_ERROR_INVALID_INTERNALS();
			return nullptr;
		}

		D3D11_MAPPED_SUBRESOURCE mapped_resource;
		const auto result = m_rhi_device->GetContextRhi()->device_context->Map(static_cast<ID3D11Buffer*>(m_buffer), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
		if (FAILED(result))
		{
			LOG_ERROR("Failed to map constant buffer.");
			return nullptr;
		}

		return mapped_resource.pData;
	}

	bool RHI_ConstantBuffer::Unmap() const
	{
		if (!m_rhi_device || !m_rhi_device->GetContextRhi()->device_context || !m_buffer)
		{
			LOG_ERROR_INVALID_INTERNALS();
			return false;
		}

		m_rhi_device->GetContextRhi()->device_context->Unmap(static_cast<ID3D11Buffer*>(m_buffer), 0);
		return true;
	}

	bool RHI_ConstantBuffer::_Create()
	{
		if (!m_rhi_device || !m_rhi_device->GetContextRhi()->device)
		{
			LOG_ERROR_INVALID_PARAMETER();
			return false;
		}

        safe_release(*reinterpret_cast<ID3D11Buffer**>(&m_buffer));

		D3D11_BUFFER_DESC buffer_desc;
		ZeroMemory(&buffer_desc, sizeof(buffer_desc));
		buffer_desc.ByteWidth			= static_cast<UINT>(m_stride);
		buffer_desc.Usage				= D3D11_USAGE_DYNAMIC;
		buffer_desc.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
		buffer_desc.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
		buffer_desc.MiscFlags			= 0;
		buffer_desc.StructureByteStride = 0;

        const auto result = m_rhi_device->GetContextRhi()->device->CreateBuffer(&buffer_desc, nullptr, reinterpret_cast<ID3D11Buffer**>(&m_buffer));
		if (FAILED(result))
		{
			LOG_ERROR("Failed to create constant buffer");
			return false;
		}

		return true;
	}

    bool RHI_ConstantBuffer::Flush(const uint32_t offset_index /*= 0*/)
    {
        return true;
    }
}
#endif
