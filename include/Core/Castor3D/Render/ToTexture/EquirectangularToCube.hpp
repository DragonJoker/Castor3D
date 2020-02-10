/*
See LICENSE file in root folder
*/
#ifndef ___C3D_EquirectangularToCube_H___
#define ___C3D_EquirectangularToCube_H___
#pragma once

#include "Castor3D/RenderToTexture/RenderCube.hpp"

#include <ashespp/RenderPass/FrameBuffer.hpp>
#include <ashespp/RenderPass/RenderPass.hpp>

namespace castor3d
{
	class EquirectangularToCube
		: private RenderCube
	{
	public:
		EquirectangularToCube( TextureLayout const & equiRectangular
			, RenderDevice const & device
			, TextureLayout const & target );
		void render();

	private:
		struct FrameBuffer
		{
			ashes::ImageView view;
			ashes::FrameBufferPtr frameBuffer;
		};
		std::array< FrameBuffer, 6u > m_frameBuffers;
		RenderDevice const & m_device;
		ashes::CommandBufferPtr m_commandBuffer;
		ashes::ImageView const & m_view;
		ashes::RenderPassPtr m_renderPass;
	};
}

#endif
