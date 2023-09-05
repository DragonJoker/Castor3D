#include "Castor3D/Binary/BinaryMorphComponent.hpp"
#include "Castor3D/Binary/BinarySubmesh.hpp"

#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< MorphComponent >::doWrite( MorphComponent const & obj )
	{
		bool result = true;

		for ( auto & it : obj.getMorphTargetsBuffers() )
		{
			if ( result )
			{
				auto size = uint32_t( std::max( { it.positions.size()
					, it.normals.size()
					, it.tangents.size()
					, it.texcoords0.size()
					, it.texcoords1.size()
					, it.texcoords2.size()
					, it.texcoords3.size()
					, it.colours.size() } ) );
				result = doWriteChunk( size, ChunkType::eMorphTargetBufferSize, m_chunk );
			}

			if ( result
				&& !it.positions.empty() )
			{
				result = doWriteChunk( it.positions, ChunkType::eMorphTargetPositions, m_chunk );
			}

			if ( result
				&& !it.normals.empty() )
			{
				result = doWriteChunk( it.normals, ChunkType::eMorphTargetNormals, m_chunk );
			}

			if ( result
				&& !it.tangents.empty() )
			{
				result = doWriteChunk( it.tangents, ChunkType::eMorphTargetTangentsMikkt, m_chunk );
			}

			if ( result
				&& !it.texcoords0.empty() )
			{
				result = doWriteChunk( it.texcoords0, ChunkType::eMorphTargetTexcoords0, m_chunk );
			}

			if ( result
				&& !it.texcoords1.empty() )
			{
				result = doWriteChunk( it.texcoords1, ChunkType::eMorphTargetTexcoords1, m_chunk );
			}

			if ( result
				&& !it.texcoords2.empty() )
			{
				result = doWriteChunk( it.texcoords2, ChunkType::eMorphTargetTexcoords2, m_chunk );
			}

			if ( result
				&& !it.texcoords3.empty() )
			{
				result = doWriteChunk( it.texcoords3, ChunkType::eMorphTargetTexcoords3, m_chunk );
			}

			if ( result
				&& !it.colours.empty() )
			{
				result = doWriteChunk( it.colours, ChunkType::eMorphTargetColours, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< MorphComponent >::Name = cuT( "MorphComponent" );

	bool BinaryParser< MorphComponent >::doParse( MorphComponent & obj )
	{
		bool result = true;
		SubmeshAnimationBuffer buffer;
		uint32_t count{ 0u };
		std::set< MorphFlag > flags;
		castor::Point3fArray values;
		BinaryChunk chunk{ doIsLittleEndian() };

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMorphTargetBufferSize:
				if ( count )
				{
					obj.addMorphTarget( std::move( buffer ) );
					count = 0u;
				}
				result = doParseChunk( count, chunk );
				values.resize( count );
				checkError( result, "Couldn't parse keyframe buffers size." );
				break;
			case ChunkType::eMorphTargetPositions:
				flags.insert( MorphFlag::ePositions );
				result = doParseChunk( values, chunk );
				checkError( result, "Couldn't parse keyframe positions." );

				if ( result )
				{
					buffer.positions = values;
				}
				break;
			case ChunkType::eMorphTargetNormals:
				flags.insert( MorphFlag::eNormals );
				result = doParseChunk( values, chunk );
				checkError( result, "Couldn't parse keyframe normals." );

				if ( result )
				{
					buffer.normals = values;
				}
				break;
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
			case ChunkType::eMorphTargetTangents:
#pragma GCC diagnostic pop
#pragma warning( pop )
				flags.insert( MorphFlag::eTangents );
				buffer.tangents.resize( count );
				break;
			case ChunkType::eMorphTargetTangentsMikkt:
				{
					flags.insert( MorphFlag::eTangents );
					castor::Point4fArray tangents;
					tangents.resize( count );
					result = doParseChunk( tangents, chunk );
					checkError( result, "Couldn't parse keyframe tangents." );

					if ( result )
					{
						buffer.tangents = tangents;
					}
				}
				break;
			case ChunkType::eMorphTargetTexcoords0:
				flags.insert( MorphFlag::eTexcoords0 );
				result = doParseChunk( values, chunk );
				checkError( result, "Couldn't parse keyframe texcoords0." );

				if ( result )
				{
					buffer.texcoords0 = values;
				}
				break;
			case ChunkType::eMorphTargetTexcoords1:
				flags.insert( MorphFlag::eTexcoords1 );
				result = doParseChunk( values, chunk );
				checkError( result, "Couldn't parse keyframe texcoords1." );

				if ( result )
				{
					buffer.texcoords1 = values;
				}
				break;
			case ChunkType::eMorphTargetTexcoords2:
				flags.insert( MorphFlag::eTexcoords2 );
				result = doParseChunk( values, chunk );
				checkError( result, "Couldn't parse keyframe texcoords2." );

				if ( result )
				{
					buffer.texcoords2 = values;
				}
				break;
			case ChunkType::eMorphTargetTexcoords3:
				flags.insert( MorphFlag::eTexcoords3 );
				result = doParseChunk( values, chunk );
				checkError( result, "Couldn't parse keyframe texcoords3." );

				if ( result )
				{
					buffer.texcoords3 = values;
				}
				break;
			case ChunkType::eMorphTargetColours:
				flags.insert( MorphFlag::eColours );
				result = doParseChunk( values, chunk );
				checkError( result, "Couldn't parse keyframe colours." );

				if ( result )
				{
					buffer.colours = values;
				}
				break;
			default:
				result = false;
				break;
			}
		}

		if ( m_fileVersion < Version{ 1, 7, 0 }
			&& flags.end() != flags.find( MorphFlag::eTangents ) )
		{
			if ( auto indexMapping = obj.getOwner()->getIndexMapping() )
			{
				if ( indexMapping->getType() == TriFaceMapping::Name )
				{
					SubmeshUtils::computeTangentsFromNormals( buffer.positions
						, buffer.texcoords0
						, buffer.normals
						, buffer.tangents
						, static_cast< TriFaceMapping const & >( *indexMapping ).getFaces() );
				}
			}
		}

		obj.m_targetDataCount = uint32_t( flags.size() );

		for ( auto & flag : flags )
		{
			obj.m_flags |= flag;
		}

		if ( result && count )
		{
			obj.addMorphTarget( std::move( buffer ) );
		}

		return result;
	}

	//*************************************************************************************************
}
