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

//= INCLUDES ======================
#include <memory>
#include "RHI_Definition.h"
#include "../Core/Spartan_Object.h"
//=================================

namespace Spartan
{
	class SPARTAN_CLASS RHI_RasterizerState : public Spartan_Object
	{
	public:
        RHI_RasterizerState(
            const std::shared_ptr<RHI_Device>& rhi_device,
            const RHI_Cull_Mode cull_mode,
            const RHI_Fill_Mode fill_mode,
            const bool depth_clip_enabled,
            const bool scissor_enabled,
            const bool multi_sample_enabled,
            const bool antialised_line_enabled,
            const float line_width = 1.0f
		);
		~RHI_RasterizerState();

		auto GetCullMode()				const { return m_cull_mode; }
		auto GetFillMode()				const { return m_fill_mode; }
		auto GetDepthClipEnabled()		const { return m_depth_clip_enabled; }
		auto GetScissorEnabled()		const { return m_scissor_enabled; }
		auto GetMultiSampleEnabled()	const { return m_multi_sample_enabled; }
		auto GetAntialisedLineEnabled() const { return m_antialised_line_enabled; }
		auto IsInitialized()			const { return m_initialized; }
		auto GetResource()				const { return m_buffer; }
        auto GetLineWidth()             const { return m_line_width; }

		bool operator==(const RHI_RasterizerState& rhs) const
		{
			return
				m_cull_mode                 == rhs.GetCullMode()            &&
				m_fill_mode                 == rhs.GetFillMode()            &&
				m_depth_clip_enabled        == rhs.GetDepthClipEnabled()    &&
				m_scissor_enabled           == rhs.GetScissorEnabled()      &&
				m_multi_sample_enabled      == rhs.GetMultiSampleEnabled()  &&
				m_antialised_line_enabled   == rhs.GetAntialisedLineEnabled();
		}

	private:
		// Properties
        RHI_Cull_Mode m_cull_mode       = RHI_Cull_Undefined;
        RHI_Fill_Mode m_fill_mode       = RHI_Fill_Undefined;
		bool m_depth_clip_enabled       = false;
		bool m_scissor_enabled          = false;
        bool m_multi_sample_enabled     = false;
        bool m_antialised_line_enabled  = false;
        float m_line_width              = 1.0f;

		// Initialized
		bool m_initialized = false;

		// Rasterizer state view
		void* m_buffer = nullptr;
	};
}
