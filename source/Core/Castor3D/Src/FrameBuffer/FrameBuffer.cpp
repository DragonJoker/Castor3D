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
		m_clearColour = p_clrClear;
	}

	void FrameBuffer::Clear()
	{
		uint32_t l_targets = 0;

		for ( auto l_attach : m_attaches )
		{
			switch ( l_attach->GetAttachmentPoint() )
			{
			case eATTACHMENT_POINT_COLOUR:
				l_targets |= eBUFFER_COMPONENT_COLOUR;
				break;

			case eATTACHMENT_POINT_DEPTH:
				l_targets |= eBUFFER_COMPONENT_DEPTH;
				break;

			case eATTACHMENT_POINT_STENCIL:
				l_targets |= eBUFFER_COMPONENT_STENCIL;
				break;
			}
		}

		DoClear( l_targets );
	}

	bool FrameBuffer::Bind( eFRAMEBUFFER_MODE p_mode, eFRAMEBUFFER_TARGET p_target )
	{
		bool l_return = DoBind( p_target );

		if ( l_return && !m_attaches.empty() && p_mode == eFRAMEBUFFER_MODE_AUTOMATIC )
		{
			SetDrawBuffers( m_attaches );
		}

		return l_return;
	}

	void FrameBuffer::Unbind()
	{
#if DEBUG_BUFFERS

		if ( !m_attaches.empty() )
		{
			for ( auto l_attach : m_attaches )
			{
				if ( l_attach->GetAttachmentPoint() == eATTACHMENT_POINT_COLOUR )
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

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_point, uint8_t p_index, TextureAttachmentSPtr p_texture, TextureType p_target, int p_layer )
	{
		p_texture->SetTarget( p_target );
		p_texture->SetLayer( p_layer );
		return DoAttach( p_point, p_index, p_texture );
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_point, TextureAttachmentSPtr p_texture, TextureType p_target, int p_layer )
	{
		return Attach( p_point, 0, p_texture, p_target, p_layer );
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_point, uint8_t p_index, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		return DoAttach( p_point, p_index, p_renderBuffer );
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_point, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		return Attach( p_point, 0, p_renderBuffer );
	}

	FrameBufferAttachmentSPtr FrameBuffer::GetAttachment( eATTACHMENT_POINT p_point, uint8_t p_index )
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
			if ( l_attach->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE )
			{
				std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture()->GetImage().Resize( p_size );
			}
			else
			{
				std::static_pointer_cast< RenderBufferAttachment >( l_attach )->GetRenderBuffer()->Resize( p_size );
			}
		}
	}

	bool FrameBuffer::BlitInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrcDst, uint32_t p_components )
	{
		bool l_return = p_buffer->Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_DRAW );

		if ( l_return )
		{
			l_return = Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_READ );

			if ( l_return )
			{
				l_return = DoBlitInto( p_buffer, p_rectSrcDst, p_components );
				Unbind();
			}

			p_buffer->Unbind();
		}

		return l_return;
	}

	bool FrameBuffer::StretchInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, uint32_t p_components, InterpolationMode p_interpolation )
	{
		bool l_return = p_buffer->Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_DRAW );

		if ( l_return )
		{
			l_return = Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_READ );

			if ( l_return )
			{
				l_return = DoStretchInto( p_buffer, p_rectSrc, p_rectDst, p_components, p_interpolation );
				Unbind();
			}

			p_buffer->Unbind();
		}

		return l_return;
	}

	void FrameBuffer::SetDrawBuffers()
	{
		SetDrawBuffers( m_attaches );
	}

	void FrameBuffer::SetDrawBuffer( TextureAttachmentSPtr p_attach )
	{
		SetDrawBuffers( AttachArray( 1, p_attach ) );
	}

	void FrameBuffer::SetDrawBuffer( RenderBufferAttachmentSPtr p_attach )
	{
		SetDrawBuffers( AttachArray( 1, p_attach ) );
	}

	PixelFormat FrameBuffer::DoGetPixelFormat( eATTACHMENT_POINT p_point, uint8_t p_index )
	{
		PixelFormat l_return = PixelFormat::Count;

		if ( !m_attaches.empty() && p_point != eATTACHMENT_POINT_NONE )
		{
			auto l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_point]( FrameBufferAttachmentSPtr p_attach )
			{
				return p_attach->GetAttachmentPoint() == p_point;
			} );

			if ( l_it != m_attaches.end() )
			{
				if ( ( *l_it )->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE )
				{
					TextureAttachmentSPtr l_attach = std::static_pointer_cast< TextureAttachment >( *l_it );
					l_return = l_attach->GetTexture()->GetImage().GetPixelFormat();
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

	bool FrameBuffer::DoAttach( eATTACHMENT_POINT p_point, uint8_t p_index, FrameBufferAttachmentSPtr p_attach )
	{
		DoDetach( p_point, p_index );
		bool l_return = p_attach->Attach( p_point, p_index, shared_from_this() );

		if ( l_return )
		{
			m_attaches.push_back( p_attach );
		}

		return l_return;
	}

	void FrameBuffer::DoDetach( eATTACHMENT_POINT p_point, uint8_t p_index )
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
