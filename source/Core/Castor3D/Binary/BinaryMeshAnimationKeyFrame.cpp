#include "Castor3D/Binary/BinaryMeshAnimationKeyFrame.hpp"

#include "Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"

namespace castor3d
{
	//*************************************************************************************************

	namespace v1_3
	{
		template< typename T >
		using OldInterleavedVertexTArray = std::vector< OldInterleavedVertexT< T > >;

		template< typename T >
		void convert( OldInterleavedVertexTArray< T > const & in
			, InterleavedVertexArray & out )
		{
			out.resize( in.size() );
			auto it = out.begin();

			for ( auto & inVtx : in )
			{
				auto & outVtx = *it;
				outVtx.pos[0] = float( inVtx.pos[0] );
				outVtx.pos[1] = float( inVtx.pos[1] );
				outVtx.pos[2] = float( inVtx.pos[2] );
				outVtx.nml[0] = float( inVtx.nml[0] );
				outVtx.nml[1] = float( inVtx.nml[1] );
				outVtx.nml[2] = float( inVtx.nml[2] );
				outVtx.tan[0] = float( inVtx.tan[0] );
				outVtx.tan[1] = float( inVtx.tan[1] );
				outVtx.tan[2] = float( inVtx.tan[2] );
				outVtx.tex[0] = float( inVtx.tex[0] );
				outVtx.tex[1] = float( inVtx.tex[1] );
				outVtx.tex[2] = float( inVtx.tex[2] );
				++it;
			}
		}
	}

	//*************************************************************************************************

