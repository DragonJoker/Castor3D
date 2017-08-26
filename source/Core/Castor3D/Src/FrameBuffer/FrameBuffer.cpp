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

	void FrameBuffer::setClearColour( castor::Colour const & colour )
	{
		m_redClear = colour.red();
		m_greenClear = colour.green();
		m_blueClear = colour.blue();
		m_alphaClear = colour.alpha();
	}

	void FrameBuffer::setClearColour( float r, float g, float b, float a )
	{
		m_redClear = r;
		m_greenClear = g;
		m_blueClear = b;
		m_alphaClear = a;
	}

	void FrameBuffer::clear( BufferComponents targets )const
	{
		doClear( targets );
	}

	void FrameBuffer::bind( FrameBufferTarget target )const
	{
		doBind( target );
	}

	void FrameBuffer::unbind()const
	{
		doUnbind();
	}

	void FrameBuffer::attach( AttachmentPoint point
		, uint8_t index
		, TextureAttachmentSPtr texture
		, TextureType target
		, int layer )
	{
		texture->setTarget( target );
		texture->setLayer( layer );
		doAttach( point, index, texture );
	}

	void FrameBuffer::attach( AttachmentPoint point
		, TextureAttachmentSPtr texture
		, TextureType target
		, int layer )
	{
		attach( point, 0, texture, target, layer );
	}

	void FrameBuffer::attach( AttachmentPoint point
		, uint8_t index
		, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		doAttach( point, index, p_renderBuffer );
	}

	void FrameBuffer::attach( AttachmentPoint point
		, RenderBufferAttachmentSPtr p_renderBuffer )
	{
		attach( point, 0, p_renderBuffer );
	}

	FrameBufferAttachmentSPtr FrameBuffer::getAttachment( AttachmentPoint point
		, uint8_t index )
	{
		auto it = std::find_if( m_attaches.begin()
			, m_attaches.end()
			, [&point, &index]( FrameBufferAttachmentSPtr p_attach )
			{
				return p_attach->getAttachmentIndex() == index && p_attach->getAttachmentPoint() == point;
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

	void FrameBuffer::resize( castor::Size const & size )
	{
		for ( auto attach : m_attaches )
		{
			if ( attach->getAttachmentType() == AttachmentType::eTexture )
			{
				std::static_pointer_cast< TextureAttachment >( attach )->getTexture()->getImage().resize( size );
			}
			else
			{
				std::static_pointer_cast< RenderBufferAttachment >( attach )->getRenderBuffer()->resize( size );
			}
		}
	}

	void FrameBuffer::blitInto( FrameBuffer const & target
		, castor::Rectangle const & rectSrcDst
		, BufferComponents const & components )const
	{
		target.bind( FrameBufferTarget::eDraw );
		bind( FrameBufferTarget::eRead );
		doBlitInto( target, rectSrcDst, components );
		unbind();
		target.unbind();
	}

	void FrameBuffer::stretchInto( FrameBuffer const & target
		, castor::Rectangle const & rectSrc
		, castor::Rectangle const & rectDst
		, BufferComponents const & components
		, InterpolationMode interpolation )const
	{
		target.bind( FrameBufferTarget::eDraw );
		bind( FrameBufferTarget::eRead );
		doStretchInto( target, rectSrc, rectDst, components, interpolation );
		unbind();
		target.unbind();
	}

	void FrameBuffer::setDrawBuffers()const
	{
		setDrawBuffers( m_attaches );
	}

	void FrameBuffer::setDrawBuffer( FrameBufferAttachmentSPtr attach )const
	{
		setDrawBuffers( AttachArray( 1, attach ) );
	}

	PixelFormat FrameBuffer::doGetPixelFormat( AttachmentPoint point
		, uint8_t index )
	{
		PixelFormat result = PixelFormat::eCount;

		if ( !m_attaches.empty() && point != AttachmentPoint::eNone )
		{
			auto it = std::find_if( m_attaches.begin()
				, m_attaches.end()
				, [&point]( FrameBufferAttachmentSPtr attach )
				{
					return attach->getAttachmentPoint() == point;
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

	void FrameBuffer::doAttach( AttachmentPoint point
		, uint8_t index
		, FrameBufferAttachmentSPtr attach )
	{
		doDetach( point, index );
		attach->attach( point, index );
		m_attaches.push_back( attach );
	}

	void FrameBuffer::doDetach( AttachmentPoint point
		, uint8_t index )
	{
		auto itAtt = std::find_if( m_attaches.begin()
			, m_attaches.end()
			, [point, index]( FrameBufferAttachmentSPtr attach )
			{
				return attach->getAttachmentPoint() == point && attach->getAttachmentIndex() == index;
			} );

		if ( itAtt != m_attaches.end() )
		{
			( *itAtt )->detach();
			m_attaches.erase( itAtt );
		}
	}
}
