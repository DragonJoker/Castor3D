#include "FrameBuffer.hpp"
#include "RenderBufferAttachment.hpp"
#include "TextureAttachment.hpp"
#include "RenderBuffer.hpp"
#include "DynamicTexture.hpp"
#include "Engine.hpp"
#include "DepthStencilState.hpp"
#include "RasteriserState.hpp"
#include "RenderSystem.hpp"
#include "Context.hpp"

#include <Logger.hpp>

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
		return DoInitialise( p_size );
	}

	void FrameBuffer::Cleanup()
	{
		DoCleanup();
	}

	void FrameBuffer::SetClearColour( Castor::Colour const & p_clrClear )
	{
		m_clearColour = p_clrClear;
		DoUpdateClearColour();
	}

	void FrameBuffer::Clear()
	{
		uint32_t l_targets = 0;

		for ( auto && l_attach : m_attaches )
		{
			if ( l_attach->GetAttachmentPoint() == eATTACHMENT_POINT_COLOUR )
			{
				l_targets |= eBUFFER_COMPONENT_COLOUR;
			}
			else if ( l_attach->GetAttachmentPoint() == eATTACHMENT_POINT_DEPTH )
			{
				l_targets |= eBUFFER_COMPONENT_DEPTH;
			}
			else if ( l_attach->GetAttachmentPoint() == eATTACHMENT_POINT_STENCIL )
			{
				l_targets |= eBUFFER_COMPONENT_STENCIL;
			}
		}

		DoClear( l_targets );
	}

	bool FrameBuffer::Bind( eFRAMEBUFFER_MODE p_mode, eFRAMEBUFFER_TARGET p_eTarget )
	{
		bool l_return = DoBind( p_eTarget );

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
			for ( auto && l_attach : m_attaches )
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

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_attachment, uint8_t p_index, TextureAttachmentSPtr p_texture, eTEXTURE_TARGET p_target, int p_layer )
	{
		p_texture->SetTarget( p_target );
		p_texture->SetLayer( p_layer );
		return DoAttach( p_attachment, p_index, p_texture );
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_attachment, TextureAttachmentSPtr p_texture, eTEXTURE_TARGET p_target, int p_layer )
	{
		return Attach( p_attachment, 0, p_texture, p_target, p_layer );
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_attachment, uint8_t p_index, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		return DoAttach( p_attachment, p_index, p_renderBuffer );
	}

	bool FrameBuffer::Attach( eATTACHMENT_POINT p_attachment, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		return Attach( p_attachment, 0, p_renderBuffer );
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
		for ( auto && l_attach : m_attaches )
		{
			if ( l_attach->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE )
			{
				std::static_pointer_cast< TextureAttachment >( l_attach )->GetTexture()->Resize( p_size );
			}
			else
			{
				std::static_pointer_cast< RenderBufferAttachment >( l_attach )->GetRenderBuffer()->Resize( p_size );
			}
		}

		DoResize( p_size );
	}

	bool FrameBuffer::BlitInto( FrameBufferSPtr p_buffer, Castor::Rectangle const & p_rectSrcDst, uint32_t p_uiComponents )
	{
		bool l_return = p_buffer->Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_DRAW );

		if ( l_return )
		{
			l_return = Bind( eFRAMEBUFFER_MODE_MANUAL, eFRAMEBUFFER_TARGET_READ );
		}

		if ( l_return )
		{
			l_return = DoBlitInto( p_buffer, p_rectSrcDst, p_uiComponents, eINTERPOLATION_MODE_NEAREST );
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

	uint32_t FrameBuffer::DoGetSamplesCount( eATTACHMENT_POINT p_point, uint8_t p_index )
	{
		uint32_t l_return = 0;

		if ( !m_attaches.empty() && p_point != eATTACHMENT_POINT_NONE )
		{
			auto && l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_point]( FrameBufferAttachmentSPtr p_attach )
			{
				return p_attach->GetAttachmentPoint() == p_point;
			} );
			
			if ( l_it != m_attaches.end() )
			{
				if ( ( *l_it )->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE )
				{
					TextureAttachmentSPtr l_attach = std::static_pointer_cast< TextureAttachment >( *l_it );
					l_return = l_attach->GetTexture()->GetSamplesCount();
				}
				else
				{
					RenderBufferAttachmentSPtr l_attach = std::static_pointer_cast< RenderBufferAttachment >( *l_it );
					l_return = l_attach->GetRenderBuffer()->GetSamplesCount();
				}
			}
		}

		return l_return;
	}

	ePIXEL_FORMAT FrameBuffer::DoGetPixelFormat( eATTACHMENT_POINT p_point, uint8_t p_index )
	{
		ePIXEL_FORMAT l_return = ePIXEL_FORMAT_COUNT;

		if ( !m_attaches.empty() && p_point != eATTACHMENT_POINT_NONE )
		{
			auto && l_it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_point]( FrameBufferAttachmentSPtr p_attach )
			{
				return p_attach->GetAttachmentPoint() == p_point;
			} );

			if ( l_it != m_attaches.end() )
			{
				if ( ( *l_it )->GetAttachmentType() == eATTACHMENT_TYPE_TEXTURE )
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

	bool FrameBuffer::DoAttach( eATTACHMENT_POINT p_point, uint8_t p_index, FrameBufferAttachmentSPtr p_attach )
	{
		DoDetach( p_point );
		bool l_return = p_attach->Attach( p_point, p_index, shared_from_this() );

		if ( l_return )
		{
			m_attaches.push_back( p_attach );
		}

		return l_return;
	}

	void FrameBuffer::DoDetach( eATTACHMENT_POINT p_point )
	{
		auto l_itAtt = std::find_if( m_attaches.begin(), m_attaches.end(), [p_point]( FrameBufferAttachmentSPtr p_att )
		{
			return p_att->GetAttachmentPoint() == p_point;
		} );

		if ( l_itAtt != m_attaches.end() )
		{
			( *l_itAtt )->Detach();
			m_attaches.erase( l_itAtt );
		}
	}
}
