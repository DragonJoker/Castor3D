#include "FrameBuffer/GlTextureAttachment.hpp"

#include "Common/OpenGl.hpp"
#include "FrameBuffer/GlFrameBuffer.hpp"
#include "Texture/GlTexture.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GlRender
{
	GlTextureAttachment::GlTextureAttachment( OpenGl & p_gl, TextureLayoutSPtr p_texture )
		: TextureAttachment( p_texture )
		, Holder( p_gl )
		, m_glAttachmentPoint( GlAttachmentPoint::eNone )
		, m_glStatus( GlFramebufferStatus::eIncompleteMissingAttachment )
	{
	}

	GlTextureAttachment::~GlTextureAttachment()
	{
	}

	void GlTextureAttachment::DoAttach()
	{
		m_glAttachmentPoint = GlAttachmentPoint( uint32_t( GetOpenGl().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
		auto l_pTexture = std::static_pointer_cast< GlTexture >( GetTexture() );

		switch ( GetTarget() )
		{
		case TextureType::eOneDimension:

			if ( l_pTexture->GetType() == TextureType::eOneDimension )
			{
				GetOpenGl().FramebufferTexture1D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0 );
			}
			else
			{
				GetOpenGl().FramebufferTexture( GlFrameBufferMode::eDefault, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
			}

			break;

		case TextureType::eTwoDimensions:

			if ( l_pTexture->GetType() == TextureType::eTwoDimensions )
			{
				GetOpenGl().FramebufferTexture2D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0 );
			}
			else
			{
				GetOpenGl().FramebufferTexture( GlFrameBufferMode::eDefault, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
			}

			break;

		case TextureType::eThreeDimensions:

			if ( l_pTexture->GetType() == TextureType::eThreeDimensions )
			{
				GetOpenGl().FramebufferTexture3D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0, GetLayer() );
			}
			else
			{
				GetOpenGl().FramebufferTexture( GlFrameBufferMode::eDefault, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
			}

			break;

		case TextureType::eTwoDimensionsArray:
			GetOpenGl().FramebufferTextureLayer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, l_pTexture->GetGlName(), 0, GetLayer() );
			break;

		case TextureType::eCube:
			GetOpenGl().FramebufferTexture( GlFrameBufferMode::eDefault, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
			break;

		case TextureType::eCubeArray:
			GetOpenGl().FramebufferTextureLayer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, l_pTexture->GetGlName(), 0, GetLayer() );
			break;

		default:
			FAILURE( cuT( "Unsupported texture type for this attachment" ) );
			break;
		}

		m_glStatus = GlFramebufferStatus( GetOpenGl().CheckFramebufferStatus( GlFrameBufferMode::eDefault ) );

		if ( m_glStatus != GlFramebufferStatus::eUnsupported )
		{
			m_glStatus = GlFramebufferStatus::eComplete;
		}
	}

	void GlTextureAttachment::DoDetach()
	{
		if ( GetOpenGl().HasFbo() )
		{
			auto l_pTexture = GetTexture();

			if ( m_glStatus != GlFramebufferStatus::eUnsupported )
			{
				switch ( GetTarget() )
				{
				case TextureType::eOneDimension:
					GetOpenGl().FramebufferTexture1D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), 0, 0 );
					break;

				case TextureType::eTwoDimensions:
					GetOpenGl().FramebufferTexture2D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), 0, 0 );
					break;

				case TextureType::eThreeDimensions:
					GetOpenGl().FramebufferTexture3D( GlFrameBufferMode::eDefault, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), 0, 0, GetLayer() );
					break;

				case TextureType::eTwoDimensionsArray:
					GetOpenGl().FramebufferTextureLayer( GlFrameBufferMode::eDefault, m_glAttachmentPoint, 0, 0, GetLayer() );
					break;
				}
			}

			m_glAttachmentPoint = GlAttachmentPoint::eNone;
		}
	}

	void GlTextureAttachment::DoClear( Colour const & p_colour )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eColour
			, GetAttachmentIndex()
			, p_colour.const_ptr() );
	}

	void GlTextureAttachment::DoClear( float p_depth )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eDepth
			, GetAttachmentIndex()
			, &p_depth );
	}

	void GlTextureAttachment::DoClear( int p_stencil )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eStencil
			, GetAttachmentIndex()
			, &p_stencil );
	}

	void GlTextureAttachment::DoClear( float p_depth, int p_stencil )const
	{
		GetOpenGl().ClearBuffer( GlComponent::eDepthStencil
			, GetAttachmentIndex()
			, p_depth
			, p_stencil );
	}
}
