#include "FrameBuffer.hpp"
#include "RenderBufferAttachment.hpp"
#include "TextureAttachment.hpp"
#include "RenderBuffer.hpp"

#include "Engine.hpp"

#include "Render/RenderSystem.hpp"
#include "Render/Context.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureImage.hpp"

#include <Log/Logger.hpp>

using namespace Castor;

namespace Castor3D
{
	FrameBuffer::FrameBuffer( Engine & p_engine )
		: OwnedBy< Engine >( p_engine )
	{
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	bool FrameBuffer::Initialise( Castor::Size const & p_size )
	{
		return true;
	}

	void FrameBuffer::Cleanup()
	{
	}

	void FrameBuffer::SetClearColour( Castor::Colour const & p_clrClear )
	{
		m_redClear = p_clrClear.red();
		m_greenClear = p_clrClear.green();
		m_blueClear = p_clrClear.blue();
		m_alphaClear = p_clrClear.alpha();
	}

	void FrameBuffer::SetClearColour( float p_r, float p_g, float p_b, float p_a )
	{
		m_redClear = p_r;
		m_greenClear = p_g;
		m_blueClear = p_b;
		m_alphaClear = p_a;
	}

	void FrameBuffer::Clear( BufferComponents p_targets )
	{
		auto l_targets = p_targets;

		for ( auto l_attach : m_attaches )
		{
			switch ( l_attach->GetAttachmentPoint() )
			{
			case AttachmentPoint::eColour:
				AddFlag( l_targets, BufferComponent::eColour );
				break;

			case AttachmentPoint::eDepth:
				AddFlag( l_targets, BufferComponent::eDepth );
				AddFlag( l_targets, BufferComponent::eStencil );
				break;

			case AttachmentPoint::eStencil:
				AddFlag( l_targets, BufferComponent::eStencil );
				break;
			}
		}

		DoClear( l_targets );
	}

	void FrameBuffer::Bind( FrameBufferMode p_mode, FrameBufferTarget p_target )const
	{
		DoBind( p_target );

		if ( !m_attaches.empty() && p_mode == FrameBufferMode::eAutomatic )
		{
			SetDrawBuffers( m_attaches );
		}
	}

	void FrameBuffer::Unbind()const
	{
#if DEBUG_BUFFERS

		if ( !m_attaches.empty() )
		{
			for ( auto l_attach : m_attaches )
			{
				if ( l_attach->GetAttachmentPoint() == AttachmentPoint::eColour )
				{
					PxBufferBaseSPtr l_buffer = l_attach->GetBuffer();

					if ( l_buffer && DownloadBuffer( l_attach->GetAttachmentPoint(), l_attach->GetAttachmentIndex(), l_buffer ) )
					{
						StringStream l_name;
						l_name << Engine::GetEngineDirectory() << cuT( "\\ColourBuffer_" ) << ( void * )l_buffer.get() << cuT( "_FBA_Unbind.png" );
						Image::BinaryLoader()( Image( cuT( "tmp" ), *l_buffer ), l_name.str() );
					}
				}
			}
		}

#endif

		DoUnbind();
	}

	void FrameBuffer::Attach( AttachmentPoint p_point, uint8_t p_index, TextureAttachmentSPtr p_texture, TextureType p_target, int p_layer )
	{
		p_texture->SetTarget( p_target );
		p_texture->SetLayer( p_layer );
		DoAttach( p_point, p_index, p_texture );
	}

	void FrameBuffer::Attach( AttachmentPoint p_point, TextureAttachmentSPtr p_texture, TextureType p_target, int p_layer )
	{
		Attach( p_point, 0, p_texture, p_target, p_layer );
	}

	void FrameBuffer::Attach( AttachmentPoint p_point, uint8_t p_index, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		DoAttach( p_point, p_index, p_renderBuffer );
	}

	void FrameBuffer::Attach( AttachmentPoint p_point, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		Attach( p_point, 0, p_renderBuffer );
	}

	FrameBufferAttachmentSPtr FrameBuffer::GetAttachment( AttachmentPoint p_point, uint8_t p_index )
	{
		auto l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_point, &p_index]( FrameBufferAttachmentSPtr p_attach )
		{
			return p_attach->GetAttachmentIndex() == p_index && p_attach->GetAttachmentPoint() == p_point;
		} );

