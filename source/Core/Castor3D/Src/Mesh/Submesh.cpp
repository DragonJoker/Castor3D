#include "Submesh.hpp"

#include "SubmeshUtils.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Mesh/Buffer/Buffer.hpp"
#include "Mesh/SubmeshComponent/BonesComponent.hpp"
#include "Mesh/SubmeshComponent/InstantiationComponent.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Vertex.hpp"

using namespace castor;

//*************************************************************************************************

namespace castor3d
{
	namespace
	{
		template< typename T, typename U >
		inline void doCopyVertices( uint32_t count
			, InterleavedVertexT< T > const * src
			, InterleavedVertexT< U > * dst )
		{
			for ( uint32_t i{ 0u }; i < count; ++i )
			{
				dst->m_pos[0] = U( src->m_pos[0] );
				dst->m_pos[1] = U( src->m_pos[1] );
				dst->m_pos[2] = U( src->m_pos[2] );
				dst->m_nml[0] = U( src->m_nml[0] );
				dst->m_nml[1] = U( src->m_nml[1] );
				dst->m_nml[2] = U( src->m_nml[2] );
				dst->m_tan[0] = U( src->m_tan[0] );
				dst->m_tan[1] = U( src->m_tan[1] );
				dst->m_tan[2] = U( src->m_tan[2] );
				dst->m_bin[0] = U( src->m_bin[0] );
				dst->m_bin[1] = U( src->m_bin[1] );
				dst->m_bin[2] = U( src->m_bin[2] );
				dst->m_tex[0] = U( src->m_tex[0] );
				dst->m_tex[1] = U( src->m_tex[1] );
				dst->m_tex[2] = U( src->m_tex[2] );
				dst++;
				src++;
			}
		}
	}

	//*************************************************************************************************

	bool BinaryWriter< Submesh >::doWrite( Submesh const & obj )
	{
		bool result = true;

		if ( result )
		{
			VertexBuffer const & buffer = obj.getVertexBuffer();
			size_t size = buffer.getSize();
			uint32_t stride = buffer.getDeclaration().stride();
			auto count = uint32_t( size / stride );
			result = doWriteChunk( count, ChunkType::eSubmeshVertexCount, m_chunk );

			if ( result )
			{
				auto const * srcbuf = reinterpret_cast< InterleavedVertex const * >( buffer.getData() );
				std::vector< InterleavedVertexT< double > > dstbuf( count );
				doCopyVertices( count, srcbuf, dstbuf.data() );
				result = doWriteChunk( dstbuf, ChunkType::eSubmeshVertex, m_chunk );
			}
		}

		if ( result )
		{
			IndexBuffer const & buffer = obj.getIndexBuffer();
			uint32_t count = buffer.getSize() / 3;
			result = doWriteChunk( count, ChunkType::eSubmeshFaceCount, m_chunk );

			if ( result )
			{
				auto const * srcbuf = reinterpret_cast< FaceIndices const * >( buffer.getData() );
				result = doWriteChunk( srcbuf, buffer.getSize() / 3, ChunkType::eSubmeshFaces, m_chunk );
			}
		}

		if ( result )
		{
			auto it = obj.m_components.find( BonesComponent::Name );

			if ( it != obj.m_components.end() )
			{
				BinaryWriter< BonesComponent >{}.write( *std::static_pointer_cast< BonesComponent >( it->second ), m_chunk );
			}
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< Submesh >::doParse( Submesh & obj )
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
		std::shared_ptr< BonesComponent > bonesComponent;

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
					obj.addPoints( dstbuf );
				}

				break;

			case ChunkType::eSubmeshBoneCount:
				if ( !bonesComponent )
				{
					bonesComponent = std::make_shared< BonesComponent >( obj );
					obj.addComponent( bonesComponent );
				}

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
					bonesComponent->addBoneDatas( bones );
				}

				boneCount = 0u;
				break;

			case ChunkType::eBonesComponent:
				bonesComponent = std::make_shared< BonesComponent >( obj );
				result = BinaryParser< BonesComponent >{}.parse( *bonesComponent, chunk );

				if ( result )
				{
					obj.addComponent( bonesComponent );
				}

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
					auto indexMapping = std::make_shared< TriFaceMapping >( obj );
					indexMapping->addFaceGroup( faces );
					obj.setIndexMapping( indexMapping );
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

