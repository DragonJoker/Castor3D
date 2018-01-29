#include "Submesh.hpp"

#include "SubmeshUtils.hpp"

#include "Engine.hpp"

#include "Event/Frame/FrameListener.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Mesh/SubmeshComponent/BonesComponent.hpp"
#include "Mesh/SubmeshComponent/InstantiationComponent.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/Scene.hpp"
#include "Vertex.hpp"

#include <Core/Device.hpp>

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
			auto count = obj.getPointsCount();
			result = doWriteChunk( count, ChunkType::eSubmeshVertexCount, m_chunk );

			if ( result )
			{
				InterleavedVertex const * srcbuf = reinterpret_cast< InterleavedVertex const * >( obj.getPoints().data() );
				std::vector< InterleavedVertexT< double > > dstbuf( count );
				doCopyVertices( count, srcbuf, dstbuf.data() );
				result = doWriteChunk( dstbuf, ChunkType::eSubmeshVertex, m_chunk );
			}
		}

		if ( result && obj.hasComponent( TriFaceMapping::Name ) )
		{
			uint32_t count = obj.getFaceCount();
			result = doWriteChunk( count, ChunkType::eSubmeshFaceCount, m_chunk );

			if ( result )
			{
				FaceIndices const * srcbuf = reinterpret_cast< FaceIndices const * >( obj.getComponent< TriFaceMapping >()->getFaces().data() );
				result = doWriteChunk( srcbuf, count, ChunkType::eSubmeshFaces, m_chunk );
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
			auto & device = *getScene()->getEngine()->getRenderSystem()->getCurrentDevice();
			m_indexBuffer = renderer::makeBuffer< uint32_t >( device
				, m_indexMapping->getCount() * m_indexMapping->getComponentsCount()
				, renderer::BufferTarget::eIndexBuffer
				, renderer::MemoryPropertyFlag::eHostVisible );

			for ( auto & component : m_components )
			{
				component.second->initialise();
				component.second->fill();
			}

			m_generated = true;
		}

		if ( !m_initialised )
		{
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

		m_vertexBuffer.reset();
		m_indexBuffer.reset();

		m_points.clear();
	}

	void Submesh::update()
	{
		if ( m_dirty )
		{
			uint32_t size = uint32_t( m_points.size() );

			if ( auto buffer = m_vertexBuffer->lock( 0u
				, size
				, renderer::MemoryMapFlag::eWrite ) )
			{
				std::copy( m_points.begin(), m_points.end(), buffer );
				m_vertexBuffer->unlock( size, true );
			}

			m_dirty = false;
		}

		for ( auto & component : m_components )
		{
			component.second->upload();
		}
	}

	void Submesh::computeContainers()
	{
		if ( !m_points.empty() )
		{
			Point3r min = m_points[0].m_pos;
			Point3r max = m_points[0].m_pos;
			uint32_t nbVertex = getPointsCount();

			for ( uint32_t i = 1; i < nbVertex; i++ )
			{
				Point3r cur = m_points[i].m_pos;
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
			case renderer::PrimitiveTopology::ePointList:
				break;

			case renderer::PrimitiveTopology::eLineList:
				result /= 2;
				break;

			case renderer::PrimitiveTopology::eLineStrip:
				result -= 1u;
				break;

			case renderer::PrimitiveTopology::eTriangleList:
				result /= 3u;
				break;

			case renderer::PrimitiveTopology::eTriangleStrip:
			case renderer::PrimitiveTopology::eTriangleFan:
				result -= 2u;
				break;
			}
		}

		return result;
	}

	uint32_t Submesh::getPointsCount()const
	{
		return std::max< uint32_t >( uint32_t( m_points.size() )
			, m_vertexBuffer->getCount() );
	}

	int Submesh::isInMyPoints( Point3r const & vertex
		, double precision )
	{
		int result = -1;
		int index = 0;

		for ( auto it = m_points.begin(); it != m_points.end() && result == -1; ++it )
		{
			if ( point::distanceSquared( vertex, it->m_pos ) < precision )
			{
				result = int( index );
			}

			index++;
		}

		return result;
	}

	InterleavedVertex Submesh::addPoint( real x, real y, real z )
	{
		InterleavedVertex result;
		result.m_pos = Point3f{ x, y, z };
		addPoint( result );
		return result;
	}

	InterleavedVertex Submesh::addPoint( Point3r const & value )
	{
		return addPoint( value[0], value[1], value[2] );
	}

	InterleavedVertex Submesh::addPoint( real * value )
	{
		return addPoint( value[0], value[1], value[2] );
	}

	void Submesh::addPoint( InterleavedVertex const & vertex )
	{
		m_points.push_back( vertex );
	}

	void Submesh::addPoints( InterleavedVertex const * const begin
		, InterleavedVertex const * const end )
	{
		m_points.insert( m_points.end(), begin, end );
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

	void Submesh::gatherBuffers( renderer::VertexBufferCRefArray & buffers )
	{
		buffers.emplace_back( *m_vertexBuffer );

		for ( auto & component : m_components )
		{
			component.second->gather( buffers );
		}
	}

	void Submesh::doGenerateVertexBuffer()
	{
		auto & device = *getScene()->getEngine()->getRenderSystem()->getCurrentDevice();
		uint32_t stride = sizeof( InterleavedVertex );
		uint32_t size = uint32_t( m_points.size() );

		if ( size )
		{
			if ( !m_vertexBuffer
				|| size != m_vertexBuffer->getCount() )
			{
				m_vertexBuffer = renderer::makeVertexBuffer< InterleavedVertex >( device
					, size
					, renderer::BufferTarget::eVertexBuffer
					, renderer::MemoryPropertyFlag::eHostVisible );
			}

			if ( auto buffer = m_vertexBuffer->lock( 0u
				, size
				, renderer::MemoryMapFlag::eWrite ) )
			{
				std::copy( m_points.begin(), m_points.end(), buffer );
				m_vertexBuffer->unlock( size, true );
			}

			//m_points.clear();
		}
	}
}