		FrameBufferAttachmentSPtr l_return;

		if ( l_it != m_attaches.end() )
		{
			l_return = *l_it;
		}

		return l_return;
	}

	void FrameBuffer::DetachAll()
	{
		for ( auto l_attach : m_attaches )
		{
			l_attach->Detach();
		}

		m_attaches.clear();
	}

	void FrameBuffer::Resize( Castor::Size const & p_size )
	{
		for ( auto l_attach : m_attaches )
		{
			if ( l_attach->GetAttachmentType() == AttachmentType::eTexture )
			{
				std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture()->GetImage().Resize( p_size );
			}
			else
			{
				std::static_pointer_cast< RenderBufferAttachment >( l_attach )->GetRenderBuffer()->Resize( p_size );
			}
		}
	}

	void FrameBuffer::BlitInto( FrameBuffer const & p_target, Castor::Rectangle const & p_rectSrcDst, FlagCombination< BufferComponent > const & p_components )const
	{
		p_target.Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw );
		Bind( FrameBufferMode::eManual, FrameBufferTarget::eRead );
		DoBlitInto( p_target, p_rectSrcDst, p_components );
		Unbind();
		p_target.Unbind();
	}

	void FrameBuffer::StretchInto( FrameBuffer const & p_target, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
		p_target.Bind( FrameBufferMode::eManual, FrameBufferTarget::eDraw );
		Bind( FrameBufferMode::eManual, FrameBufferTarget::eRead );
		DoStretchInto( p_target, p_rectSrc, p_rectDst, p_components, p_interpolation );
		Unbind();
		p_target.Unbind();
	}

	void FrameBuffer::SetDrawBuffers()const
	{
		SetDrawBuffers( m_attaches );
	}

	void FrameBuffer::SetDrawBuffer( TextureAttachmentSPtr p_attach )const
	{
		SetDrawBuffers( AttachArray( 1, p_attach ) );
	}

	void FrameBuffer::SetDrawBuffer( RenderBufferAttachmentSPtr p_attach )const
	{
		SetDrawBuffers( AttachArray( 1, p_attach ) );
	}

	PixelFormat FrameBuffer::DoGetPixelFormat( AttachmentPoint p_point, uint8_t p_index )
	{
		PixelFormat l_return = PixelFormat::eCount;

		if ( !m_attaches.empty() && p_point != AttachmentPoint::eNone )
		{
			auto l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_point]( FrameBufferAttachmentSPtr p_attach )
			{
				return p_attach->GetAttachmentPoint() == p_point;
			} );

			if ( l_it != m_attaches.end() )
			{
				if ( ( *l_it )->GetAttachmentType() == AttachmentType::eTexture )
				{
					TextureAttachmentSPtr l_attach = std::static_pointer_cast< TextureAttachment >( *l_it );
					l_return = l_attach->GetTexture()->GetPixelFormat();
				}
				else
				{
					RenderBufferAttachmentSPtr l_attach = std::static_pointer_cast< RenderBufferAttachment >( *l_it );
					l_return = l_attach->GetRenderBuffer()->GetPixelFormat();
				}
			}
		}

		return l_return;
	}

	void FrameBuffer::DoAttach( AttachmentPoint p_point, uint8_t p_index, FrameBufferAttachmentSPtr p_attach )
	{
		DoDetach( p_point, p_index );
		p_attach->Attach( p_point, p_index );
		m_attaches.push_back( p_attach );
	}

	void FrameBuffer::DoDetach( AttachmentPoint p_point, uint8_t p_index )
	{
		auto l_itAtt = std::find_if( m_attaches.begin(), m_attaches.end(), [p_point, p_index]( FrameBufferAttachmentSPtr p_att )
		{
			return p_att->GetAttachmentPoint() == p_point && p_att->GetAttachmentIndex() == p_index;
		} );

		if ( l_itAtt != m_attaches.end() )
		{
			( *l_itAtt )->Detach();
			m_attaches.erase( l_itAtt );
		}
	}
}
