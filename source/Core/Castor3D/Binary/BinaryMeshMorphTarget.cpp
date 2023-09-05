#include "Castor3D/Binary/BinaryMeshMorphTarget.hpp"

#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp"
#include "Castor3D/Animation/Animable.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"

namespace castor3d
{
	//*************************************************************************************************

	namespace v1_3
	{
		template< typename T >
		using OldInterleavedVertexTArray = std::vector< OldInterleavedVertexT< T > >;

		template< typename T >
		void convert( OldInterleavedVertexTArray< T > const & in
			, InterleavedVertexNoMikkArray & out )
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
		inline void dispatchVertices( InterleavedVertexNoMikkArray const & src
			, castor::Point3fArray & pos
			, castor::Point3fArray & nml
			, castor::Point3fArray & tex )
		{
			pos.reserve( src.size() );
			nml.reserve( src.size() );
			tex.reserve( src.size() );

			for ( auto & vertex : src )
			{
				pos.push_back( vertex.pos );
				nml.push_back( vertex.nml );
				tex.push_back( vertex.tex );
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< MeshMorphTarget >::doWrite( MeshMorphTarget const & obj )
	{
		auto result = doWriteChunk( double( obj.getTimeIndex().count() ) / 1000.0, ChunkType::eMeshMorphTargetTime, m_chunk );

		for ( auto & it : obj )
		{
			if ( result )
			{
				result = doWriteChunk( uint32_t( it.first ), ChunkType::eMeshMorphTargetSubmeshID, m_chunk );
			}

			if ( result )
			{
				result = doWriteChunk( it.second, ChunkType::eMeshMorphTargetWeights, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	template<>
	castor::String BinaryParserBase< MeshMorphTarget >::Name = cuT( "MeshMorphTarget" );

	bool BinaryParser< MeshMorphTarget >::doParse( MeshMorphTarget & obj )
	{
		bool result = true;
		BinaryChunk chunk{ doIsLittleEndian() };
		uint32_t id{ 0 };
		double time{ 0.0 };
		SubmeshRPtr submesh{};
		std::vector< float > weights;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eMeshMorphTargetTime:
				result = doParseChunk( time, chunk );
				checkError( result, "Couldn't parse time index." );
				obj.doSetTimeIndex( castor::Milliseconds{ int64_t( time * 1000 ) } );
				break;
			case ChunkType::eMeshMorphTargetSubmeshID:
				submesh = {};
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
			case ChunkType::eMeshMorphTargetWeights:
				weights.resize( submesh->getMorphTargetsCount() );
				result = doParseChunk( weights, chunk );
				checkError( result, "Couldn't parse submesh morph targets weights." );
				if ( result )
				{
					obj.setTargetsWeights( *submesh
						, std::move( weights ) );
				}
				break;
			default:
				break;
			}
		}


		return result;
	}

	bool BinaryParser< MeshMorphTarget >::doParse_v1_3( MeshMorphTarget & obj )
	{
		bool result = true;

		if ( m_fileVersion <= Version{ 1, 3, 0 } )
		{
			v1_3::OldInterleavedVertexTArray< double > pointsd;
			InterleavedVertexNoMikkArray points;
			BinaryChunk chunk{ doIsLittleEndian() };

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
					result = doParseChunk( pointsd, chunk );
					checkError( result, "Couldn't parse buffer data." );

					if ( result )
					{
						SubmeshAnimationBuffer buffer;
						v1_3::convert( pointsd, points );
						v1_5::dispatchVertices( points
							, buffer.positions
							, buffer.normals
							, buffer.texcoords0 );
					}
					break;
				default:
					break;
				}
			}
		}

		return result;
	}

	bool BinaryParser< MeshMorphTarget >::doParse_v1_5( MeshMorphTarget & obj )
	{
		bool result = true;

		if ( m_fileVersion <= Version{ 1, 5, 0 } )
		{
			v1_3::OldInterleavedVertexTArray< double > bufferd;
			InterleavedVertexNoMikkArray points;
			BinaryChunk chunk{ doIsLittleEndian() };

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
							, buffer.texcoords0 );
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
