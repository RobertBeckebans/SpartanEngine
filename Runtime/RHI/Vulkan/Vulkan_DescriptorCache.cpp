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
#ifdef API_GRAPHICS_VULKAN
#include "../RHI_Implementation.h"
//================================

//= INCLUDES ======================
#include "../RHI_DescriptorCache.h"
#include "../RHI_Shader.h"
//=================================

//= NAMESPACES =====
using namespace std;
//==================

namespace Spartan
{
    RHI_DescriptorCache::~RHI_DescriptorCache()
    {
        if (m_descriptor_pool)
        {
            vkDestroyDescriptorPool(m_rhi_device->GetContextRhi()->device, static_cast<VkDescriptorPool>(m_descriptor_pool), nullptr);
            m_descriptor_pool = nullptr;
        }
    }

    void RHI_DescriptorCache::SetDescriptorSetCapacity(uint32_t descriptor_set_capacity)
    {
        if (!m_rhi_device || !m_rhi_device->GetContextRhi())
        {
            LOG_ERROR_INVALID_INTERNALS();
            return;
        }

        // Wait in case the pool is being used
        m_rhi_device->Queue_WaitAll();

        // Destroy layouts (and descriptor sets)
        m_descriptor_set_layouts.clear();
        m_descriptor_layout_current = nullptr;

        // Destroy pool
        if (m_descriptor_pool)
        {
            vkDestroyDescriptorPool(m_rhi_device->GetContextRhi()->device, static_cast<VkDescriptorPool>(m_descriptor_pool), nullptr);
            m_descriptor_pool = nullptr;
        }

        // Re-allocate everything with double size
        CreateDescriptorPool(descriptor_set_capacity);
    }

    bool RHI_DescriptorCache::CreateDescriptorPool(uint32_t descriptor_set_capacity)
    {
        // Pool sizes
        vector<VkDescriptorPoolSize> pool_sizes(4);
        pool_sizes[0].type              = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        pool_sizes[0].descriptorCount   = RHI_Context::descriptor_max_constant_buffers;
        pool_sizes[1].type              = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        pool_sizes[1].descriptorCount   = RHI_Context::descriptor_max_constant_buffers_dynamic;
        pool_sizes[2].type              = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        pool_sizes[2].descriptorCount   = RHI_Context::descriptor_max_textures;
        pool_sizes[3].type              = VK_DESCRIPTOR_TYPE_SAMPLER;
        pool_sizes[3].descriptorCount   = RHI_Context::descriptor_max_samplers;

        // Create info
        VkDescriptorPoolCreateInfo pool_create_info = {};
        pool_create_info.sType          = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_create_info.flags          = 0;
        pool_create_info.poolSizeCount  = static_cast<uint32_t>(pool_sizes.size());
        pool_create_info.pPoolSizes     = pool_sizes.data();
        pool_create_info.maxSets        = descriptor_set_capacity;

        // Pool
        const auto descriptor_pool = reinterpret_cast<VkDescriptorPool*>(&m_descriptor_pool);
        if (!vulkan_common::error::check(vkCreateDescriptorPool(m_rhi_device->GetContextRhi()->device, &pool_create_info, nullptr, descriptor_pool)))
            return false;

        return true;
    }
}
#endif
