#include "FrameBuffer/GlTextureAttachment.hpp"

#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlFrameBuffer.hpp"
#include "Texture/GlTexture.hpp"

using namespace castor3d;
using namespace castor;

namespace GlRender
{
	GlTextureAttachment::GlTextureAttachment( OpenGl & p_gl
		, TextureLayoutSPtr p_texture
		, uint32_t mipLevel )
		: TextureAttachment( p_texture, mipLevel )
		, Holder( p_gl )
		, m_glAttachmentPoint( GlAttachmentPoint::eNone )
		, m_glStatus( GlFramebufferStatus::eIncompleteMissingAttachment )
	{
	}

	GlTextureAttachment::~GlTextureAttachment()
	{
	}

	void GlTextureAttachment::doDownload( castor::Position const & p_offset
		, castor::PxBufferBase & p_buffer )const
	{
		getOpenGl().ReadBuffer( GlBufferBinding( int( GlBufferBinding::eColor0 ) + getAttachmentIndex() ) );
		auto format = getOpenGl().get( p_buffer.format() );
		getOpenGl().ReadPixels( p_offset
			, p_buffer.dimensions()
			, format.Format
			, format.Type
			, p_buffer.ptr() );
	}

	void GlTextureAttachment::doAttach()
	{
		m_glAttachmentPoint = GlAttachmentPoint( uint32_t( getOpenGl().get( getAttachmentPoint() ) ) + getAttachmentIndex() );
		auto pTexture = std::static_pointer_cast< GlTexture >( getTexture() );
		auto mipLevel = getMipLevel();

		if ( pTexture->getType() == TextureType::eTwoDimensionsMS
			|| pTexture->getType() == TextureType::eTwoDimensionsMSArray )
		{
			mipLevel = 0u;
		}

		switch ( getTarget() )
		{
		case TextureType::eOneDimension:

			if ( pTexture->getType() == TextureType::eOneDimension )
			{
				getOpenGl().FramebufferTexture1D( GlFrameBufferMode::eDefault
					, m_glAttachmentPoint
					, getOpenGl().get( pTexture->getType() )
					, pTexture->getGlName()
					, mipLevel );
			}
			else
			{
				getOpenGl().FramebufferTexture( GlFrameBufferMode::eDefault
					, m_glAttachmentPoint
					, pTexture->getGlName()
					, mipLevel );
			}

			break;

		case TextureType::eTwoDimensions:

			if ( pTexture->getType() == TextureType::eTwoDimensions )
			{
				getOpenGl().FramebufferTexture2D( GlFrameBufferMode::eDefault
					, m_glAttachmentPoint
					, getOpenGl().get( pTexture->getType() )
					, pTexture->getGlName()
					, mipLevel );
			}
			else
			{
				getOpenGl().FramebufferTexture( GlFrameBufferMode::eDefault
					, m_glAttachmentPoint
					, pTexture->getGlName()
					, mipLevel );
			}

			break;

		case TextureType::eThreeDimensions:

			if ( pTexture->getType() == TextureType::eThreeDimensions )
			{
				getOpenGl().FramebufferTexture3D( GlFrameBufferMode::eDefault
					, m_glAttachmentPoint
					, getOpenGl().get( pTexture->getType() )
					, pTexture->getGlName()
					, mipLevel
					, getLayer() );
			}
			else
			{
				getOpenGl().FramebufferTexture( GlFrameBufferMode::eDefault
					, m_glAttachmentPoint
					, pTexture->getGlName()
					, mipLevel );
			}

			break;

		case TextureType::eTwoDimensionsArray:
			getOpenGl().FramebufferTextureLayer( GlFrameBufferMode::eDefault
				, m_glAttachmentPoint
				, pTexture->getGlName()
				, mipLevel
				, getLayer() );
			break;

		case TextureType::eCube:
			getOpenGl().FramebufferTexture( GlFrameBufferMode::eDefault
				, m_glAttachmentPoint
				, pTexture->getGlName()
				, mipLevel );
			break;

		case TextureType::eCubeArray:
			getOpenGl().FramebufferTextureLayer( GlFrameBufferMode::eDefault
				, m_glAttachmentPoint
				, pTexture->getGlName()
				, mipLevel
				, getLayer() );
			break;

		default:
			FAILURE( cuT( "Unsupported texture type for this attachment" ) );
			break;
		}

		m_glStatus = GlFramebufferStatus( getOpenGl().CheckFramebufferStatus( GlFrameBufferMode::eDefault ) );
		REQUIRE( m_glStatus == GlFramebufferStatus::eComplete );

		if ( m_glStatus != GlFramebufferStatus::eUnsupported )
		{
			m_glStatus = GlFramebufferStatus::eComplete;
		}
	}

	void GlTextureAttachment::doDetach()
	{
		if ( getOpenGl().hasFbo() )
		{
			auto pTexture = getTexture();
			auto mipLevel = getMipLevel();

			if ( pTexture->getType() == TextureType::eTwoDimensionsMS
				|| pTexture->getType() == TextureType::eTwoDimensionsMSArray )
			{
				mipLevel = 0u;
			}

			if ( m_glStatus != GlFramebufferStatus::eUnsupported )
			{
				switch ( getTarget() )
				{
				case TextureType::eOneDimension:
					getOpenGl().FramebufferTexture1D( GlFrameBufferMode::eDefault
						, m_glAttachmentPoint
						, getOpenGl().get( pTexture->getType() )
						, 0
						, mipLevel );
					break;

				case TextureType::eTwoDimensions:
					getOpenGl().FramebufferTexture2D( GlFrameBufferMode::eDefault
						, m_glAttachmentPoint
						, getOpenGl().get( pTexture->getType() )
						, 0
						, mipLevel );
					break;

				case TextureType::eThreeDimensions:
					getOpenGl().FramebufferTexture3D( GlFrameBufferMode::eDefault
						, m_glAttachmentPoint
						, getOpenGl().get( pTexture->getType() )
						, 0
						, mipLevel
						, getLayer() );
					break;

				case TextureType::eTwoDimensionsArray:
					getOpenGl().FramebufferTextureLayer( GlFrameBufferMode::eDefault
						, m_glAttachmentPoint
						, 0
						, mipLevel
						, getLayer() );
					break;
				}
			}

			m_glAttachmentPoint = GlAttachmentPoint::eNone;
		}
	}

	void GlTextureAttachment::doClear( RgbaColour const & p_colour )const
	{
		getOpenGl().ClearBuffer( GlComponent::eColour
			, getAttachmentIndex()
			, p_colour.constPtr() );
	}

	void GlTextureAttachment::doClear( float p_depth )const
	{
		getOpenGl().ClearBuffer( GlComponent::eDepth
			, getAttachmentIndex()
			, &p_depth );
	}

	void GlTextureAttachment::doClear( int p_stencil )const
	{
		getOpenGl().ClearBuffer( GlComponent::eStencil
			, getAttachmentIndex()
			, &p_stencil );
	}

	void GlTextureAttachment::doClear( float p_depth, int p_stencil )const
	{
		getOpenGl().ClearBuffer( GlComponent::eDepthStencil
			, getAttachmentIndex()
			, p_depth
			, p_stencil );
	}
}
