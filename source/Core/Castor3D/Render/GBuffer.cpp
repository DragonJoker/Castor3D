#include "Castor3D/Render/GBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Render/RenderSystem.hpp"

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	GBuffer::GBuffer( Engine & engine )
		: m_engine{ engine }
	{
	}

	void GBuffer::doInitialise( Textures textures )
	{
		auto & device = getCurrentRenderDevice( m_engine );
		m_result = std::move( textures );
		uint32_t index = 0u;
		ashes::ImageViewCreateInfo view
		{
			0u,
			VK_NULL_HANDLE,
			VK_IMAGE_VIEW_TYPE_2D,
			VK_FORMAT_UNDEFINED,
			VkComponentMapping{},
			{ 0u, 0u, 1u, 0u, 1u }
		};

		for ( auto & texture : m_result )
		{
			view->image = *texture;
			view->format = texture->getFormat();
			view->subresourceRange.aspectMask = ashes::getAspectMask( view->format );

			if ( index == 0u )
			{
				m_depthStencilView = texture->createView( view );
				view->subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			}

			m_samplableViews.push_back( texture->createView( view ) );
			++index;
		}

		ashes::SamplerCreateInfo sampler
		{
			0u,
			VK_FILTER_LINEAR,
			VK_FILTER_LINEAR,
			VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VK_FALSE,
			0.0f,
			VK_FALSE,
			VK_COMPARE_OP_NEVER,
			0.0f,
			1.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE,
			VK_FALSE,
		};
		m_sampler = device->createSampler( std::move( sampler ) );
	}

	//*********************************************************************************************
}
