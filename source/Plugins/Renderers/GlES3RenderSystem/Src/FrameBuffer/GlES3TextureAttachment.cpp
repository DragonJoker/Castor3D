#include "FrameBuffer/GlES3TextureAttachment.hpp"

#include "Common/OpenGlES3.hpp"
#include "FrameBuffer/GlES3FrameBuffer.hpp"
#include "Texture/GlES3Texture.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlES3Render
{
	GlES3TextureAttachment::GlES3TextureAttachment( OpenGlES3 & p_gl, TextureLayoutSPtr p_texture )
		: TextureAttachment( p_texture )
		, Holder( p_gl )
		, m_glAttachmentPoint( GlES3AttachmentPoint::eNone )
		, m_glStatus( GlES3FramebufferStatus::eIncompleteMissingAttachment )
	{
	}

	GlES3TextureAttachment::~GlES3TextureAttachment()
	{
	}

	void GlES3TextureAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, InterpolationMode p_interpolation )
	{
		REQUIRE( GetFrameBuffer()->IsComplete() );
		GlES3FrameBufferSPtr l_pBuffer = std::static_pointer_cast< GlES3FrameBuffer >( p_buffer );
		GetOpenGlES3().BindFramebuffer( GlES3FrameBufferMode::eRead, std::static_pointer_cast< GlES3FrameBuffer >( GetFrameBuffer() )->GetGlES3Name() );
		GetOpenGlES3().BindFramebuffer( GlES3FrameBufferMode::eDraw, l_pBuffer->GetGlES3Name() );
		GetOpenGlES3().ReadBuffer( GlES3BufferBinding( uint32_t( GetOpenGlES3().Get( GetOpenGlES3().Get( GetAttachmentPoint() ) ) ) + GetAttachmentIndex() ) );

		if ( m_glAttachmentPoint == GlES3AttachmentPoint::eDepth )
		{
			GetOpenGlES3().BlitFramebuffer( p_rectSrc, p_rectDst, uint32_t( GlES3BufferBit::eDepth ), GlES3InterpolationMode::eNearest );
		}
		else if ( m_glAttachmentPoint == GlES3AttachmentPoint::eStencil )
		{
			GetOpenGlES3().BlitFramebuffer( p_rectSrc, p_rectDst, uint32_t( GlES3BufferBit::eStencil ), GlES3InterpolationMode::eNearest );
		}
		else
		{
			GetOpenGlES3().BlitFramebuffer( p_rectSrc, p_rectDst, uint32_t( GlES3BufferBit::eColour ), GetOpenGlES3().Get( p_interpolation ) );
		}

		GetOpenGlES3().BindFramebuffer( GlES3FrameBufferMode::eRead, 0 );
		GetOpenGlES3().BindFramebuffer( GlES3FrameBufferMode::eDraw, 0 );
	}

	void GlES3TextureAttachment::DoAttach()
	{
		m_glAttachmentPoint = GlES3AttachmentPoint( uint32_t( GetOpenGlES3().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
		auto l_pTexture = std::static_pointer_cast< GlES3Texture >( GetTexture() );

		switch ( GetTarget() )
		{
		case TextureType::eOneDimension:
		case TextureType::eTwoDimensions:
			GetOpenGlES3().FramebufferTexture2D( GlES3FrameBufferMode::eDefault, m_glAttachmentPoint, GetOpenGlES3().Get( TextureType::eTwoDimensions ), l_pTexture->GetGlES3Name(), 0 );
			break;

		case TextureType::eThreeDimensions:
		case TextureType::eTwoDimensionsArray:
		case TextureType::eCube:
		case TextureType::eCubeArray:
			GetOpenGlES3().FramebufferTextureLayer( GlES3FrameBufferMode::eDefault, m_glAttachmentPoint, l_pTexture->GetGlES3Name(), 0, GetLayer() );
			break;

		default:
			FAILURE( cuT( "Unsupported texture type for this attachment" ) );
			break;
		}

		m_glStatus = GlES3FramebufferStatus( GetOpenGlES3().CheckFramebufferStatus( GlES3FrameBufferMode::eDefault ) );

		if ( m_glStatus != GlES3FramebufferStatus::eUnsupported )
		{
			m_glStatus = GlES3FramebufferStatus::eComplete;
		}
	}

	void GlES3TextureAttachment::DoDetach()
	{
		if ( GetOpenGlES3().HasFbo() )
		{
			auto l_pTexture = GetTexture();

			if ( m_glStatus != GlES3FramebufferStatus::eUnsupported )
			{
				switch ( GetTarget() )
				{
				case TextureType::eOneDimension:
				case TextureType::eTwoDimensions:
					GetOpenGlES3().FramebufferTexture2D( GlES3FrameBufferMode::eDefault, m_glAttachmentPoint, GetOpenGlES3().Get( TextureType::eTwoDimensions ), 0, 0 );
					break;

				case TextureType::eThreeDimensions:
				case TextureType::eTwoDimensionsArray:
					GetOpenGlES3().FramebufferTextureLayer( GlES3FrameBufferMode::eDefault, m_glAttachmentPoint, 0, 0, GetLayer() );
					break;
				}
			}

			m_glAttachmentPoint = GlES3AttachmentPoint::eNone;
		}
	}

	void GlES3TextureAttachment::DoClear( BufferComponent p_component )const
	{
		GetOpenGl().Clear( GetOpenGl().GetComponents( p_component ) );
	}
}
