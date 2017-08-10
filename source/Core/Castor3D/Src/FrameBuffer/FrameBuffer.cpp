#include "FrameBuffer.hpp"
#include "RenderBufferAttachment.hpp"
#include "TextureAttachment.hpp"
#include "RenderBuffer.hpp"

#include "Engine.hpp"

#include "Texture/TextureLayout.hpp"

using namespace castor;

namespace castor3d
{
	FrameBuffer::FrameBuffer( Engine & engine )
		: OwnedBy< Engine >( engine )
	{
	}

	FrameBuffer::~FrameBuffer()
	{
	}

	bool FrameBuffer::initialise( castor::Size const & p_size )
	{
		return true;
	}

	void FrameBuffer::cleanup()
	{
	}

	void FrameBuffer::setClearColour( castor::Colour const & p_clrClear )
	{
		m_redClear = p_clrClear.red();
		m_greenClear = p_clrClear.green();
		m_blueClear = p_clrClear.blue();
		m_alphaClear = p_clrClear.alpha();
	}

	void FrameBuffer::setClearColour( float p_r, float p_g, float p_b, float p_a )
	{
		m_redClear = p_r;
		m_greenClear = p_g;
		m_blueClear = p_b;
		m_alphaClear = p_a;
	}

	void FrameBuffer::clear( BufferComponents p_targets )
	{
		doClear( p_targets );
	}

	void FrameBuffer::bind( FrameBufferTarget p_target )const
	{
		doBind( p_target );
	}

	void FrameBuffer::unbind()const
	{
		doUnbind();
	}

	void FrameBuffer::attach( AttachmentPoint p_point, uint8_t p_index, TextureAttachmentSPtr p_texture, TextureType p_target, int p_layer )
	{
		p_texture->setTarget( p_target );
		p_texture->setLayer( p_layer );
		doAttach( p_point, p_index, p_texture );
	}

	void FrameBuffer::attach( AttachmentPoint p_point, TextureAttachmentSPtr p_texture, TextureType p_target, int p_layer )
	{
		attach( p_point, 0, p_texture, p_target, p_layer );
	}

	void FrameBuffer::attach( AttachmentPoint p_point, uint8_t p_index, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		doAttach( p_point, p_index, p_renderBuffer );
	}

	void FrameBuffer::attach( AttachmentPoint p_point, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		attach( p_point, 0, p_renderBuffer );
	}

	FrameBufferAttachmentSPtr FrameBuffer::getAttachment( AttachmentPoint p_point, uint8_t p_index )
	{
		auto it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_point, &p_index]( FrameBufferAttachmentSPtr p_attach )
		{
			return p_attach->getAttachmentIndex() == p_index && p_attach->getAttachmentPoint() == p_point;
		} );

		FrameBufferAttachmentSPtr result;

		if ( it != m_attaches.end() )
		{
			result = *it;
		}

		return result;
	}

	void FrameBuffer::detachAll()
	{
		for ( auto attach : m_attaches )
		{
			attach->detach();
		}

		m_attaches.clear();
	}

	void FrameBuffer::resize( castor::Size const & p_size )
	{
		for ( auto attach : m_attaches )
		{
			if ( attach->getAttachmentType() == AttachmentType::eTexture )
			{
				std::static_pointer_cast< TextureAttachment >( attach )->getTexture()->getImage().resize( p_size );
			}
			else
			{
				std::static_pointer_cast< RenderBufferAttachment >( attach )->getRenderBuffer()->resize( p_size );
			}
		}
	}

	void FrameBuffer::blitInto( FrameBuffer const & p_target, castor::Rectangle const & p_rectSrcDst, FlagCombination< BufferComponent > const & p_components )const
	{
		p_target.bind( FrameBufferTarget::eDraw );
		bind( FrameBufferTarget::eRead );
		doBlitInto( p_target, p_rectSrcDst, p_components );
		unbind();
		p_target.unbind();
	}

	void FrameBuffer::stretchInto( FrameBuffer const & p_target, castor::Rectangle const & p_rectSrc, castor::Rectangle const & p_rectDst, FlagCombination< BufferComponent > const & p_components, InterpolationMode p_interpolation )const
	{
		p_target.bind( FrameBufferTarget::eDraw );
		bind( FrameBufferTarget::eRead );
		doStretchInto( p_target, p_rectSrc, p_rectDst, p_components, p_interpolation );
		unbind();
		p_target.unbind();
	}

	void FrameBuffer::setDrawBuffers()const
	{
		setDrawBuffers( m_attaches );
	}

	void FrameBuffer::setDrawBuffer( FrameBufferAttachmentSPtr p_attach )const
	{
		setDrawBuffers( AttachArray( 1, p_attach ) );
	}

	PixelFormat FrameBuffer::doGetPixelFormat( AttachmentPoint p_point, uint8_t p_index )
	{
		PixelFormat result = PixelFormat::eCount;

		if ( !m_attaches.empty() && p_point != AttachmentPoint::eNone )
		{
			auto it = std::find_if( m_attaches.begin(), m_attaches.end(), [&p_point]( FrameBufferAttachmentSPtr p_attach )
			{
				return p_attach->getAttachmentPoint() == p_point;
			} );

			if ( it != m_attaches.end() )
			{
				if ( ( *it )->getAttachmentType() == AttachmentType::eTexture )
				{
					TextureAttachmentSPtr attach = std::static_pointer_cast< TextureAttachment >( *it );
					result = attach->getTexture()->getPixelFormat();
				}
				else
				{
					RenderBufferAttachmentSPtr attach = std::static_pointer_cast< RenderBufferAttachment >( *it );
					result = attach->getRenderBuffer()->getPixelFormat();
				}
			}
		}

		return result;
	}

	void FrameBuffer::doAttach( AttachmentPoint p_point, uint8_t p_index, FrameBufferAttachmentSPtr p_attach )
	{
		doDetach( p_point, p_index );
		p_attach->attach( p_point, p_index );
		m_attaches.push_back( p_attach );
	}

	void FrameBuffer::doDetach( AttachmentPoint p_point, uint8_t p_index )
	{
		auto itAtt = std::find_if( m_attaches.begin(), m_attaches.end(), [p_point, p_index]( FrameBufferAttachmentSPtr p_att )
		{
			return p_att->getAttachmentPoint() == p_point && p_att->getAttachmentIndex() == p_index;
		} );

		if ( itAtt != m_attaches.end() )
		{
			( *itAtt )->detach();
			m_attaches.erase( itAtt );
		}
	}
}
