#include "Submesh.hpp"

#include "SubmeshUtils.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Mesh/Skeleton/BonedVertex.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/ShaderStorageBuffer.hpp"
#include "Vertex.hpp"

#include <Miscellaneous/BlockTracker.hpp>

using namespace castor;

//*************************************************************************************************

namespace castor3d
{
	namespace
	{
		template< typename T, typename U >
		inline void doCopyVertices( uint32_t p_count, InterleavedVertexT< T > const * p_src, InterleavedVertexT< U > * p_dst )
		{
			for ( uint32_t i{ 0u }; i < p_count; ++i )
			{
				p_dst->m_pos[0] = U( p_src->m_pos[0] );
				p_dst->m_pos[1] = U( p_src->m_pos[1] );
				p_dst->m_pos[2] = U( p_src->m_pos[2] );
				p_dst->m_nml[0] = U( p_src->m_nml[0] );
				p_dst->m_nml[1] = U( p_src->m_nml[1] );
				p_dst->m_nml[2] = U( p_src->m_nml[2] );
				p_dst->m_tan[0] = U( p_src->m_tan[0] );
				p_dst->m_tan[1] = U( p_src->m_tan[1] );
				p_dst->m_tan[2] = U( p_src->m_tan[2] );
				p_dst->m_bin[0] = U( p_src->m_bin[0] );
				p_dst->m_bin[1] = U( p_src->m_bin[1] );
				p_dst->m_bin[2] = U( p_src->m_bin[2] );
				p_dst->m_tex[0] = U( p_src->m_tex[0] );
				p_dst->m_tex[1] = U( p_src->m_tex[1] );
				p_dst->m_tex[2] = U( p_src->m_tex[2] );
				p_dst++;
				p_src++;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< Submesh >::doWrite( Submesh const & p_obj )
	{
		bool result = true;

		if ( result )
		{
			VertexBuffer const & buffer = p_obj.getVertexBuffer();
			size_t size = buffer.getSize();
			uint32_t stride = buffer.getDeclaration().stride();
			uint32_t count = uint32_t( size / stride );
			result = doWriteChunk( count, ChunkType::eSubmeshVertexCount, m_chunk );

			if ( result )
			{
				InterleavedVertex const * srcbuf = reinterpret_cast< InterleavedVertex const * >( buffer.getData() );
				std::vector< InterleavedVertexT< double > > dstbuf( count );
				doCopyVertices( count, srcbuf, dstbuf.data() );
				result = doWriteChunk( dstbuf, ChunkType::eSubmeshVertex, m_chunk );
			}
		}

		if ( result )
		{
			IndexBuffer const & buffer = p_obj.getIndexBuffer();
			uint32_t count = buffer.getSize() / 3;
			result = doWriteChunk( count, ChunkType::eSubmeshFaceCount, m_chunk );

			if ( result )
			{
				FaceIndices const * srcbuf = reinterpret_cast< FaceIndices const * >( buffer.getData() );
				result = doWriteChunk( srcbuf, buffer.getSize() / 3, ChunkType::eSubmeshFaces, m_chunk );
			}
		}

		if ( result && p_obj.hasBonesBuffer() )
		{
			VertexBuffer const & buffer = p_obj.getBonesBuffer();
			uint32_t stride = buffer.getDeclaration().stride();
			uint32_t count = buffer.getSize() / stride;
			result = doWriteChunk( count, ChunkType::eSubmeshBoneCount, m_chunk );

			if ( result )
			{
				VertexBoneData const * srcbuf = reinterpret_cast< VertexBoneData const * >( buffer.getData() );
				result = doWriteChunk( srcbuf, buffer.getSize() / sizeof( VertexBoneData ), ChunkType::eSubmeshBones, m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Submesh >::doParse( Submesh & p_obj )
	{
		bool result = true;
		String name;
		std::vector< FaceIndices > faces;
		std::vector< VertexBoneData > bones;
		std::vector< InterleavedVertexT< double > > srcbuf;
		uint32_t count{ 0u };
		uint32_t faceCount{ 0u };
		uint32_t boneCount{ 0u };
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSubmeshVertexCount:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					srcbuf.resize( count );
				}

				break;

			case ChunkType::eSubmeshVertex:
				result = doParseChunk( srcbuf, chunk );

				if ( result && !srcbuf.empty() )
				{
					std::vector< InterleavedVertex > dstbuf( srcbuf.size() );
					doCopyVertices( uint32_t( srcbuf.size() ), srcbuf.data(), dstbuf.data() );
					p_obj.addPoints( dstbuf );
				}

				break;

			case ChunkType::eSubmeshBoneCount:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					boneCount = count;
					bones.resize( count );
				}

				break;

			case ChunkType::eSubmeshBones:
				result = doParseChunk( bones, chunk );

				if ( result && boneCount > 0 )
				{
					p_obj.addBoneDatas( bones );
				}

				boneCount = 0u;
				break;

			case ChunkType::eSubmeshFaceCount:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					faceCount = count;
					faces.resize( count );
				}

				break;

			case ChunkType::eSubmeshFaces:
				result = doParseChunk( faces, chunk );

				if ( result && faceCount > 0 )
				{
					p_obj.addFaceGroup( faces );
				}

				faceCount = 0u;
				break;

			default:
				result = false;
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	Submesh::Submesh( Scene & p_scene, Mesh & p_mesh, uint32_t p_id )
		: OwnedBy< Scene >( p_scene )
		, m_defaultMaterial( p_scene.getEngine()->getMaterialCache().getDefaultMaterial() )
		, m_id( p_id )
		, m_parentMesh( p_mesh )
		, m_vertexBuffer
		{
			*p_scene.getEngine(),
			BufferDeclaration
			{
				{
					BufferElementDeclaration( ShaderProgram::Position, uint32_t( ElementUsage::ePosition ), ElementType::eVec3, Vertex::getOffsetPos() ),
					BufferElementDeclaration( ShaderProgram::Normal, uint32_t( ElementUsage::eNormal ), ElementType::eVec3, Vertex::getOffsetNml() ),
					BufferElementDeclaration( ShaderProgram::Tangent, uint32_t( ElementUsage::eTangent ), ElementType::eVec3, Vertex::getOffsetTan() ),
					BufferElementDeclaration( ShaderProgram::Bitangent, uint32_t( ElementUsage::eBitangent ), ElementType::eVec3, Vertex::getOffsetBin() ),
					BufferElementDeclaration( ShaderProgram::Texture, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec3, Vertex::getOffsetTex() ),
				}
			}
		}
		, m_indexBuffer{ *p_scene.getEngine() }
	{
	}

	Submesh::~Submesh()
	{
		cleanup();
	}

	void Submesh::initialise()
	{
		if ( !m_generated )
		{
			doGenerateBuffers();
		}

		if ( !m_initialised )
		{
			m_initialised = m_vertexBuffer.initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );

			if ( m_initialised )
			{
				m_initialised = m_indexBuffer.initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}

			if ( m_initialised && m_animBuffer )
			{
				m_initialised = m_animBuffer->initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}

			if ( m_initialised && m_bonesBuffer )
			{
				m_initialised = m_bonesBuffer->initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}

			if ( m_initialised && m_matrixBuffer )
			{
				m_initialised = m_matrixBuffer->initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}

			if ( m_initialised && m_instancedBonesBuffer )
			{
				m_initialised = m_instancedBonesBuffer->initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
			}

			m_dirty = !m_initialised;
		}
	}

	void Submesh::cleanup()
	{
		m_initialised = false;
		doDestroyBuffers();
		m_faces.clear();
		m_points.clear();
		m_pointsData.clear();
		m_bones.clear();
		m_bonesData.clear();
		m_geometryBuffers.clear();
	}

	void Submesh::computeContainers()
	{
		if ( m_pointsData.size() > 0 )
		{
			Point3r min;
			Point3r max;
			Point3r cur;
			Vertex::getPosition( m_points[0], min );
			Vertex::getPosition( m_points[0], max );
			uint32_t nbVertex = getPointsCount();

			for ( uint32_t i = 1; i < nbVertex; i++ )
			{
				Vertex::getPosition( m_points[i], cur );
				max[0] = std::max( cur[0], max[0] );
				max[1] = std::max( cur[1], max[1] );
				max[2] = std::max( cur[2], max[2] );
				min[0] = std::min( cur[0], min[0] );
				min[1] = std::min( cur[1], min[1] );
				min[2] = std::min( cur[2], min[2] );
			}

			m_box.load( min, max );
			m_sphere.load( m_box );
		}
	}

	uint32_t Submesh::getFaceCount()const
	{
		return std::max( uint32_t( m_faces.size() ), m_indexBuffer.getSize() / 3 );
	}

	uint32_t Submesh::getPointsCount()const
	{
		return std::max< uint32_t >( uint32_t( m_points.size() ), m_vertexBuffer.getSize() / m_vertexBuffer.getDeclaration().stride() );
	}

	int Submesh::isInMyPoints( Point3r const & p_vertex, double p_precision )
	{
		int iReturn = -1;
		int iIndex = 0;
		Point3r ptPos;

		for ( VertexPtrArrayConstIt it = m_points.begin(); it != m_points.end() && iReturn == -1; ++it )
		{
			if ( point::lengthSquared( p_vertex - Vertex::getPosition( ( *it ), ptPos ) ) < p_precision )
			{
				iReturn = int( iIndex );
			}

			iIndex++;
		}

		return iReturn;
	}

	BufferElementGroupSPtr Submesh::addPoint( real x, real y, real z )
	{
		BufferElementGroupSPtr result;
		uint32_t stride = 3 * sizeof( real ) * 5;
		m_pointsData.push_back( ByteArray( stride ) );
		uint8_t * data = m_pointsData.back().data();
		result = std::make_shared< BufferElementGroup >( data, uint32_t( m_points.size() ) );
		Vertex::setPosition( result, x, y, z );
		m_points.push_back( result );
		return result;
	}

	BufferElementGroupSPtr Submesh::addPoint( Point3r const & p_v )
	{
		return addPoint( p_v[0], p_v[1], p_v[2] );
	}

	BufferElementGroupSPtr Submesh::addPoint( real * p_v )
	{
		return addPoint( p_v[0], p_v[1], p_v[2] );
	}

	void Submesh::addPoints( InterleavedVertex const * const p_begin, InterleavedVertex const * const p_end )
	{
		uint32_t stride = m_vertexBuffer.getDeclaration().stride();
		m_pointsData.push_back( ByteArray( std::distance( p_begin, p_end ) * stride ) );
		uint8_t * data = m_pointsData.back().data();

		std::for_each( p_begin, p_end, [this, &data, &stride]( InterleavedVertex const & p_data )
		{
			BufferElementGroupSPtr vertex = std::make_shared< BufferElementGroup >( data, uint32_t( m_points.size() ) );
			Vertex::setPosition( vertex, p_data.m_pos.data() );
			Vertex::setNormal( vertex, p_data.m_nml.data() );
			Vertex::setTangent( vertex, p_data.m_tan.data() );
			Vertex::setBitangent( vertex, p_data.m_bin.data() );
			Vertex::setTexCoord( vertex, p_data.m_tex.data() );
			m_points.push_back( vertex );
			data += stride;
		} );
	}

	void Submesh::addBoneDatas( VertexBoneData const * const p_begin, VertexBoneData const * const p_end )
	{
		uint32_t stride = BonedVertex::Stride;
		m_bonesData.push_back( ByteArray( std::distance( p_begin, p_end ) * stride ) );
		auto data = &( *m_bonesData.rbegin() )[0];

		std::for_each( p_begin, p_end, [this, &data, &stride]( VertexBoneData const & p_data )
		{
			BufferElementGroupSPtr bonesData = std::make_shared< BufferElementGroup >( data, uint32_t( m_bones.size() ) );
			BonedVertex::setBones( bonesData, p_data );
			m_bones.push_back( bonesData );
			data += stride;
		} );

		addFlag( m_programFlags, ProgramFlag::eSkinning );
	}

	Face Submesh::addFace( uint32_t a, uint32_t b, uint32_t c )
	{
		Face result{ a, b, c };

		if ( a < m_points.size() && b < m_points.size() && c < m_points.size() )
		{
			m_faces.push_back( result );
			m_hasNormals = false;
		}
		else
		{
			throw std::range_error( "Submesh::addFace - One or more index out of bound" );
		}

		return result;
	}

	void Submesh::addFaceGroup( FaceIndices const * const p_begin, FaceIndices const * const p_end )
	{
		std::for_each( p_begin, p_end, [this]( FaceIndices const & p_data )
		{
			addFace( p_data.m_index[0], p_data.m_index[1], p_data.m_index[2] );
		} );
	}

	void Submesh::addQuadFace( uint32_t a, uint32_t b, uint32_t c, uint32_t d, Point3r const & p_minUV, Point3r const & p_maxUV )
	{
		addFace( a, b, c );
		addFace( a, c, d );
		Vertex::setTexCoord( m_points[a], p_minUV[0], p_minUV[1] );
		Vertex::setTexCoord( m_points[b], p_maxUV[0], p_minUV[1] );
		Vertex::setTexCoord( m_points[c], p_maxUV[0], p_maxUV[1] );
		Vertex::setTexCoord( m_points[d], p_minUV[0], p_maxUV[1] );
	}

	void Submesh::clearFaces()
	{
		m_faces.clear();
		m_indexBuffer.clear();
	}

	void Submesh::resetGpuBuffers()
	{
		doDestroyBuffers();
		doGenerateBuffers();

		if ( !m_initialised )
		{
			initialise();
		}

		if ( m_initialised )
		{
			for ( auto & geometryBuffers : m_geometryBuffers )
			{
				doInitialiseGeometryBuffers( geometryBuffers );
			}
		}
	}

	void Submesh::resetMatrixBuffers()
	{
		if ( m_matrixBuffer )
		{
			uint32_t count = 0;

			for ( auto it : m_instanceCount )
			{
				count = std::max( count, it.second );
			}

			doGenerateMatrixBuffer( count );
			m_matrixBuffer->initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
		}
	}

	void Submesh::draw( GeometryBuffers const & p_geometryBuffers )
	{
		ENSURE( m_initialised );
		uint32_t size = m_vertexBuffer.getSize() / m_vertexBuffer.getDeclaration().stride();

		if ( !m_indexBuffer.isEmpty() )
		{
			size = m_indexBuffer.getSize();
		}

		if ( m_dirty )
		{
			m_vertexBuffer.upload();

			if ( m_animBuffer )
			{
				m_animBuffer->upload();
			}

			m_dirty = false;
		}

		p_geometryBuffers.draw( size, 0u );
	}

	void Submesh::drawInstanced( GeometryBuffers const & p_geometryBuffers, uint32_t p_count )
	{
		ENSURE( m_initialised );
		uint32_t size = m_vertexBuffer.getSize() / m_vertexBuffer.getDeclaration().stride();

		if ( !m_indexBuffer.isEmpty() )
		{
			size = m_indexBuffer.getSize();
		}

		if ( m_dirty )
		{
			m_vertexBuffer.upload();
			m_dirty = false;
		}

		p_geometryBuffers.drawInstanced( size, 0u, p_count );
	}

	void Submesh::computeFacesFromPolygonVertex()
	{
		SubmeshUtils::computeFacesFromPolygonVertex( *this );
	}

	void Submesh::computeNormals( bool p_reverted )
	{
		if ( !m_hasNormals )
		{
			SubmeshUtils::computeNormals( *this, p_reverted );
			m_hasNormals = true;
		}
	}

	void Submesh::computeNormals( Face const & p_face )
	{
		SubmeshUtils::computeNormals( *this, p_face );
	}

	void Submesh::computeTangents( Face const & p_face )
	{
		SubmeshUtils::computeTangents( *this, p_face );
	}

	void Submesh::computeTangentsFromNormals()
	{
		SubmeshUtils::computeTangentsFromNormals( *this );
	}

	void Submesh::computeTangentsFromBitangents()
	{
		SubmeshUtils::computeTangentsFromBitangents( *this );
	}

	void Submesh::computeBitangents()
	{
		SubmeshUtils::computeBitangents( *this );
	}

	void Submesh::sortByDistance( Point3r const & p_ptCameraPosition )
	{
		ENSURE( m_initialised );

		try
		{
			if ( m_cameraPosition != p_ptCameraPosition )
			{
				if ( m_initialised
					&& !m_indexBuffer.isEmpty()
					&& !m_vertexBuffer.isEmpty() )
				{
					IndexBuffer & indices = m_indexBuffer;
					VertexBuffer & vertices = m_vertexBuffer;

					vertices.bind();
					indices.bind();
					m_cameraPosition = p_ptCameraPosition;
					uint32_t uiIdxSize = indices.getSize();
					uint32_t * pIdx = indices.lock( 0, uiIdxSize, AccessType::eRead | AccessType::eWrite );

					if ( pIdx )
					{
						struct stFACE_DISTANCE
						{
							uint32_t m_index[3];
							double m_distance;
						};
						uint32_t stride = vertices.getDeclaration().stride();
						uint8_t * pVtx = vertices.getData();
						DECLARE_VECTOR( stFACE_DISTANCE, Face );
						FaceArray arraySorted;
						arraySorted.reserve( uiIdxSize / 3 );

						if ( pVtx )
						{
							for ( uint32_t * it = pIdx + 0; it < pIdx + uiIdxSize; it += 3 )
							{
								double dDistance = 0.0;
								Coords3r pVtx1( reinterpret_cast< real * >( &pVtx[it[0] * stride] ) );
								dDistance += point::lengthSquared( pVtx1 - p_ptCameraPosition );
								Coords3r pVtx2( reinterpret_cast< real * >( &pVtx[it[1] * stride] ) );
								dDistance += point::lengthSquared( pVtx2 - p_ptCameraPosition );
								Coords3r pVtx3( reinterpret_cast< real * >( &pVtx[it[2] * stride] ) );
								dDistance += point::lengthSquared( pVtx3 - p_ptCameraPosition );
								stFACE_DISTANCE face = { { it[0], it[1], it[2] }, dDistance };
								arraySorted.push_back( face );
							}

							std::sort( arraySorted.begin(), arraySorted.end(), []( stFACE_DISTANCE const & p_left, stFACE_DISTANCE const & p_right )
							{
								return p_left.m_distance < p_right.m_distance;
							} );

							for ( FaceArrayConstIt it = arraySorted.begin(); it != arraySorted.end(); ++it )
							{
								*pIdx++ = it->m_index[0];
								*pIdx++ = it->m_index[1];
								*pIdx++ = it->m_index[2];
							}
						}

						indices.unlock();
					}

					indices.unbind();
					vertices.unbind();
				}
			}
		}
		catch ( Exception const & p_exc )
		{
			Logger::logError( std::stringstream() << "Submesh::SortFaces - Error: " << p_exc.what() );
		}
	}

	uint32_t Submesh::ref( MaterialSPtr p_material )
	{
		auto it = m_instanceCount.find( p_material );

		if ( it == m_instanceCount.end() )
		{
			m_instanceCount[p_material] = 0;
			it = m_instanceCount.find( p_material );
		}

		return it->second++;
	}

	uint32_t Submesh::unref( MaterialSPtr p_material )
	{
		auto it = m_instanceCount.find( p_material );
		uint32_t result{ 0u };

		if ( it != m_instanceCount.end() )
		{
			result = it->second;

			if ( it->second )
			{
				it->second--;
			}

			if ( !it->second )
			{
				m_instanceCount.erase( it );
			}
		}

		return result;
	}

	uint32_t Submesh::getRefCount( MaterialSPtr p_material )const
	{
		uint32_t result = 0;
		auto it = m_instanceCount.find( p_material );

		if ( it != m_instanceCount.end() )
		{
			result = it->second;
		}

		return result;
	}

	uint32_t Submesh::getMaxRefCount()const
	{
		return uint32_t( std::accumulate( m_instanceCount.begin()
			, m_instanceCount.end()
			, 0u
			, []( uint32_t p_value, auto const & p_pair )
		{
			return std::max( p_value, p_pair.second );
		} ) );
	}

	Topology Submesh::getTopology()const
	{
		Topology result = Topology::eCount;

		for ( auto buffers : m_geometryBuffers )
		{
			result = buffers->getTopology();
		}

		return result;
	}

	void Submesh::setTopology( Topology p_value )
	{
		for ( auto buffers : m_geometryBuffers )
		{
			buffers->setTopology( p_value );
		}
	}

	GeometryBuffersSPtr Submesh::getGeometryBuffers( ShaderProgram const & p_program )
	{
		GeometryBuffersSPtr geometryBuffers;
		auto it = std::find_if( std::begin( m_geometryBuffers ), std::end( m_geometryBuffers ), [&p_program]( GeometryBuffersSPtr p_buffers )
		{
			return &p_buffers->getProgram() == &p_program;
		} );

		if ( it == m_geometryBuffers.end() )
		{
			geometryBuffers = getScene()->getEngine()->getRenderSystem()->createGeometryBuffers( Topology::eTriangles, p_program );
			m_geometryBuffers.push_back( geometryBuffers );
			doInitialiseGeometryBuffers( geometryBuffers );
		}
		else
		{
			geometryBuffers = *it;
		}

		return geometryBuffers;
	}

	void Submesh::setAnimated( bool p_animated )
	{
		if ( p_animated )
		{
			addFlag( m_programFlags, ProgramFlag::eMorphing );
		}
		else
		{
			remFlag( m_programFlags, ProgramFlag::eMorphing );
		}
	}

	void Submesh::doCreateBuffers()
	{
		if ( checkFlag( getProgramFlags(), ProgramFlag::eMorphing ) )
		{
			m_animBuffer = std::make_unique< VertexBuffer >( *getScene()->getEngine(), BufferDeclaration
			{
				{
					BufferElementDeclaration( ShaderProgram::Position2, uint32_t( ElementUsage::ePosition ), ElementType::eVec3, Vertex::getOffsetPos() ),
					BufferElementDeclaration( ShaderProgram::Normal2, uint32_t( ElementUsage::eNormal ), ElementType::eVec3, Vertex::getOffsetNml() ),
					BufferElementDeclaration( ShaderProgram::Tangent2, uint32_t( ElementUsage::eTangent ), ElementType::eVec3, Vertex::getOffsetTan() ),
					BufferElementDeclaration( ShaderProgram::Bitangent2, uint32_t( ElementUsage::eBitangent ), ElementType::eVec3, Vertex::getOffsetBin() ),
					BufferElementDeclaration( ShaderProgram::Texture2, uint32_t( ElementUsage::eTexCoords ), ElementType::eVec3, Vertex::getOffsetTex() ),
				}
			} );
		}
		else if ( getScene()->getEngine()->getRenderSystem()->getGpuInformations().hasInstancing() )
		{
			uint32_t count = 0;

			for ( auto it : m_instanceCount )
			{
				count = std::max( count, it.second );
			}

			if ( count > 1 )
			{
				addFlag( m_programFlags, ProgramFlag::eInstantiation );
				m_matrixBuffer = std::make_unique< VertexBuffer >( *getScene()->getEngine(), BufferDeclaration
				{
					{
						BufferElementDeclaration{ ShaderProgram::Transform, uint32_t( ElementUsage::eTransform ), ElementType::eMat4, 0, 1 },
						BufferElementDeclaration{ ShaderProgram::Material, uint32_t( ElementUsage::eMatIndex ), ElementType::eInt, 64, 1 },
					}
				} );
			}
			else
			{
				m_matrixBuffer.reset();
			}
		}

		if ( checkFlag( m_programFlags, ProgramFlag::eSkinning ) && m_parentMesh.getSkeleton() )
		{
			m_bonesBuffer = std::make_unique< VertexBuffer >( *getScene()->getEngine(), BufferDeclaration
			{
				{
					BufferElementDeclaration{ ShaderProgram::BoneIds0, uint32_t( ElementUsage::eBoneIds0 ), ElementType::eIVec4, 0 },
					BufferElementDeclaration{ ShaderProgram::BoneIds1, uint32_t( ElementUsage::eBoneIds1 ), ElementType::eIVec4, 16 },
					BufferElementDeclaration{ ShaderProgram::Weights0, uint32_t( ElementUsage::eBoneWeights0 ), ElementType::eVec4, 32 },
					BufferElementDeclaration{ ShaderProgram::Weights1, uint32_t( ElementUsage::eBoneWeights1 ), ElementType::eVec4, 48 },
				}
			} );
			ENSURE( m_bonesBuffer->getDeclaration().stride() == BonedVertex::Stride );

			if ( checkFlag( m_programFlags, ProgramFlag::eInstantiation ) )
			{
				m_instancedBonesBuffer = std::make_unique< ShaderStorageBuffer >( *getScene()->getEngine() );
			}
		}
	}

	void Submesh::doDestroyBuffers()
	{
		m_generated = false;
		m_initialised = false;
		m_dirty = true;
		m_vertexBuffer.cleanup();

		if ( !m_indexBuffer.isEmpty() )
		{
			m_indexBuffer.cleanup();
		}

		if ( m_animBuffer )
		{
			m_animBuffer->cleanup();
		}

		if ( m_matrixBuffer )
		{
			m_matrixBuffer->cleanup();
		}

		if ( m_bonesBuffer )
		{
			m_bonesBuffer->cleanup();
		}

		if ( m_instancedBonesBuffer )
		{
			m_instancedBonesBuffer->cleanup();
		}

		for ( auto buffers : m_geometryBuffers )
		{
			buffers->cleanup();
		}
	}

	void Submesh::doGenerateBuffers()
	{
		doCreateBuffers();
		doGenerateVertexBuffer();
		doGenerateIndexBuffer();
		uint32_t count = 0;

		for ( auto it : m_instanceCount )
		{
			count = std::max( count, it.second );
		}

		if ( count > 1 )
		{
			addFlag( m_programFlags, ProgramFlag::eInstantiation );
			doGenerateMatrixBuffer( count );
		}

		if ( !m_bones.empty() )
		{
			doGenerateBonesBuffer();

			if ( count > 1 )
			{
				doGenerateInstantiatedBonesBuffer( count );
			}
		}

		if ( checkFlag( m_programFlags, ProgramFlag::eMorphing ) )
		{
			doGenerateAnimBuffer();
		}

		m_generated = true;
	}

	void Submesh::doGenerateVertexBuffer()
	{
		VertexBuffer & vertexBuffer = m_vertexBuffer;
		uint32_t stride = m_vertexBuffer.getDeclaration().stride();
		uint32_t size = uint32_t( m_points.size() ) * stride;

		if ( size )
		{
			if ( vertexBuffer.getSize() != size )
			{
				vertexBuffer.resize( size );
			}

			auto buffer = vertexBuffer.getData();

			for ( auto it : m_pointsData )
			{
				ENSURE( buffer < vertexBuffer.getData() + vertexBuffer.getSize() );
				ENSURE( ( buffer < vertexBuffer.getData() + vertexBuffer.getSize()
						&& buffer + it.size() <= vertexBuffer.getData() + vertexBuffer.getSize() )
					|| ( buffer == vertexBuffer.getData() + vertexBuffer.getSize() ) );
				std::memcpy( buffer, it.data(), it.size() );
				buffer += it.size();
			}

			buffer = vertexBuffer.getData();

			for ( auto point : m_points )
			{
				ENSURE( buffer < vertexBuffer.getData() + vertexBuffer.getSize() );
				ENSURE( ( buffer < vertexBuffer.getData() + vertexBuffer.getSize()
						&& buffer + stride <= vertexBuffer.getData() + vertexBuffer.getSize() )
					|| ( buffer == vertexBuffer.getData() + vertexBuffer.getSize() ) );
				point->linkCoords( buffer );
				buffer += stride;
			}

			//m_points.clear();
			//m_pointsData.clear();
		}
	}

	void Submesh::doGenerateAnimBuffer()
	{
		if ( m_animBuffer )
		{
			VertexBuffer & vertexBuffer = m_vertexBuffer;
			VertexBuffer & animBuffer = *m_animBuffer;
			uint32_t size = vertexBuffer.getSize();

			if ( size && animBuffer.getSize() != size )
			{
				animBuffer.resize( size );
			}
		}
	}

	void Submesh::doGenerateIndexBuffer()
	{
		FaceSPtr pFace;
		IndexBuffer & indexBuffer = m_indexBuffer;
		uint32_t uiSize = uint32_t( m_faces.size() * 3 );

		if ( uiSize )
		{
			if ( indexBuffer.getSize() != uiSize )
			{
				indexBuffer.resize( uiSize );
			}

			uint32_t index = 0;

			for ( auto const & face : m_faces )
			{
				indexBuffer[index++] = face[0];
				indexBuffer[index++] = face[1];
				indexBuffer[index++] = face[2];
			}

			//m_faces.clear();
		}
	}

	void Submesh::doGenerateBonesBuffer()
	{
		if ( m_bonesBuffer )
		{
			VertexBuffer & bonesBuffer = *m_bonesBuffer;
			uint32_t stride = BonedVertex::Stride;
			uint32_t size = uint32_t( m_bones.size() ) * stride;
			auto itbones = m_bones.begin();

			if ( size )
			{
				if ( bonesBuffer.getSize() != size )
				{
					bonesBuffer.resize( size );
				}

				auto buffer = bonesBuffer.getData();

				for ( auto it : m_bonesData )
				{
					std::memcpy( buffer, it.data(), it.size() );
					buffer += it.size();
				}

				buffer = bonesBuffer.getData();

				for ( auto point : m_bones )
				{
					point->linkCoords( buffer );
					buffer += stride;
				}

				//m_bones.clear();
				//m_bonesData.clear();
			}
		}
	}

	void Submesh::doGenerateMatrixBuffer( uint32_t p_count )
	{
		if ( m_matrixBuffer )
		{
			if ( p_count )
			{
				VertexBuffer & matrixBuffer = *m_matrixBuffer;
				uint32_t size = p_count * matrixBuffer.getDeclaration().stride();

				if ( matrixBuffer.getSize() != size )
				{
					matrixBuffer.resize( size );
				}
			}
			else
			{
				m_matrixBuffer.reset();
			}
		}
	}

	void Submesh::doGenerateInstantiatedBonesBuffer( uint32_t p_count )
	{
		if ( m_instancedBonesBuffer )
		{
			if ( p_count )
			{
				auto & bonesBuffer = *m_instancedBonesBuffer;
				auto stride = uint32_t( sizeof( float ) * 16u * 400u );
				uint32_t size = p_count * stride;

				if ( bonesBuffer.getSize() != size )
				{
					bonesBuffer.resize( size );
				}
			}
			else
			{
				m_instancedBonesBuffer.reset();
			}
		}
	}

	void Submesh::doInitialiseGeometryBuffers( GeometryBuffersSPtr p_geometryBuffers )
	{
		VertexBufferArray buffers;
		buffers.push_back( m_vertexBuffer );

		if ( m_animBuffer )
		{
			buffers.push_back( *m_animBuffer );
		}

		if ( m_bonesBuffer )
		{
			buffers.push_back( *m_bonesBuffer );
		}

		if ( m_matrixBuffer )
		{
			buffers.push_back( *m_matrixBuffer );
		}

		if ( getScene()->getEngine()->getRenderSystem()->getCurrentContext() )
		{
			p_geometryBuffers->initialise( buffers, &m_indexBuffer );
		}
		else
		{
			getScene()->getListener().postEvent( makeFunctorEvent( EventType::ePreRender, [this, p_geometryBuffers, buffers]()
			{
				p_geometryBuffers->initialise( buffers, &m_indexBuffer );
			} ) );
		}
	}
}
