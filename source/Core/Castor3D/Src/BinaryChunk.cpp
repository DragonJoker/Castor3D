﻿#include "BinaryChunk.hpp"

#include <numeric>

#include <BinaryFile.hpp>

using namespace Castor;

namespace Castor3D
{
	BinaryChunk::BinaryChunk()
		:	m_eChunkType( eCHUNK_TYPE_UNKNOWN )
		,	m_index( 0 )
	{
	}

	BinaryChunk::BinaryChunk( eCHUNK_TYPE p_type )
		:	m_eChunkType( p_type )
		,	m_index( 0 )
	{
	}

	void BinaryChunk::Finalise()
	{
		uint32_t l_size = std::accumulate( m_addedData.begin(), m_addedData.end(), 0, [&]( uint32_t p_value, ByteArray const & p_array )
		{
			return p_value + uint32_t( p_array.size() );
		} );
		m_pData.resize( l_size );
		size_t l_index = 0;
		std::for_each( m_addedData.begin(), m_addedData.end(), [&]( ByteArray const & p_array )
		{
			std::memcpy( &m_pData[l_index], p_array.data(), p_array.size() );
			l_index += p_array.size();
		} );
	}

	void BinaryChunk::Add( uint8_t * p_data, uint32_t p_size )
	{
		ByteArray l_buffer( p_data, p_data + p_size );
		m_addedData.push_back( l_buffer );
	}

	void BinaryChunk::Get( uint8_t * p_data, uint32_t p_size )
	{
		std::memcpy( p_data, &m_pData[m_index], p_size );
		m_index += p_size;
	}

	bool BinaryChunk::CheckAvailable( uint32_t p_size )const
	{
		return m_index + p_size <= m_pData.size();
	}

	uint32_t BinaryChunk::GetRemaining()const
	{
		return uint32_t( m_pData.size() - m_index );
	}

	bool BinaryChunk::GetSubChunk( BinaryChunk & p_chunkDst )
	{
		uint32_t l_uiDataSize = 0;
		// First we retrieve the chunk type
		bool l_return = DoRead( &p_chunkDst.m_eChunkType, 1 );

		if ( l_return )
		{
			// Then the chunk data size
			l_return = DoRead( &l_uiDataSize, 1 );
		}

		if ( l_return )
		{
			l_return = m_index + l_uiDataSize <= m_pData.size();
		}

		if ( l_return )
		{
			// Eventually we retrieve the chunk data
			p_chunkDst.m_pData.insert( p_chunkDst.m_pData.end(), m_pData.begin() + m_index, m_pData.begin() + m_index + l_uiDataSize );
			p_chunkDst.m_index = 0;
			m_index += l_uiDataSize;
		}

		return l_return;
	}

	bool BinaryChunk::AddSubChunk( BinaryChunk const & p_subchunk )
	{
		uint32_t l_uiSize = uint32_t( p_subchunk.m_pData.size() );
		ByteArray l_data;
		l_data.reserve( sizeof( uint32_t ) + sizeof( eCHUNK_TYPE ) + l_uiSize );
		uint8_t const * l_pData = reinterpret_cast< uint8_t const * >( &p_subchunk.m_eChunkType );
		l_data.insert( l_data.end(), l_pData, l_pData + sizeof( eCHUNK_TYPE ) );
		l_pData = reinterpret_cast< uint8_t * >( &l_uiSize );
		l_data.insert( l_data.end(), l_pData, l_pData + sizeof( uint32_t ) );
		l_data.insert( l_data.end(), p_subchunk.m_pData.begin(), p_subchunk.m_pData.end() );
		Add( l_data.data(), uint32_t( l_data.size() ) );
		return true;
	}

	bool BinaryChunk::Write( Castor::BinaryFile & p_file )
	{
		bool l_return = true;

		if ( l_return )
		{
			l_return = p_file.Write( m_eChunkType ) == sizeof( eCHUNK_TYPE );
		}

		if ( l_return )
		{
			Finalise();
			l_return = p_file.Write( GetDataSize() ) == sizeof( uint32_t );
		}

		if ( l_return )
		{
			l_return = p_file.WriteArray( m_pData.data(), m_pData.size() ) == m_pData.size();
		}

		return l_return;
	}

	bool BinaryChunk::Read( Castor::BinaryFile & p_file )
	{
		uint32_t l_uiDataSize = 0;
		bool l_return = p_file.Read( m_eChunkType ) == sizeof( eCHUNK_TYPE );

		if ( l_return )
		{
			l_return = p_file.Read( l_uiDataSize ) == sizeof( uint32_t );
		}

		if ( l_return )
		{
			m_pData.resize( l_uiDataSize );
			l_return = p_file.ReadArray( m_pData.data(), m_pData.size() ) == m_pData.size();
		}

		return l_return;
	}
}
