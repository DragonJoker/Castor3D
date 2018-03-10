/*
This file belongs to RendererLib.
See LICENSE file in root folder.
*/
namespace renderer
{
	template< typename T >
	inline VertexBuffer< T >::VertexBuffer( Device const & device
		, uint32_t count
		, BufferTargets target
		, MemoryPropertyFlags flags )
		: VertexBufferBase{ device
			, uint32_t( count * sizeof( T ) )
			, target
			, flags }
	{
	}
}