	Submesh::Submesh( Scene & scene, Mesh & mesh, uint32_t id )
		: OwnedBy< Scene >( scene )
		, m_defaultMaterial( scene.getEngine()->getMaterialCache().getDefaultMaterial() )
		, m_id( id )
		, m_parentMesh( mesh )
		, m_vertexBuffer
		{
			*scene.getEngine(),
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
		, m_indexBuffer{ *scene.getEngine() }
	{
		addComponent( std::make_shared< InstantiationComponent >( *this ) );
	}

	Submesh::~Submesh()
	{
		cleanup();
	}

	void Submesh::initialise()
	{
		if ( !m_generated )
		{
			doGenerateVertexBuffer();

			for ( auto & component : m_components )
			{
				component.second->initialise();
				component.second->fill();
			}

			m_generated = true;
		}

		if ( !m_initialised )
		{
			m_initialised = m_vertexBuffer.initialise( renderer::MemoryPropertyFlag::eHostVisible );

			if ( m_initialised )
			{
				m_initialised = m_indexBuffer.initialise( renderer::MemoryPropertyFlag::eHostVisible );
			}

			for ( auto & component : m_components )
			{
				m_initialised &= component.second->initialise();
			}

			m_dirty = !m_initialised;
		}
	}

	void Submesh::cleanup()
	{
		m_initialised = false;

		for ( auto & component : m_components )
		{
			component.second->cleanup();
		}

		m_vertexBuffer.cleanup();

		if ( !m_indexBuffer.isEmpty() )
		{
			m_indexBuffer.cleanup();
		}

		m_points.clear();
		m_pointsData.clear();
	}

	void Submesh::computeContainers()
	{
		if ( !m_pointsData.empty() )
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

	void Submesh::updateContainers( castor::BoundingBox const & boundingBox )
	{
		m_box = boundingBox;
		m_sphere.load( m_box );
	}

	uint32_t Submesh::getFaceCount()const
	{
		uint32_t result = 0u;
		
		if ( m_indexMapping )
		{
			result = m_indexMapping->getCount();
		}
		else
		{
			result = getPointsCount();

			switch ( getTopology() )
			{
			case Topology::ePoints:
				break;

			case Topology::eLines:
				result = result / 2;
				break;

			case Topology::eLineLoop:
				break;

			case Topology::eLineStrip:
				result = result - 1u;
				break;

			case Topology::eTriangles:
				result = result / 3u;
				break;

			case Topology::eTriangleStrips:
			case Topology::eTriangleFan:
				result = result - 2u;
				break;
			}
		}

		return result;
	}

	uint32_t Submesh::getPointsCount()const
	{
		return std::max< uint32_t >( uint32_t( m_points.size() )
			, m_vertexBuffer.getSize() / m_vertexBuffer.getDeclaration().stride() );
	}

	int Submesh::isInMyPoints( Point3r const & vertex
		, double precision )
	{
		int result = -1;
		int index = 0;
		Point3r pos;

		for ( auto it = m_points.begin(); it != m_points.end() && result == -1; ++it )
		{
			if ( point::lengthSquared( vertex - Vertex::getPosition( ( *it ), pos ) ) < precision )
			{
				result = int( index );
			}

			index++;
		}

		return result;
	}

	BufferElementGroupSPtr Submesh::addPoint( real x, real y, real z )
	{
		BufferElementGroupSPtr result;
		uint32_t stride = 3 * sizeof( real ) * 5;
		m_pointsData.emplace_back( stride );
		uint8_t * data = m_pointsData.back().data();
		result = std::make_shared< BufferElementGroup >( data, uint32_t( m_points.size() ) );
		Vertex::setPosition( result, x, y, z );
		m_points.push_back( result );
		return result;
	}

	BufferElementGroupSPtr Submesh::addPoint( Point3r const & value )
	{
		return addPoint( value[0], value[1], value[2] );
	}

	BufferElementGroupSPtr Submesh::addPoint( real * value )
	{
		return addPoint( value[0], value[1], value[2] );
	}

	void Submesh::addPoints( InterleavedVertex const * const begin
		, InterleavedVertex const * const end )
	{
		uint32_t stride = m_vertexBuffer.getDeclaration().stride();
		m_pointsData.emplace_back( std::distance( begin, end ) * stride );
		uint8_t * data = m_pointsData.back().data();

		std::for_each( begin, end, [this, &data, &stride]( InterleavedVertex const & p_data )
		{
			auto vertex = std::make_shared< BufferElementGroup >( data, uint32_t( m_points.size() ) );
			Vertex::setPosition( vertex, p_data.m_pos.data() );
			Vertex::setNormal( vertex, p_data.m_nml.data() );
			Vertex::setTangent( vertex, p_data.m_tan.data() );
			Vertex::setBitangent( vertex, p_data.m_bin.data() );
			Vertex::setTexCoord( vertex, p_data.m_tex.data() );
			m_points.push_back( vertex );
			data += stride;
		} );
	}

	void Submesh::draw( GeometryBuffers const & geometryBuffers )
	{
		REQUIRE( m_initialised );

		if ( m_dirty )
		{
			m_vertexBuffer.upload();
			m_dirty = false;
		}

		for ( auto & component : m_components )
		{
			component.second->upload();
		}

		if ( !m_indexBuffer.isEmpty() )
		{
			geometryBuffers.draw( m_indexBuffer.getSize()
				, m_indexBuffer.getOffset() );
		}
		else
		{
			geometryBuffers.draw( m_vertexBuffer.getSize() / m_vertexBuffer.getDeclaration().stride()
				, 0u );
		}
	}

	void Submesh::drawInstanced( GeometryBuffers const & geometryBuffers
		, uint32_t count )
	{
		REQUIRE( m_initialised );

		if ( m_dirty )
		{
			m_vertexBuffer.upload();
			m_dirty = false;
		}

		for ( auto & component : m_components )
		{
			component.second->upload();
		}

		if ( !m_indexBuffer.isEmpty() )
		{
			geometryBuffers.drawInstanced( m_indexBuffer.getSize()
				, m_indexBuffer.getOffset()
				, count );
		}
		else
		{
			geometryBuffers.drawInstanced( m_vertexBuffer.getSize() / m_vertexBuffer.getDeclaration().stride()
				, 0u
				, count );
		}
	}

	void Submesh::computeNormals( bool reverted )
	{
		if ( m_indexMapping )
		{
			m_indexMapping->computeNormals( reverted );
		}
	}

	void Submesh::sortByDistance( castor::Point3r const & cameraPosition )
	{
		if ( m_indexMapping )
		{
			m_indexMapping->sortByDistance( cameraPosition );
		}
	}

	ProgramFlags Submesh::getProgramFlags()const
	{
		auto result = m_programFlags;

		for ( auto & component : m_components )
		{
			result |= component.second->getProgramFlags();
		}

		return result;
	}

	void Submesh::setMaterial( MaterialSPtr oldMaterial
		, MaterialSPtr newMaterial
		, bool update )
	{
		if ( oldMaterial != newMaterial )
		{
			getScene()->setChanged();
		}

		for ( auto & component : m_components )
		{
			component.second->setMaterial( oldMaterial, newMaterial, update );
		}
	}

	void Submesh::gatherBuffers( VertexBufferArray & buffers )
	{
		buffers.emplace_back( m_vertexBuffer );

		for ( auto & component : m_components )
		{
			component.second->gather( buffers );
		}
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
				REQUIRE( buffer < vertexBuffer.getData() + vertexBuffer.getSize() );
				REQUIRE( ( buffer < vertexBuffer.getData() + vertexBuffer.getSize()
						&& buffer + it.size() <= vertexBuffer.getData() + vertexBuffer.getSize() )
					|| ( buffer == vertexBuffer.getData() + vertexBuffer.getSize() ) );
				std::memcpy( buffer, it.data(), it.size() );
				buffer += it.size();
			}

			buffer = vertexBuffer.getData();

			for ( auto point : m_points )
			{
				REQUIRE( buffer < vertexBuffer.getData() + vertexBuffer.getSize() );
				REQUIRE( ( buffer < vertexBuffer.getData() + vertexBuffer.getSize()
						&& buffer + stride <= vertexBuffer.getData() + vertexBuffer.getSize() )
					|| ( buffer == vertexBuffer.getData() + vertexBuffer.getSize() ) );
				point->linkCoords( buffer );
				buffer += stride;
			}

			//m_points.clear();
			//m_pointsData.clear();
		}
	}
}
