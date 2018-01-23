/*
This file belongs to Renderer.
See LICENSE file in root folder.
*/
namespace renderer
{
	template< typename T >
	Buffer< T >::Buffer( Device const & device
		, uint32_t count
		, BufferTargets target
		, MemoryPropertyFlags flags )
		: m_buffer{ device.createBuffer( uint32_t( count * sizeof( T ) )
			, target
			, flags ) }
	{
	}
}
