#include "BackBuffers.hpp"

#include "Engine.hpp"

#include <PixelBufferBase.hpp>

using namespace Castor;

namespace Castor3D
{
	BackBuffers::BackBuffers( Engine & p_engine )
		: FrameBuffer( p_engine )
	{
	}

	BackBuffers::~BackBuffers()
	{
	}

	bool BackBuffers::Initialise( Size const & p_size, ePIXEL_FORMAT p_format )
	{
		m_buffer = PxBufferBase::create( p_size, p_format );
		return DoInitialise( p_size );
	}

	bool BackBuffers::DoBind( eFRAMEBUFFER_TARGET p_eTarget )
	{
		FAILURE( "BackBuffers instances can't be bound like regular FrameBuffer ones, use BackBuffers::Bind( eBUFFER, eFRAME_BUFFER_TARGET )." );
		return false;
	}

	void BackBuffers::DoUnbind()
	{
#if DEBUG_BUFFERS

		if ( m_buffer && DownloadBuffer( eATTACHMENT_POINT_COLOUR, 0, m_buffer ) )
		{
			StringStream l_name;
			l_name << Engine::GetEngineDirectory() << cuT( "\\ColourBuffer_" ) << ( void * )m_buffer.get() << cuT( "_BACK.png" );
			Image::BinaryLoader()( Image( cuT( "tmp" ), *m_buffer ), l_name.str() );
		}

#endif
	}
}
