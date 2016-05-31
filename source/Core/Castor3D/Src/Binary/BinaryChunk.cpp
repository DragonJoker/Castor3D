#include "BinaryChunk.hpp"

#include <numeric>

#include <BinaryFile.hpp>

using namespace Castor;

namespace Castor3D
{
	BinaryChunk::BinaryChunk()
		: m_type{ eCHUNK_TYPE_UNKNOWN }
		, m_index{ 0 }
	{
	}

	BinaryChunk::BinaryChunk( eCHUNK_TYPE p_type )
		:	m_type{ p_type }
		,	m_index{ 0 }
	{
	}

	void BinaryChunk::Finalise()
	{
		uint32_t l_size = std::accumulate( m_addedData.begin(), m_addedData.end(), 0, [&]( uint32_t p_value, ByteArray const & p_array )
		{
			return p_value + uint32_t( p_array.size() );
		} );
		m_data.resize( l_size );
		size_t l_index = 0;

		for ( auto const & l_array : m_addedData )
		{
			std::memcpy( &m_data[l_index], l_array.data(), l_array.size() );
			l_index += l_array.size();
		}
	}

	void BinaryChunk::Add( uint8_t * p_data, uint32_t p_size )
	{
		ByteArray l_buffer( p_data, p_data + p_size );
		m_addedData.push_back( l_buffer );
	}

	void BinaryChunk::Get( uint8_t * p_data, uint32_t p_size )
	{
		std::memcpy( p_data, &m_data[m_index], p_size );
		m_index += p_size;
	}

	bool BinaryChunk::CheckAvailable( uint32_t p_size )const
	{
		return m_index + p_size <= m_data.size();
	}

	uint32_t BinaryChunk::GetRemaining()const
	{
		return uint32_t( m_data.size() - m_index );
	}

	bool BinaryChunk::GetSubChunk( BinaryChunk & p_chunkDst )
	{
		// First we retrieve the chunk type
		BinaryChunk l_subchunk;
		bool l_return = DoRead( &l_subchunk.m_type, 1 );
		uint32_t l_size = 0;

		if ( l_return )
		{
			// Then the chunk data size
			l_return = DoRead( &l_size, 1 );
		}

		if ( l_return )
		{
			l_return = m_index + l_size <= m_data.size();
		}

		if ( l_return )
		{
			// Eventually we retrieve the chunk data
			l_subchunk.m_data.insert( l_subchunk.m_data.end(), m_data.begin() + m_index, m_data.begin() + m_index + l_size );
			l_subchunk.m_index = 0;
			m_index += l_size;
			p_chunkDst = l_subchunk;
		}

		return l_return;
	}

	bool BinaryChunk::AddSubChunk( BinaryChunk const & p_subchunk )
	{
		uint32_t l_size = uint32_t( p_subchunk.m_data.size() );
		ByteArray l_buffer;
		l_buffer.reserve( sizeof( uint32_t ) + sizeof( eCHUNK_TYPE ) + l_size );
		// Write subchunk type
		auto l_data = reinterpret_cast< uint8_t const * >( &p_subchunk.m_type );
		l_buffer.insert( l_buffer.end(), l_data, l_data + sizeof( eCHUNK_TYPE ) );
		// The its size
		l_data = reinterpret_cast< uint8_t * >( &l_size );
		l_buffer.insert( l_buffer.end(), l_data, l_data + sizeof( uint32_t ) );
		// And eventually its data
		l_buffer.insert( l_buffer.end(), p_subchunk.m_data.begin(), p_subchunk.m_data.end() );
		// And add it to this chunk
		Add( l_buffer.data(), uint32_t( l_buffer.size() ) );
		return true;
	}

	bool BinaryChunk::Write( Castor::BinaryFile & p_file )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = p_file.Write( m_type ) == sizeof( eCHUNK_TYPE );
		}

		if ( l_return )
		{
			Finalise();
			l_return = p_file.Write( GetDataSize() ) == sizeof( uint32_t );
		}

		if ( l_return )
		{
			l_return = p_file.WriteArray( m_data.data(), m_data.size() ) == m_data.size();
		}

		return l_return;
	}

	bool BinaryChunk::Read( Castor::BinaryFile & p_file )
	{
		uint32_t l_uiDataSize = 0;
		bool l_return = p_file.Read( m_type ) == sizeof( eCHUNK_TYPE );

		if ( l_return )
		{
			l_return = p_file.Read( l_uiDataSize ) == sizeof( uint32_t );
		}

		if ( l_return )
		{
			m_data.resize( l_uiDataSize );
			l_return = p_file.ReadArray( m_data.data(), m_data.size() ) == m_data.size();
		}

		return l_return;
	}
}
