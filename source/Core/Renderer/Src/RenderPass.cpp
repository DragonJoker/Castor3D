/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass.hpp"

#include "FrameBuffer.hpp"
#include "Device.hpp"
#include "CommandBuffer.hpp"
#include "RenderPassState.hpp"
#include "RenderSubpass.hpp"
#include "Texture.hpp"

namespace renderer
{
	RenderPass::RenderPass( Device const & device
		, std::vector< PixelFormat > const & formats
		, RenderSubpassPtrArray const & subpasses
		, RenderPassState const & initialState
		, RenderPassState const & finalState
		, bool clear
		, SampleCountFlag samplesCount )
	{
	}
}