	namespace v1_5
	{
		inline void dispatchVertices( InterleavedVertexArray const & src
			, castor::Point3fArray & pos
			, castor::Point3fArray & nml
			, castor::Point3fArray & tan
			, castor::Point3fArray & tex )
		{
			pos.reserve( src.size() );
			nml.reserve( src.size() );
			tan.reserve( src.size() );
			tex.reserve( src.size() );

			for ( auto & vertex : src )
			{
				pos.push_back( vertex.pos );
				nml.push_back( vertex.nml );
				tan.push_back( vertex.tan );
				tex.push_back( vertex.tex );
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< MeshAnimationKeyFrame >::doWrite( MeshAnimationKeyFrame const & obj )
	{
		auto result = doWriteChunk( double( obj.getTimeIndex().count() ) / 1000.0, ChunkType::eMeshAnimationKeyFrameTime, m_chunk );

		for ( auto & it : obj )
		{
			if ( result )
			{
				result = doWriteChunk( uint32_t( it.first ), ChunkType::eMeshAnimationKeyFrameSubmeshID, m_chunk );
			}

			if ( result )
			{
				auto size = uint32_t( std::max( { it.second.positions.size()
					, it.second.normals.size()
					, it.second.tangents.size()
					, it.second.texcoords.size() } ) );
				result = doWriteChunk( size, ChunkType::eMeshAnimationKeyFrameBufferSize, m_chunk );
			}

			if ( result
				&& !it.second.positions.empty() )
			{
				result = doWriteChunk( it.second.positions, ChunkType::eMeshAnimationKeyFramePositions, m_chunk );
			}

			if ( result
				&& !it.second.normals.empty() )
			{
				result = doWriteChunk( it.second.normals, ChunkType::eMeshAnimationKeyFrameNormals, m_chunk );
			}

			if ( result
				&& !it.second.tangents.empty() )
			{
				result = doWriteChunk( it.second.tangents, ChunkType::eMeshAnimationKeyFrameTangents, m_chunk );
			}

			if ( result
				&& !it.second.texcoords.empty() )
			{
				result = doWriteChunk( it.second.texcoords, ChunkType::eMeshAnimationKeyFrameTexcoords, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< MeshAnimationKeyFrame >::Name = cuT( "MeshAnimationKeyFrame" );

	bool BinaryParser< MeshAnimationKeyFrame >::doParse( MeshAnimationKeyFrame & obj )
	{
		bool result = true;
		castor::Matrix4x4f transform;
		SubmeshAnimationBuffer buffer;
		BinaryChunk chunk;
		uint32_t id{ 0 };
		uint32_t count{ 0 };
		double time{ 0.0 };
		SubmeshSPtr submesh;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMeshAnimationKeyFrameTime:
				result = doParseChunk( time, chunk );
				checkError( result, "Couldn't parse time index." );
				obj.doSetTimeIndex( castor::Milliseconds{ int64_t( time * 1000 ) } );
				break;
			case ChunkType::eMeshAnimationKeyFrameSubmeshID:
				result = doParseChunk( id, chunk );
				checkError( result, "Couldn't parse submesh ID." );
				if ( result )
				{
					submesh = static_cast< Mesh const & >( *obj.getOwner()->getAnimable() ).getSubmesh( id );

					if ( !obj.getOwner()->hasChild( *submesh ) )
					{
						obj.getOwner()->addChild( { *obj.getOwner(), *submesh } );
					}
				}
				break;
			case ChunkType::eMeshAnimationKeyFrameBufferSize:
				result = doParseChunk( count, chunk );
				checkError( result, "Couldn't parse keyframe buffers size." );
				break;
			case ChunkType::eMeshAnimationKeyFramePositions:
				buffer.positions.resize( count );
				result = doParseChunk( buffer.positions, chunk );
				checkError( result, "Couldn't parse keyframe positions." );
				break;
			case ChunkType::eMeshAnimationKeyFrameNormals:
				buffer.normals.resize( count );
				result = doParseChunk( buffer.normals, chunk );
				checkError( result, "Couldn't parse keyframe positions." );
				break;
			case ChunkType::eMeshAnimationKeyFrameTangents:
				buffer.tangents.resize( count );
				result = doParseChunk( buffer.tangents, chunk );
				checkError( result, "Couldn't parse keyframe positions." );
				break;
			case ChunkType::eMeshAnimationKeyFrameTexcoords:
				buffer.texcoords.resize( count );
				result = doParseChunk( buffer.texcoords, chunk );
				checkError( result, "Couldn't parse keyframe positions." );
				break;
			default:
				break;
			}
		}

		if ( result && submesh && count )
		{
			obj.addSubmeshBuffer( *submesh, std::move( buffer ) );
		}

		return result;
	}

	bool BinaryParser< MeshAnimationKeyFrame >::doParse_v1_3( MeshAnimationKeyFrame & obj )
	{
		bool result = true;

		if ( m_fileVersion <= Version{ 1, 3, 0 } )
		{
			v1_3::OldInterleavedVertexTArray< double > pointsd;
			InterleavedVertexArray points;
			BinaryChunk chunk;
			SubmeshSPtr submesh;

			while ( result && doGetSubChunk( chunk ) )
			{
				switch ( chunk.getChunkType() )
				{
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				case ChunkType::eMeshAnimationKeyFrameBufferData:
#pragma GCC diagnostic pop
#pragma warning( pop )
					if ( m_fileVersion <= Version{ 1, 3, 0 } )
					{
						result = doParseChunk( pointsd, chunk );
						checkError( result, "Couldn't parse buffer data." );

						if ( result )
						{
							SubmeshAnimationBuffer buffer;
							v1_3::convert( pointsd, points );
							v1_5::dispatchVertices( points
								, buffer.positions
								, buffer.normals
								, buffer.tangents
								, buffer.texcoords );
							obj.addSubmeshBuffer( *submesh, std::move( buffer ) );
						}
					}
					break;
				default:
					break;
				}
			}
		}

		return result;
	}

	bool BinaryParser< MeshAnimationKeyFrame >::doParse_v1_5( MeshAnimationKeyFrame & obj )
	{
		bool result = true;

		if ( m_fileVersion <= Version{ 1, 5, 0 } )
		{
			v1_3::OldInterleavedVertexTArray< double > bufferd;
			InterleavedVertexArray points;
			BinaryChunk chunk;
			SubmeshSPtr submesh;

			while ( result && doGetSubChunk( chunk ) )
			{
				switch ( chunk.getChunkType() )
				{
#pragma warning( push )
#pragma warning( disable: 4996 )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
				case ChunkType::eMeshAnimationKeyFrameBufferData:
#pragma GCC diagnostic pop
#pragma warning( pop )
					result = doParseChunk( points, chunk );
					checkError( result, "Couldn't parse buffer data." );

					if ( result )
					{
						SubmeshAnimationBuffer buffer;
						v1_5::dispatchVertices( points
							, buffer.positions
							, buffer.normals
							, buffer.tangents
							, buffer.texcoords );
						obj.addSubmeshBuffer( *submesh, std::move( buffer ) );
					}
					break;
				default:
					break;
				}
			}
		}

		return result;
	}

	//*************************************************************************************************
}
