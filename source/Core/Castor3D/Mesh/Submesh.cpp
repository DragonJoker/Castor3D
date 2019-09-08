#include "Castor3D/Mesh/Submesh.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Mesh/SubmeshUtils.hpp"
#include "Castor3D/Mesh/Vertex.hpp"
#include "Castor3D/Mesh/SubmeshComponent/BonesComponent.hpp"
#include "Castor3D/Mesh/SubmeshComponent/InstantiationComponent.hpp"
#include "Castor3D/Mesh/SubmeshComponent/TriFaceMapping.hpp"
#include "Castor3D/Mesh/SubmeshComponent/LinesMapping.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <ashespp/Core/Device.hpp>

using namespace castor;

namespace castor3d
{
	Submesh::Submesh( Scene & scene, Mesh & mesh, uint32_t id )
		: OwnedBy< Scene >{ scene }
		, m_parentMesh{ mesh }
		, m_id{ id }
		, m_defaultMaterial{ getScene()->getEngine()->getMaterialCache().getDefaultMaterial() }
	{
		addComponent( std::make_shared< InstantiationComponent >( *this, 2u ) );
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
			auto & device = getCurrentRenderDevice( *this );
			m_indexBuffer = makeBuffer< uint32_t >( device
				, m_indexMapping->getCount() * m_indexMapping->getComponentsCount()
				, VK_BUFFER_USAGE_INDEX_BUFFER_BIT
				, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
				, m_parentMesh.getName() + "Submesh" + castor::string::toString( m_id ) + "IndexBuffer" );

			for ( auto & component : m_components )
			{
				component.second->initialise();
				component.second->fill();
				component.second->upload();
			}

			if ( !m_vertexLayout )
			{
				m_vertexLayout = std::make_unique< ashes::PipelineVertexInputStateCreateInfo >( 0u
					, ashes::VkVertexInputBindingDescriptionArray
					{
						{ 0u, sizeof( InterleavedVertex ), VK_VERTEX_INPUT_RATE_VERTEX },
					}
					, ashes::VkVertexInputAttributeDescriptionArray
					{
						{ RenderPass::VertexInputs::PositionLocation, 0u, VK_FORMAT_R32G32B32_SFLOAT, offsetof( InterleavedVertex, pos ) },
						{ RenderPass::VertexInputs::NormalLocation, 0u, VK_FORMAT_R32G32B32_SFLOAT, offsetof( InterleavedVertex, nml ) },
						{ RenderPass::VertexInputs::TangentLocation, 0u, VK_FORMAT_R32G32B32_SFLOAT, offsetof( InterleavedVertex, tan ) },
						{ RenderPass::VertexInputs::TextureLocation, 0u, VK_FORMAT_R32G32B32_SFLOAT, offsetof( InterleavedVertex, tex ) },
					} );
			}

			m_generated = true;
		}

		if ( !m_initialised )
		{
			m_initialised = true;

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
			auto size = uint32_t( m_points.size() );

			if ( auto buffer = m_vertexBuffer->lock( 0u
				, size
				, 0u ) )
			{
				std::copy( m_points.begin(), m_points.end(), buffer );
				m_vertexBuffer->flush( 0u, size );
				m_vertexBuffer->unlock();
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
			Point3r min = m_points[0].pos;
			Point3r max = m_points[0].pos;
			uint32_t nbVertex = getPointsCount();

			for ( uint32_t i = 1; i < nbVertex; i++ )
			{
				Point3r cur = m_points[i].pos;
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
			case VK_PRIMITIVE_TOPOLOGY_POINT_LIST:
				break;

			case VK_PRIMITIVE_TOPOLOGY_LINE_LIST:
				result /= 2;
				break;

			case VK_PRIMITIVE_TOPOLOGY_LINE_STRIP:
				result -= 1u;
				break;

			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST:
				result /= 3u;
				break;

			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP:
			case VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN:
				result -= 2u;
				break;
			}
		}

		return result;
	}

	uint32_t Submesh::getPointsCount()const
	{
		return std::max< uint32_t >( uint32_t( m_points.size() )
			, ( m_vertexBuffer ? uint32_t( m_vertexBuffer->getCount() ) : 0u ) );
	}

	int Submesh::isInMyPoints( Point3r const & vertex
		, double precision )
	{
		int result = -1;
		int index = 0;

		for ( auto it = m_points.begin(); it != m_points.end() && result == -1; ++it )
		{
			if ( point::distanceSquared( vertex, it->pos ) < precision )
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
		result.pos = Point3f{ x, y, z };
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

	ProgramFlags Submesh::getProgramFlags( MaterialSPtr material )const
	{
		auto result = m_programFlags;

		for ( auto & component : m_components )
		{
			result |= component.second->getProgramFlags( material );
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

	GeometryBuffers const & Submesh::getGeometryBuffers( MaterialSPtr material )const
	{
		auto it = m_geometryBuffers.find( material );

		if ( it == m_geometryBuffers.end() )
		{
			ashes::BufferCRefArray buffers;
			ashes::UInt64Array offsets;
			ashes::PipelineVertexInputStateCreateInfoCRefArray layouts;
			buffers.emplace_back( m_vertexBuffer->getBuffer() );
			offsets.emplace_back( 0u );
			layouts.emplace_back( *m_vertexLayout );

			for ( auto & component : m_components )
			{
				component.second->gather( material, buffers, offsets, layouts );
			}

			GeometryBuffers result;
			result.vbo = buffers;
			result.vboOffsets = offsets;
			result.layouts = layouts;
			result.ibo = &m_indexBuffer->getBuffer();
			result.iboOffset = 0u;
			result.idxCount = uint32_t( m_indexBuffer->getCount() );
			result.vtxCount = 0u;
			it = m_geometryBuffers.emplace( material, std::move( result ) ).first;
		}

		return it->second;
	}

	void Submesh::doGenerateVertexBuffer()
	{
		auto & device = getCurrentRenderDevice( *this );
		uint32_t size = uint32_t( m_points.size() );

		if ( size )
		{
			if ( !m_vertexBuffer
				|| size != m_vertexBuffer->getCount() )
			{
				m_vertexBuffer = makeVertexBuffer< InterleavedVertex >( device
					, size
					, 0u
					, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
					, m_parentMesh.getName() + "Submesh" + castor::string::toString( m_id ) + "VertexBuffer" );
			}

			if ( auto buffer = m_vertexBuffer->getBuffer().lock( 0u
				, ~( 0ull )
				, 0u ) )
			{
				std::memcpy( buffer, m_points.data(), m_points.size() * sizeof( InterleavedVertex ) );
				m_vertexBuffer->getBuffer().flush( 0u, ~( 0ull ) );
				m_vertexBuffer->getBuffer().unlock();
			}

			//m_points.clear();
		}
	}
}
