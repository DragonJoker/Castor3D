#include "BackBuffers.hpp"

#include "Engine.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace castor;

namespace castor3d
{
	BackBuffers::BackBuffers( Engine & engine )
		: FrameBuffer( engine )
	{
	}

	BackBuffers::~BackBuffers()
	{
	}

	bool BackBuffers::initialise( Size const & size, PixelFormat format )
	{
		m_buffer = PxBufferBase::create( size, format );
		return true;
	}

	void BackBuffers::doBind( FrameBufferTarget target )const
	{
		FAILURE( "BackBuffers instances can't be bound like regular FrameBuffer ones, use BackBuffers::bind( WindowBuffer, eFRAME_BUFFER_TARGET )." );
	}

	void BackBuffers::doUnbind()const
	{
	}
}
