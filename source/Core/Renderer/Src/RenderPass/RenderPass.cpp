/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
#include "RenderPass/RenderPass.hpp"

#include "RenderPass/FrameBuffer.hpp"
#include "Core/Device.hpp"
#include "Command/CommandBuffer.hpp"
#include "RenderPass/RenderPassState.hpp"
#include "RenderPass/RenderSubpass.hpp"
#include "Image/Texture.hpp"

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
