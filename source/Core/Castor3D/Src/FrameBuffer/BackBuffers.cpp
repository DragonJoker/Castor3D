#include "BackBuffers.hpp"

#include "Engine.hpp"

#include <Graphics/PixelBufferBase.hpp>

using namespace Castor;

namespace Castor3D
{
	BackBuffers::BackBuffers( Engine & engine )
		: FrameBuffer( engine )
	{
	}

	BackBuffers::~BackBuffers()
	{
	}

	bool BackBuffers::Initialise( Size const & p_size, PixelFormat p_format )
	{
		m_buffer = PxBufferBase::create( p_size, p_format );
		return true;
	}

	void BackBuffers::DoBind( FrameBufferTarget p_target )const
	{
		FAILURE( "BackBuffers instances can't be bound like regular FrameBuffer ones, use BackBuffers::Bind( WindowBuffer, eFRAME_BUFFER_TARGET )." );
	}

	void BackBuffers::DoUnbind()const
	{
	}
}
