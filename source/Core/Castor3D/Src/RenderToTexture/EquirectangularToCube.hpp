/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EquirectangularToCube_H___
#define ___C3D_EquirectangularToCube_H___
#pragma once

#include "RenderToTexture/RenderCube.hpp"

#include <RenderPass/FrameBuffer.hpp>
#include <RenderPass/RenderPass.hpp>

namespace castor3d
{
	class EquirectangularToCube
		: private RenderCube
	{
	public:
		EquirectangularToCube( TextureLayout const & equiRectangular
			, RenderSystem & renderSystem
			, TextureLayout const & target
			, float gamma );
		void render();

	private:
		struct FrameBuffer
		{
			renderer::TextureViewPtr view;
			renderer::FrameBufferPtr frameBuffer;
		};
		std::array< FrameBuffer, 6u > m_frameBuffers;
		renderer::Device const & m_device;
		renderer::CommandBufferPtr m_commandBuffer;
		renderer::TextureView const & m_view;
		renderer::RenderPassPtr m_renderPass;
	};
}

#endif
