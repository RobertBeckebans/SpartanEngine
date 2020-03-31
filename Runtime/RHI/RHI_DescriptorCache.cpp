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

//= INCLUDES =======================
#include "RHI_DescriptorCache.h"
#include "RHI_Shader.h"
#include "RHI_Sampler.h"
#include "RHI_Texture.h"
#include "RHI_PipelineState.h"
#include "RHI_ConstantBuffer.h"
#include "RHI_Implementation.h"
#include "RHI_DescriptorSetLayout.h"
#include "..\Utilities\Hash.h"
//==================================

//= NAMESPACES =====
using namespace std;
//==================

namespace Spartan
{
    RHI_DescriptorCache::RHI_DescriptorCache(const RHI_Device* rhi_device)
    {
        m_rhi_device = rhi_device;

        // Set the descriptor set capacity to an initial value
        SetDescriptorSetCapacity(m_descriptor_set_capacity);
    }

    void RHI_DescriptorCache::SetPipelineState(RHI_PipelineState& pipeline_state)
    {
        // Name this resource, very useful for Vulkan debugging
        m_name = (pipeline_state.shader_vertex ? pipeline_state.shader_vertex->GetName() : "null") + "-" + (pipeline_state.shader_pixel ? pipeline_state.shader_pixel->GetName() : "null");

       // Compute shader hash (which defines the descriptor set layout)
       size_t hash = 0;
       Utility::Hash::hash_combine(hash, pipeline_state.shader_vertex->GetId());
       if (pipeline_state.shader_pixel)
       {
           Utility::Hash::hash_combine(hash, pipeline_state.shader_pixel->GetId());
       }

       // If there is no descriptor set layout for this particular hash, create one
       if (m_descriptor_set_layouts.find(hash) == m_descriptor_set_layouts.end())
       {
           // Generate descriptors from the reflected shaders
           vector<RHI_Descriptor> descriptors = GenerateDescriptors(pipeline_state);

           // Emplace a new descriptor set layout
           m_descriptor_set_layouts.emplace(make_pair(hash, make_shared<RHI_DescriptorSetLayout>(m_rhi_device, descriptors)));
       }

       // Get the descriptor set layout we will be using
        m_descriptor_layout_current = m_descriptor_set_layouts.at(hash).get();
        m_descriptor_layout_current->NeedsToBind();
    }

    void RHI_DescriptorCache::SetConstantBuffer(const uint32_t slot, RHI_ConstantBuffer* constant_buffer)
    {
        if (!m_descriptor_layout_current)
        {
            LOG_ERROR("Invalid descriptor set layout");
            return;
        }

        m_descriptor_layout_current->SetConstantBuffer(slot, constant_buffer);
    }

    void RHI_DescriptorCache::SetSampler(const uint32_t slot, RHI_Sampler* sampler)
    {
        if (!m_descriptor_layout_current)
        {
            LOG_ERROR("Invalid descriptor set layout");
            return;
        }

        m_descriptor_layout_current->SetSampler(slot, sampler);
    }

    void RHI_DescriptorCache::SetTexture(const uint32_t slot, RHI_Texture* texture)
    {
        if (!m_descriptor_layout_current)
        {
            LOG_ERROR("Invalid descriptor set layout");
            return;
        }

        m_descriptor_layout_current->SetTexture(slot, texture);
    }

    void* RHI_DescriptorCache::GetResource_DescriptorSetLayout() const
    {
        if (!m_descriptor_layout_current)
        {
            LOG_ERROR("Invalid descriptor set layout");
            return nullptr;
        }

        return m_descriptor_layout_current->GetResource_DescriptorSetLayout();
    }

    bool RHI_DescriptorCache::GetResource_DescriptorSet(void*& descriptor_set)
    {
        if (!m_descriptor_layout_current)
        {
            LOG_ERROR("Invalid descriptor set layout");
            return nullptr;
        }

        return m_descriptor_layout_current->GetResource_DescriptorSet(this, descriptor_set);
    }

    const std::vector<uint32_t>& RHI_DescriptorCache::GetDynamicOffsets() const
    {
        if (!m_descriptor_layout_current)
        {
            LOG_ERROR("Invalid descriptor set layout");
            return std::vector<uint32_t>();
        }

        return m_descriptor_layout_current->GetDynamicOffsets();
    }

    bool RHI_DescriptorCache::HasEnoughCapacity() const
    {
        return m_descriptor_set_capacity > GetDescriptorSetCount();
    }

    void RHI_DescriptorCache::GrowIfNeeded()
    {
        // If there is room for at least one more descriptor set (hence +1), we don't need to re-allocate yet
        const uint32_t required_capacity = GetDescriptorSetCount() + 1;

        // If we are over-budget, re-allocate the descriptor pool with double size
        if (required_capacity > m_descriptor_set_capacity)
        {
            m_descriptor_set_capacity *= 2;
            SetDescriptorSetCapacity(m_descriptor_set_capacity);
            LOG_INFO("Capacity has been increased to %d elements", m_descriptor_set_capacity);
        }
    }

    uint32_t RHI_DescriptorCache::GetDescriptorSetCount() const
    {
        uint32_t descriptor_set_count = 0;
        for (const auto& it : m_descriptor_set_layouts)
        {
            descriptor_set_count += it.second->GetDescriptorSetCount();
        }

        return descriptor_set_count;
    }

    vector<RHI_Descriptor> RHI_DescriptorCache::GenerateDescriptors(RHI_PipelineState& pipeline_state)
    {
        vector<RHI_Descriptor> descriptors;

        if (!pipeline_state.shader_vertex)
        {
            LOG_ERROR("Vertex shader is invalid");
            return descriptors;
        }

        // Wait for shader to compile
        while (pipeline_state.shader_vertex->GetCompilationState() == Shader_Compilation_Compiling) {}

        // Get vertex shader descriptors
        descriptors = pipeline_state.shader_vertex->GetDescriptors();

        // If there is a pixel shader, merge it's resources into our map as well
        if (pipeline_state.shader_pixel)
        {
            while (pipeline_state.shader_pixel->GetCompilationState() == Shader_Compilation_Compiling) {}
            for (const RHI_Descriptor& descriptor_reflected : pipeline_state.shader_pixel->GetDescriptors())
            {
                // Assume that the descriptor has been created in the vertex shader and only try to update it's shader stage
                bool updated_existing = false;
                for (RHI_Descriptor& descriptor : descriptors)
                {
                    bool is_same_resource =
                        (descriptor.type == descriptor_reflected.type) &&
                        (descriptor.slot == descriptor_reflected.slot);

                    if ((descriptor.type == descriptor_reflected.type) && (descriptor.slot == descriptor_reflected.slot))
                    {
                        descriptor.stage |= descriptor_reflected.stage;
                        updated_existing = true;
                        break;
                    }
                }

                // If no updating took place, this descriptor is new, so add it
                if (!updated_existing)
                {
                    descriptors.emplace_back(descriptor_reflected);
                }
            }
        }

        // Change constant buffers to dynamic (if requested) - This is a hack and not flexible, must improve
        if (pipeline_state.dynamic_constant_buffer_slot != -1)
        {
            for (RHI_Descriptor& descriptor : descriptors)
            {
                if (descriptor.type == RHI_Descriptor_ConstantBuffer)
                {
                    if (descriptor.slot == pipeline_state.dynamic_constant_buffer_slot + m_rhi_device->GetContextRhi()->shader_shift_buffer)
                    {
                        descriptor.type = RHI_Descriptor_ConstantBufferDynamic;
                    }
                }
            }
        }

        return descriptors;
    }
}
