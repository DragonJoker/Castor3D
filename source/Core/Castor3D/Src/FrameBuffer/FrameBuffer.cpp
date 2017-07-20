#include "FrameBuffer.hpp"
#include "RenderBufferAttachment.hpp"
#include "TextureAttachment.hpp"
#include "RenderBuffer.hpp"

#include "Engine.hpp"

#include "Texture/TextureLayout.hpp"

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
		DoClear( p_targets );
	}

	void FrameBuffer::Bind( FrameBufferTarget p_target )const
	{
		DoBind( p_target );
	}

	void FrameBuffer::Unbind()const
	{
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
		auto it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_point, &p_index]( FrameBufferAttachmentSPtr p_attach )
		{
			return p_attach->GetAttachmentIndex() == p_index && p_attach->GetAttachmentPoint() == p_point;
		} );

		FrameBufferAttachmentSPtr result;

		if ( it != m_attaches.end() )
		{
			result = *it;
		}

		return result;
	}

	void FrameBuffer::DetachAll()
	{
		for ( auto attach : m_attaches )
		{
			attach->Detach();
		}

		m_attaches.clear();
	}

	void FrameBuffer::Resize( Castor::Size const & p_size )
	{
		for ( auto attach : m_attaches )
		{
			if ( attach->GetAttachmentType() == AttachmentType::eTexture )
			{
				std::static_pointer_cast< TextureAttachment >( attach )->GetTexture()->GetImage().Resize( p_size );
			}
			else
			{
				std::static_pointer_cast< RenderBufferAttachment >( attach )->GetRenderBuffer()->Resize( p_size );
			}
		}
	}

	void FrameBuffer::BlitInto( FrameBuffer const & p_target, Castor::Rectangle const & p_rectSrcDst, FlagCombination< BufferComponent > const & p_components )const
	{
		p_target.Bind( FrameBufferTarget::eDraw );
		Bind( FrameBufferTarget::eRead );
		DoBlitInto( p_target, p_rectSrcDst, p_components );
		Unbind();
		p_target.Unbind();
	}

	void FrameBuffer::StretchInto( FrameBuffer const & p_target, Castor::Rectangle const & p_rectSrc, Castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
		p_target.Bind( FrameBufferTarget::eDraw );
		Bind( FrameBufferTarget::eRead );
		DoStretchInto( p_target, p_rectSrc, p_rectDst, p_components, p_interpolation );
		Unbind();
		p_target.Unbind();
	}

	void FrameBuffer::SetDrawBuffers()const
	{
		SetDrawBuffers( m_attaches );
	}

	void FrameBuffer::SetDrawBuffer( FrameBufferAttachmentSPtr p_attach )const
	{
		SetDrawBuffers( AttachArray( 1, p_attach ) );
	}

	PixelFormat FrameBuffer::DoGetPixelFormat( AttachmentPoint p_point, uint8_t p_index )
	{
		PixelFormat result = PixelFormat::eCount;

		if ( !m_attaches.empty() && p_point != AttachmentPoint::eNone )
		{
			auto it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_point]( FrameBufferAttachmentSPtr p_attach )
			{
				return p_attach->GetAttachmentPoint() == p_point;
			} );

			if ( it != m_attaches.end() )
			{
				if ( ( *it )->GetAttachmentType() == AttachmentType::eTexture )
				{
					TextureAttachmentSPtr attach = std::static_pointer_cast< TextureAttachment >( *it );
					result = attach->GetTexture()->GetPixelFormat();
				}
				else
				{
					RenderBufferAttachmentSPtr attach = std::static_pointer_cast< RenderBufferAttachment >( *it );
					result = attach->GetRenderBuffer()->GetPixelFormat();
				}
			}
		}

		return result;
	}

	void FrameBuffer::DoAttach( AttachmentPoint p_point, uint8_t p_index, FrameBufferAttachmentSPtr p_attach )
	{
		DoDetach( p_point, p_index );
		p_attach->Attach( p_point, p_index );
		m_attaches.push_back( p_attach );
	}

	void FrameBuffer::DoDetach( AttachmentPoint p_point, uint8_t p_index )
	{
		auto itAtt = std::find_if( m_attaches.begin(), m_attaches.end(), [p_point, p_index]( FrameBufferAttachmentSPtr p_att )
		{
			return p_att->GetAttachmentPoint() == p_point && p_att->GetAttachmentIndex() == p_index;
		} );

		if ( itAtt != m_attaches.end() )
		{
			( *itAtt )->Detach();
			m_attaches.erase( itAtt );
		}
	}
}
