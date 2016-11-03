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
		, m_glAttachmentPoint( GlAttachmentPoint::None )
		, m_glStatus( GlFramebufferStatus::IncompleteMissingAttachment )
	{
	}

	GlTextureAttachment::~GlTextureAttachment()
	{
	}

	bool GlTextureAttachment::Blit( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, InterpolationMode p_interpolation )
	{
		bool l_return = false;

		if ( GetOpenGl().HasFbo() )
		{
			l_return = GetFrameBuffer()->IsComplete();
			GlFrameBufferSPtr l_pBuffer = std::static_pointer_cast< GlFrameBuffer >( p_buffer );

			if ( l_return )
			{
				l_return = GetOpenGl().BindFramebuffer( GlFrameBufferMode::Read, std::static_pointer_cast< GlFrameBuffer >( GetFrameBuffer() )->GetGlName() );
			}

			if ( l_return )
			{
				l_return = GetOpenGl().BindFramebuffer( GlFrameBufferMode::Draw, l_pBuffer->GetGlName() );
			}

			if ( l_return )
			{
				l_return = GetOpenGl().ReadBuffer( GlBufferBinding( uint32_t( GetOpenGl().Get( GetOpenGl().Get( GetAttachmentPoint() ) ) ) + GetAttachmentIndex() ) );
			}

			if ( l_return )
			{
				if ( m_glAttachmentPoint == GlAttachmentPoint::Depth )
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, uint32_t( GlBufferBit::Depth ), GlInterpolationMode::Nearest );
				}
				else if ( m_glAttachmentPoint == GlAttachmentPoint::Stencil )
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, uint32_t( GlBufferBit::Stencil ), GlInterpolationMode::Nearest );
				}
				else
				{
					l_return = GetOpenGl().BlitFramebuffer( p_rectSrc, p_rectDst, uint32_t( GlBufferBit::Colour ), GetOpenGl().Get( p_interpolation ) );
				}
			}

			GetOpenGl().BindFramebuffer( GlFrameBufferMode::Read, 0 );
			GetOpenGl().BindFramebuffer( GlFrameBufferMode::Draw, 0 );
		}

		return l_return;
	}

	bool GlTextureAttachment::DoAttach()
	{
		bool l_return = false;

		if ( GetOpenGl().HasFbo() )
		{
			m_glAttachmentPoint = GlAttachmentPoint( uint32_t( GetOpenGl().Get( GetAttachmentPoint() ) ) + GetAttachmentIndex() );
			auto l_pTexture = std::static_pointer_cast< GlTexture >( GetTexture() );

			switch ( GetTarget() )
			{
			case TextureType::OneDimension:

				if ( l_pTexture->GetType() == TextureType::OneDimension )
				{
					l_return = GetOpenGl().FramebufferTexture1D( GlFrameBufferMode::Default, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0 );
				}
				else
				{
					l_return = GetOpenGl().FramebufferTexture( GlFrameBufferMode::Default, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case TextureType::TwoDimensions:

				if ( l_pTexture->GetType() == TextureType::TwoDimensions )
				{
					l_return = GetOpenGl().FramebufferTexture2D( GlFrameBufferMode::Default, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0 );
				}
				else
				{
					l_return = GetOpenGl().FramebufferTexture( GlFrameBufferMode::Default, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case TextureType::ThreeDimensions:

				if ( l_pTexture->GetType() == TextureType::ThreeDimensions )
				{
					l_return = GetOpenGl().FramebufferTexture3D( GlFrameBufferMode::Default, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), l_pTexture->GetGlName(), 0, GetLayer() );
				}
				else
				{
					l_return = GetOpenGl().FramebufferTexture( GlFrameBufferMode::Default, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
				}

				break;

			case TextureType::TwoDimensionsArray:
				l_return = GetOpenGl().FramebufferTextureLayer( GlFrameBufferMode::Default, m_glAttachmentPoint, l_pTexture->GetGlName(), 0, GetLayer() );
				break;

			case TextureType::Cube:
				l_return = GetOpenGl().FramebufferTexture( GlFrameBufferMode::Default, m_glAttachmentPoint, l_pTexture->GetGlName(), 0 );
				break;

			case TextureType::CubeArray:
				l_return = GetOpenGl().FramebufferTextureLayer( GlFrameBufferMode::Default, m_glAttachmentPoint, l_pTexture->GetGlName(), 0, GetLayer() );
				break;
			}

			if ( l_return )
			{
				m_glStatus = GlFramebufferStatus( GetOpenGl().CheckFramebufferStatus( GlFrameBufferMode::Default ) );

				if ( m_glStatus != GlFramebufferStatus::Unsupported )
				{
					m_glStatus = GlFramebufferStatus::Complete;
				}
			}
			else
			{
				m_glStatus = GlFramebufferStatus::Unsupported;
			}
		}

		return l_return;
	}

	void GlTextureAttachment::DoDetach()
	{
		if ( GetOpenGl().HasFbo() )
		{
			auto l_pTexture = GetTexture();

			if ( m_glStatus != GlFramebufferStatus::Unsupported )
			{
				switch ( GetTarget() )
				{
				case TextureType::OneDimension:
					GetOpenGl().FramebufferTexture1D( GlFrameBufferMode::Default, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), 0, 0 );
					break;

				case TextureType::TwoDimensions:
					GetOpenGl().FramebufferTexture2D( GlFrameBufferMode::Default, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), 0, 0 );
					break;

				case TextureType::ThreeDimensions:
					GetOpenGl().FramebufferTexture3D( GlFrameBufferMode::Default, m_glAttachmentPoint, GetOpenGl().Get( l_pTexture->GetType() ), 0, 0, GetLayer() );
					break;

				case TextureType::TwoDimensionsArray:
					GetOpenGl().FramebufferTextureLayer( GlFrameBufferMode::Default, m_glAttachmentPoint, 0, 0, GetLayer() );
					break;
				}
			}

			m_glAttachmentPoint = GlAttachmentPoint::None;
		}
	}
}
