/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EquirectangularToCube_H___
#define ___C3D_EquirectangularToCube_H___
#pragma once

#include "Castor3D/RenderToTexture/RenderCube.hpp"

#include <Ashes/RenderPass/FrameBuffer.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>

namespace castor3d
{
	class EquirectangularToCube
		: private RenderCube
	{
	public:
		EquirectangularToCube( TextureLayout const & equiRectangular
			, RenderSystem & renderSystem
			, TextureLayout const & target );
		void render();

	private:
		struct FrameBuffer
		{
			ashes::TextureViewPtr view;
			ashes::FrameBufferPtr frameBuffer;
		};
		std::array< FrameBuffer, 6u > m_frameBuffers;
		ashes::Device const & m_device;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::TextureView const & m_view;
		ashes::RenderPassPtr m_renderPass;
	};
}

#endif
