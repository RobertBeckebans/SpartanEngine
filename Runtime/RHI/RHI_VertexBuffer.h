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

//= INCLUDES ==========
#include <vector>
#include "RHI_Object.h"
//=====================

namespace Spartan
{
	class RHI_VertexBuffer : public RHI_Object
	{
	public:
		RHI_VertexBuffer(const std::shared_ptr<RHI_Device>& rhi_device, const uint32_t stride = 0) 
		{
			m_rhi_device	= rhi_device;
			m_stride		= stride;
		}

		~RHI_VertexBuffer();

		template<typename T>
		bool Create(const std::vector<T>& vertices)
		{
			m_stride        = static_cast<uint32_t>(sizeof(T));
			m_vertex_count	= static_cast<uint32_t>(vertices.size());
            m_size_gpu      = static_cast<uint64_t>(m_stride * m_vertex_count);
			return _Create(static_cast<const void*>(vertices.data()));
		}

		template<typename T>
		bool Create(const T* vertices, const uint32_t vertex_count)
		{
			m_stride        = static_cast<uint32_t>(sizeof(T));
			m_vertex_count	= vertex_count;
            m_size_gpu      = static_cast<uint64_t>(m_stride * m_vertex_count);
			return _Create(static_cast<const void*>(vertices));
		}

		template<typename T>
		bool CreateDynamic(const uint32_t vertex_count)
		{
			m_stride        = static_cast<uint32_t>(sizeof(T));
			m_vertex_count  = vertex_count;
            m_size_gpu      = static_cast<uint64_t>(m_stride * m_vertex_count);
			return _Create(nullptr);
		}

		void* Map() const;
		bool Unmap() const;
        bool Flush() const;

		void* GetResource()         const { return m_buffer; }
        uint32_t GetStride()        const { return m_stride; }
        uint32_t GetVertexCount()   const { return m_vertex_count; }

	private:
		bool _Create(const void* vertices);

		uint32_t m_stride			= 0;
		uint32_t m_vertex_count		= 0;

		// API
		std::shared_ptr<RHI_Device> m_rhi_device;
		void* m_buffer			= nullptr;
		void* m_buffer_memory	= nullptr;
        bool m_mappable         = false;
	};
}
