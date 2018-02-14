#include "RenderTechniqueFbo.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/FrameBufferAttachment.hpp"
#include "Technique/RenderTechnique.hpp"
#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	RenderTechniqueFbo::RenderTechniqueFbo( RenderTechnique & technique )
		: m_technique{ technique }
	{
	}

	bool RenderTechniqueFbo::initialise( Size size )
	{
		m_colourTexture = m_technique.getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
			, AccessType::eNone
			, AccessType::eRead | AccessType::eWrite
			, PixelFormat::eRGBA16F32F
			, size );
		m_colourTexture->getImage().initialiseSource();
		size = m_colourTexture->getDimensions();

		bool result = m_colourTexture->initialise();

		if ( result )
		{
			m_frameBuffer = m_technique.getEngine()->getRenderSystem()->createFrameBuffer();
			m_depthBuffer = m_technique.getEngine()->getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eD24S8
				, size );
			m_depthBuffer->getImage().initialiseSource();
			result = m_depthBuffer->initialise();
		}

		if ( result )
		{
			m_colourAttach = m_frameBuffer->createAttachment( m_colourTexture );
			m_depthAttach = m_frameBuffer->createAttachment( m_depthBuffer );
			result = m_frameBuffer->initialise();
		}

		if ( result )
		{
			m_frameBuffer->bind();
			m_frameBuffer->attach( AttachmentPoint::eColour, 0, m_colourAttach, m_colourTexture->getType() );
			m_frameBuffer->attach( AttachmentPoint::eDepthStencil, m_depthAttach, m_depthBuffer->getType() );
			m_frameBuffer->setDrawBuffer( m_colourAttach );
			result = m_frameBuffer->isComplete();
			m_frameBuffer->unbind();
		}

		return result;
	}

	void RenderTechniqueFbo::cleanup()
	{
		if ( m_frameBuffer )
		{
			m_frameBuffer->bind();
			m_frameBuffer->detachAll();
			m_frameBuffer->unbind();
			m_frameBuffer->cleanup();
			m_colourTexture->cleanup();
			m_depthBuffer->cleanup();

			m_depthAttach.reset();
			m_depthBuffer.reset();
			m_colourAttach.reset();
			m_colourTexture.reset();
			m_frameBuffer.reset();
		}
	}
}
