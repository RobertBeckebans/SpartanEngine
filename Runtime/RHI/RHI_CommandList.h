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

//= INCLUDES ==============
#include <vector>
#include "RHI_Definition.h"
//=========================

namespace Spartan
{
	class Profiler;
    class Renderer;

    enum RHI_Cmd_List_State
    {
        RHI_Cmd_List_Idle,
        RHI_Cmd_List_Idle_Sync_Cpu_To_Gpu,
        RHI_Cmd_List_Recording,
        RHI_Cmd_List_Ended
    };

	class SPARTAN_CLASS RHI_CommandList : public RHI_Object
	{
	public:
		RHI_CommandList(uint32_t index, RHI_SwapChain* swap_chain, Context* context);
		~RHI_CommandList();

        // Passes
        bool Begin(RHI_PipelineState& pipeline_state);
        bool End();

        // Clear
        void Clear(RHI_PipelineState& pipeline_state);

		// Draw/Dispatch
		void Draw(uint32_t vertex_count);
		void DrawIndexed(uint32_t index_count, uint32_t index_offset = 0, uint32_t vertex_offset = 0);
        void Dispatch(uint32_t x, uint32_t y, uint32_t z = 1) const;

		// Viewport
		void SetViewport(const RHI_Viewport& viewport) const;

        // Scissor
		void SetScissorRectangle(const Math::Rectangle& scissor_rectangle) const;

		// Vertex buffer
		void SetBufferVertex(const RHI_VertexBuffer* buffer);
        inline void SetBufferVertex(const std::shared_ptr<RHI_VertexBuffer>& buffer) { SetBufferVertex(buffer.get()); }

		// Index buffer
		void SetBufferIndex(const RHI_IndexBuffer* buffer);
        inline void SetBufferIndex(const std::shared_ptr<RHI_IndexBuffer>& buffer) { SetBufferIndex(buffer.get()); }

        // Compute shader
        void SetShaderCompute(const RHI_Shader* shader) const;
        inline void SetShaderCompute(const std::shared_ptr<RHI_Shader>& shader) const { SetShaderCompute(shader.get()); }

		// Constant buffer
        void SetConstantBuffer(const uint32_t slot, const uint8_t scope, RHI_ConstantBuffer* constant_buffer) const;
        inline void SetConstantBuffer(const uint32_t slot, const uint8_t scope, const std::shared_ptr<RHI_ConstantBuffer>& constant_buffer) const { SetConstantBuffer(slot, scope, constant_buffer.get()); }

		// Sampler
        void SetSampler(const uint32_t slot, RHI_Sampler* sampler) const;
        inline void SetSampler(const uint32_t slot, const std::shared_ptr<RHI_Sampler>& sampler) const { SetSampler(slot, sampler.get()); }

		// Texture
        void SetTexture(const uint32_t slot, RHI_Texture* texture);
        inline void SetTexture(const uint32_t slot, const std::shared_ptr<RHI_Texture>& texture) { SetTexture(slot, texture.get()); }
        
        // Submit/Flush
		bool Submit();
        bool Flush();

        // Timestamps
        bool Timestamp_Start(void* query_disjoint = nullptr, void* query_start = nullptr) const;
        bool Timestamp_End(void* query_disjoint = nullptr, void* query_end = nullptr) const;
        float Timestamp_GetDuration(void* query_disjoint = nullptr, void* query_start = nullptr, void* query_end = nullptr);

        static uint32_t Gpu_GetMemory(RHI_Device* rhi_device);
        static uint32_t Gpu_GetMemoryUsed(RHI_Device* rhi_device);
        static bool Gpu_QueryCreate(RHI_Device* rhi_device, void** query = nullptr, RHI_Query_Type type = RHI_Query_Timestamp);
        static void Gpu_QueryRelease(void*& query_object);
        
        // Misc
        void* GetResource_CommandBuffer() const { return m_cmd_buffer; }

	private:
        void MarkAndProfileStart(const RHI_PipelineState* pipeline_state);
        void MarkAndProfileEnd(const RHI_PipelineState* pipeline_state);
        void BeginRenderPass();
        bool BindDescriptorSet();
        bool OnDraw();

        uint32_t m_pass_index                   = 0;
        RHI_Cmd_List_State m_cmd_state          = RHI_Cmd_List_Idle;
		RHI_Pipeline* m_pipeline	            = nullptr; 
        RHI_SwapChain* m_swap_chain             = nullptr;
        Renderer* m_renderer                    = nullptr;
        RHI_PipelineCache* m_pipeline_cache     = nullptr;
        RHI_DescriptorCache* m_descriptor_cache = nullptr;
        RHI_PipelineState* m_pipeline_state     = nullptr;
        RHI_Device* m_rhi_device                = nullptr;
        Profiler* m_profiler                    = nullptr;
        void* m_cmd_buffer                      = nullptr;
        void* m_cmd_list_consumed_fence         = nullptr;
        void* m_query_pool                      = nullptr;
        bool m_render_pass_begun_pipeline_bound = false;
        std::vector<uint64_t> m_timestamps;
        std::vector<bool> m_passes_active;

        // Variables to minimise state changes
        uint32_t m_set_id_buffer_vertex = 0;
        uint32_t m_set_id_buffer_pixel  = 0;
	};
}
