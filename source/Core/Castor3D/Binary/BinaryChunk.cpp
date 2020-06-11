#include "Castor3D/Binary/BinaryChunk.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Data/BinaryFile.hpp>

#include <numeric>

using namespace castor;

namespace castor3d
{
	BinaryChunk::BinaryChunk()
		: m_type{ ChunkType::eUnknown }
		, m_index{ 0 }
	{
	}

	BinaryChunk::BinaryChunk( ChunkType p_type )
		:	m_type{ p_type }
		,	m_index{ 0 }
	{
	}

	void BinaryChunk::finalise()
	{
		uint32_t size = std::accumulate( m_addedData.begin(), m_addedData.end(), 0, [&]( uint32_t p_value, ByteArray const & p_array )
		{
			return p_value + uint32_t( p_array.size() );
		} );
		m_data.resize( size );
		size_t index = 0;

		for ( auto const & array : m_addedData )
		{
			std::memcpy( &m_data[index], array.data(), array.size() );
			index += array.size();
		}
	}

	void BinaryChunk::add( uint8_t * p_data, uint32_t p_size )
	{
		ByteArray buffer( p_data, p_data + p_size );
		m_addedData.push_back( buffer );
	}

	void BinaryChunk::get( uint8_t * p_data, uint32_t p_size )
	{
		std::memcpy( p_data, &m_data[m_index], p_size );
		m_index += p_size;
	}

	bool BinaryChunk::checkAvailable( uint32_t p_size )const
	{
		return size_t( m_index ) + p_size <= m_data.size();
	}

	uint32_t BinaryChunk::getRemaining()const
	{
		return uint32_t( m_data.size() - m_index );
	}

	bool BinaryChunk::getSubChunk( BinaryChunk & p_chunkDst )
	{
		// First we retrieve the chunk type
		BinaryChunk subchunk;
		bool result = doRead( &subchunk.m_type, 1 );
		uint32_t size = 0;

		if ( result )
		{
			// Then the chunk data size
			result = doRead( &size, 1 );
		}

		if ( result )
		{
			result = size_t( m_index ) + size <= m_data.size();
		}

		if ( result )
		{
			// Eventually we retrieve the chunk data
			subchunk.m_data.insert( subchunk.m_data.end(), m_data.begin() + m_index, m_data.begin() + m_index + size );
			subchunk.m_index = 0;
			m_index += size;
			p_chunkDst = subchunk;
		}

		return result;
	}

	bool BinaryChunk::addSubChunk( BinaryChunk const & p_subchunk )
	{
		uint32_t size = uint32_t( p_subchunk.m_data.size() );
		ByteArray buffer;
		buffer.reserve( sizeof( uint32_t ) + sizeof( ChunkType ) + size );
		// write subchunk type
		auto type = systemEndianToBigEndian( p_subchunk.m_type );
		auto data = reinterpret_cast< uint8_t const * >( &type );
		buffer.insert( buffer.end(), data, data + sizeof( ChunkType ) );
		// The its size
		systemEndianToBigEndian( size );
		data = reinterpret_cast< uint8_t * >( &size );
		buffer.insert( buffer.end(), data, data + sizeof( uint32_t ) );
		// And eventually its data
		buffer.insert( buffer.end(), p_subchunk.m_data.begin(), p_subchunk.m_data.end() );
		// And add it to this chunk
		add( buffer.data(), uint32_t( buffer.size() ) );
		return true;
	}

	bool BinaryChunk::write( castor::BinaryFile & p_file )
	{
		auto type = systemEndianToBigEndian( getChunkType() );
		auto result = p_file.write( type ) == sizeof( ChunkType );

		if ( result )
		{
			finalise();
			auto size = systemEndianToBigEndian( getDataSize() );
			result = p_file.write( size ) == sizeof( uint32_t );
		}

		if ( result )
		{
			result = p_file.writeArray( m_data.data(), m_data.size() ) == m_data.size();
		}

		return result;
	}

	bool BinaryChunk::read( castor::BinaryFile & p_file )
	{
		uint32_t size = 0;
		bool result = p_file.read( m_type ) == sizeof( ChunkType );
		bigEndianToSystemEndian( m_type );

		if ( result )
		{
			result = p_file.read( size ) == sizeof( uint32_t );
			bigEndianToSystemEndian( size );
		}

		if ( result )
		{
			m_data.resize( size );
			result = p_file.readArray( m_data.data(), m_data.size() ) == m_data.size();
		}

		return result;
	}

	void BinaryChunk::binaryError( std::string_view view )
	{
		log::error << view;
	}
}
